#ifndef OPTIONS_H
#define OPTIONS_H

#include <iostream>

#include <boost/program_options/errors.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <yaml-cpp/yaml.h>

namespace po = boost::program_options;

struct options
{
  private:
    std::string _prog_name;
    po::options_description _all;

  public:
    YAML::Node config;
    std::string config_file;

    po::variables_map map;

    virtual ~options();

    void add_defaults(const std::string &default_config_file);

    void print_usage();
    void parse_command_line(int argc, char **argv);

    void process_help();
    void process_config();

    void setup_logging();

    void default_init(int argc,
                      char **argv,
                      const std::string &default_config_file,
                      const std::initializer_list<po::options_description> &groups);

  private:
    void process_easter_egg();
};

#endif // OPTIONS_H
