#include "job.h"

#include <algorithm>
#include <csignal>
#include <string>
#include <vector>

namespace util
{

job::job(const data::catalog &input)
    : _graceful_sigint(isatty(fileno(stdin)) || isatty(fileno(stdout)) || isatty(fileno(stderr))),
      _sigint_caught(false),
      _files(input.files())
{
}

std::vector<std::string> job::files() const
{
    std::size_t total = _files.size();
    std::size_t begin = total * _job_id / _job_count;
    std::size_t end = std::min(begin + _max_files, total * (_job_id + 1) / _job_count);

    assert(end <= _files.size());

    std::vector<std::string> files;
    std::copy(_files.begin() + begin, _files.begin() + end, std::back_inserter(files));
    return files;
}

void job::configure(const class options &opt)
{
    configure(opt.config["job"]);
    configure(opt.map);
}

void job::configure(const YAML::Node &node) { configure_common(node); }

void job::configure(const po::variables_map &varmap)
{
    configure_common(varmap);
    util::set_value_safe(
        varmap, _job_count, "job-count", "number of jobs", [](int val) { return val > 0; });
    util::set_value_safe(varmap, _job_id, "job-id", "job id", [&](int val) -> bool {
        return val >= 0 && val < _job_count;
    });
}

template <class Container> void job::configure_common(const Container &container)
{
    util::set_value_safe(container, _max_files, "max files", "maximum number of files");
    if (_max_files < 0) {
        _max_files = std::numeric_limits<int>::max();
    }
    util::set_value_safe(container, _max_events, "max events", "maximum number of events");
    if (_max_events < 0) {
        _max_events = std::numeric_limits<long long>::max();
    }
}

po::options_description job::options()
{
    po::options_description options("Job control options");
    options.add_options()(
        "max-events", po::value<long long>(), "Maximum number of events to read (-1 for no limit)")(
        "max-files",
        po::value<int>()->default_value(-1),
        "Maximum number of files to read (-1 for no limit)")(
        "job-id", po::value<int>()->default_value(0), "Job id (useful when running on batch)")(
        "job-count",
        po::value<int>()->default_value(1),
        "Number of jobs (useful when running on batch)");
    return options;
}

namespace
{
static job *sigint_handler_target = nullptr;
}

void job::setup_sigint_handler(class job *job)
{
    if (job == nullptr) {
        logging::debug << "Unsetting SIGINT handler for " << sigint_handler_target << std::endl;
        sigint_handler_target = nullptr;
        std::signal(SIGINT, SIG_DFL);
        return;
    }
    logging::debug << "Setting up SIGINT handler for " << job << std::endl;
    if (sigint_handler_target != nullptr) {
        logging::error << "SIGINT handler already defined." << std::endl;
        return;
    }
    sigint_handler_target = job;
    if (job != nullptr) {
        std::signal(SIGINT, sigint_handler);
    }
}

void job::sigint_handler(int)
{
    if (sigint_handler_target != nullptr) {
        sigint_handler_target->_sigint_caught = true;
    }
}
} // namespace util
