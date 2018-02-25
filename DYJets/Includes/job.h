#ifndef JOB_H
#define JOB_H

#include <string>
#include <vector>

#include <boost/program_options/errors.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <TChain.h>
#include <TTreeReader.h>

#include "ansi_seq.h"
#include "catalog.h"
#include "chains.h"
#include "logging.h"
#include "timer.h"

namespace po = boost::program_options;

/**
 * \brief The core class to run analysis codes.
 *
 * This class handles all the complexity of looping on events. Its main features
 * are:
 *
 *   * Command line options handling
 *   * Running on a fraction of the data only
 *   * Division of files in a number of jobs (file-based)
 *   * Progress display (through \ref timer)
 *   * Graceful handling of exceptions thrown by the analysis code
 */
class job
{
  public:
    struct settings
    {
        settings &operator<<(const YAML::Node &node);
        settings &operator<<(const po::variables_map &varmap);

        int job_id = 0;
        int job_count = 1;
        int max_files = std::numeric_limits<int>::max();
        long long max_events = std::numeric_limits<long long>::max();

      private:
        template <class Container> void set_common_options(const Container &);
    };

  private:
    int _job_id = 0;
    int _job_count = 1;
    int _max_files = std::numeric_limits<int>::max();
    long long _max_events = std::numeric_limits<long long>::max();
    bool _fatal_exceptions = false;
    bool _interactive = false;

    std::vector<std::string> _files;

  public:
    /// \brief Constructs a job to run on files from the given catalog.
    explicit job(const catalog &input, const settings &s);

    /// \brief Retrieves the list of files that will be processed.
    std::vector<std::string> files() const { return _files; }

    /**
     * \brief Loops on data.
     *
     * This function creates an instance of the \c Analyzer class, passing a \c TTreeReader as the
     * first argument, followed by any arguments passed to this function:
     *
     * ~~~
     * new Analyser(<TTreeReader>, <args...>);
     * ~~~
     *
     * The \c Analyzer class should have the following methods:
     *
     * ~~~{.cpp}
     * <return value ignored> operator()();
     * <return value ignored> write(int);
     * ~~~
     *
     * The `operator()` method will be called for every event. `write()` will be called once at
     * the end of the processing.
     *
     * \tparam Analyzer An analyzer class.
     * \param args Arguments to pass to the analyzer constructor.
     */
    template <class Analyzer, class... Args> inline void run(Args... args);

    static po::options_description options();
};

template <class Analyzer, class... Args> void job::run(Args... args)
{
    using namespace logging;

    if (files().empty()) {
        throw std::runtime_error("No file set for input.");
    } else {
        logging::info << "Initializing reader (this can take a while)..." << std::endl;

        chains ch(files());
        TTreeReader reader(ch.events().get());

        Analyzer ana(reader, args...);

        long long count = reader.GetEntries(true);
        if (count == 0) {
            throw std::runtime_error("Input files don't appear to contain data. Is your proxy valid?");
        }
        count = std::min(_max_events, count);

        if (count <= 0) {
            warn << "Running on zero event. Skipping event loop." << std::endl;
        } else {
            info << "We will run on " << count << " events." << std::endl;
            reader.SetEntriesRange(0, count);

            bool had_exception = false;

            timer time(count);
            time.start();
            while (reader.Next()) {
                time.next();
                try {
                    ana();
                } catch (std::exception &e) {
                    had_exception = true;
                    error << "Caught exception while processing events: " << e.what() << std::endl;
                    if (_fatal_exceptions) {
                        throw;
                    }
                }
            }
            time.next(); // Reach 100%
            time.stop();

            if (had_exception) {
                warn << "Caught exceptions while processing events. Output may not be complete."
                     << std::endl;
            }
        }

        ana.write();
    }
}

#endif // JOB_H
