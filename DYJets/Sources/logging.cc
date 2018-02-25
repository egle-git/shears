#include "logging.h"

#include <cstdio> // fileno, stdin
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <unistd.h> // isatty

#include <boost/iostreams/device/null.hpp>
#include <boost/iostreams/filter/line.hpp>
#include <boost/iostreams/tee.hpp>
#include <boost/ref.hpp>

#include <TError.h>

#include <yaml-cpp/yaml.h>

#include "ansi_seq.h"

namespace logging
{

// Set streams to non-null sinks in case the user doesn't call init()
stream debug(boost::ref(std::cerr));
stream info(boost::ref(std::cerr));
stream warn(boost::ref(std::cerr));
stream error(boost::ref(std::cerr));
stream fatal(boost::ref(std::cerr));

namespace /* anonymous */
{
level str_to_level(const std::string &str)
{
    if (str == "debug") {
        return level::debug;
    } else if (str == "info") {
        return level::info;
    } else if (str == "warn") {
        return level::warn;
    } else if (str == "error") {
        return level::error;
    } else if (str == "fatal") {
        return level::fatal;
    } else {
        error << "Invalid log level: " << str << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
} // namespace anonymous

settings &settings::operator<<(const YAML::Node &node)
{
    if (node["color"].IsScalar()) {
        try {
            bool enabled = node["color"].as<bool>();
            color = enabled ? color_mode::enabled : color_mode::disabled;
        } catch (...) {
            std::string strval = node["color"].as<std::string>();
            if (strval == "auto") {
                color = color_mode::autodetect;
            } else {
                error << "Invalid color mode: " << strval << std::endl;
                std::exit(EXIT_FAILURE);
            }
        }
    }
    if (node["log level"].IsScalar()) {
        screen_level = str_to_level(node["log level"].as<std::string>());
    }
    if (node["log file"].IsScalar()) {
        log_file = node["log file level"].as<std::string>();
    }
    if (node["log file level"].IsScalar()) {
        log_file_level = str_to_level(node["log file level"].as<std::string>());
    }
    if (node["override root handler"].IsScalar()) {
        try {
            override_root_handler = node["override root handler"].as<bool>();
        } catch (...) {

            error << "Invalid bool: " << node["override root handler"].as<std::string>()
                  << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }
    return *this;
}

namespace /* anonymous */
{

class label_filter : public boost::iostreams::line_filter
{
    std::string _prefix;

  public:
    explicit label_filter(const std::string &prefix) : _prefix(prefix) {}
  private:
    std::string do_filter(const std::string &line) { return "[" + _prefix + "] " + line; }
};

std::string level_string(level l, color_mode color = color_mode::autodetect)
{
    if (color == color_mode::enabled) {
        using namespace ansi;

        switch (l) {
        case level::debug:
            return "DEBUG";
        case level::info:
            return setcolor(cyan) + "INFO " + reset();
        case level::warn:
            return setcolor(bright_magenta) + "WARN " + reset();
        case level::error:
            return setcolor(bright_red) + "ERROR" + reset();
        case level::fatal:
            return setcolor(bright_yellow) + setcolor(red, background) + "FATAL" + reset();
        }
    } else {
        switch (l) {
        case level::debug:
            return "DEBUG";
        case level::info:
            return "INFO ";
        case level::warn:
            return "WARN ";
        case level::error:
            return "ERROR";
        case level::fatal:
            return "FATAL";
        }
    }
    return "";
}

void push_chain(stream &log_stream,
                level stream_level,
                level min_level,
                bool label,
                color_mode color,
                std::ostream &out)
{
    if (label) {
        log_stream.push(label_filter(level_string(stream_level, color)));
    }
    if (stream_level >= min_level) {
        log_stream.push(boost::ref(out));
    } else {
        log_stream.push(boost::iostreams::null_sink());
    }
}

std::ofstream *fileout = nullptr;

stream *debug_fs = nullptr;
stream *info_fs = nullptr;
stream *warn_fs = nullptr;
stream *error_fs = nullptr;
stream *fatal_fs = nullptr;

void root_error_handler(int level, bool abort, const char *location, const char *msg)
{
    if (level < kInfo) {
        debug << location << ": " << msg << std::endl;
    } else if (level < kWarning) {
        info << location << ": " << msg << std::endl;
    } else if (level < kError) {
        warn << location << ": " << msg << std::endl;
    } else if (level < kBreak) {
        error << location << ": " << msg << std::endl;
    } else {
        fatal << location << ": " << msg << std::endl;
    }
    if (abort) {
        std::terminate();
    }
}

} // namespace anonymous

void init(const struct settings &settings)
{
    // Reset chains
    debug.reset();
    info.reset();
    warn.reset();
    error.reset();
    fatal.reset();

    color_mode color = settings.color;
    if (color == color_mode::autodetect) {
        color = isatty(fileno(stderr)) ? color_mode::enabled : color_mode::disabled;
    }

    // Create file output streams
    if (!settings.log_file.empty()) {
        fileout = new std::ofstream(settings.log_file);

        debug_fs = new stream;
        push_chain(*debug_fs,
                   level::debug,
                   settings.log_file_level,
                   settings.prepend_label,
                   color_mode::disabled,
                   *fileout);
        debug.push(tee(boost::ref(*debug_fs)));

        info_fs = new stream;
        push_chain(*info_fs,
                   level::info,
                   settings.log_file_level,
                   settings.prepend_label,
                   color_mode::disabled,
                   *fileout);
        info.push(tee(boost::ref(*info_fs)));

        warn_fs = new stream;
        push_chain(*warn_fs,
                   level::warn,
                   settings.log_file_level,
                   settings.prepend_label,
                   color_mode::disabled,
                   *fileout);
        warn.push(tee(boost::ref(*warn_fs)));

        error_fs = new stream;
        push_chain(*error_fs,
                   level::error,
                   settings.log_file_level,
                   settings.prepend_label,
                   color_mode::disabled,
                   *fileout);
        error.push(tee(boost::ref(*error_fs)));

        fatal_fs = new stream;
        push_chain(*fatal_fs,
                   level::fatal,
                   settings.log_file_level,
                   settings.prepend_label,
                   color_mode::disabled,
                   *fileout);
        fatal.push(tee(boost::ref(*fatal_fs)));
    }

    // Create cerr output streams
    push_chain(
        debug, level::debug, settings.screen_level, settings.prepend_label, color, std::cerr);
    push_chain(info, level::info, settings.screen_level, settings.prepend_label, color, std::cerr);
    push_chain(warn, level::warn, settings.screen_level, settings.prepend_label, color, std::cerr);
    push_chain(
        error, level::error, settings.screen_level, settings.prepend_label, color, std::cerr);
    push_chain(
        fatal, level::fatal, settings.screen_level, settings.prepend_label, color, std::cerr);

    // Override ROOT error settings
    if (settings.override_root_handler) {
        SetErrorHandler(root_error_handler);
    }
}

void close()
{
    if (fileout != nullptr) {
        fileout->close();
        fileout = nullptr;
    }
}

} // namespace logging
