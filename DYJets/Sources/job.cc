#include "job.h"

#include <algorithm>
#include <string>
#include <vector>

job::job(const catalog &input, const job::settings &s)
    : _max_events(s.max_events),
      _interactive(isatty(fileno(stdin)) || isatty(fileno(stdout)) || isatty(fileno(stderr)))
{
    std::vector<std::string> files = input.files();
    std::size_t total = files.size();
    std::size_t begin = total * s.job_id / s.job_count;
    std::size_t end = std::min(begin + s.max_files, total * (s.job_id + 1) / s.job_count);

    assert(end <= files.size());

    std::copy(files.begin() + begin, files.begin() + end, std::back_inserter(_files));
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
