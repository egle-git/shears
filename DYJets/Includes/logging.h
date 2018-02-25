#ifndef LOGGING_H
#define LOGGING_H

#include <boost/iostreams/filtering_stream.hpp>

/// \cond
namespace YAML
{
class Node;
}
/// \endcond

namespace logging
{

using stream = boost::iostreams::filtering_ostream;

extern stream debug;
extern stream info;
extern stream warn;
extern stream error;
extern stream fatal;

enum class level { debug, info, warn, error, fatal };

enum class color_mode { enabled, disabled, autodetect };

struct settings
{
    settings &operator<<(const YAML::Node &node);

    color_mode color = color_mode::autodetect;

    bool override_root_handler = true;

    level log_file_level = level::debug;
    std::string log_file = "";

    level screen_level = level::info;
};

void init(const struct settings &settings);
}

#endif // LOGGING_H
