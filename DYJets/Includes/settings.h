#ifndef SETTINGS_H
#define SETTINGS_H

#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;

struct options
{
    po::options_description general("General options");
    po::options_description job_control("Job control options");
};

#endif // SETTINGS_H
