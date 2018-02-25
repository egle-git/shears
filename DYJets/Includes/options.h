#ifndef OPTIONS_H
#define OPTIONS_H

#include <iostream>

#include <boost/algorithm/string/replace.hpp>
#include <boost/program_options/errors.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <yaml-cpp/yaml.h>

#include "logging.h"

namespace po = boost::program_options;

class options
{
    std::string _prog_name;
    po::options_description _all;

  public:
    YAML::Node config;
    std::string _config_file;

    po::variables_map map;

    virtual ~options() = default;

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

/**
 * \brief Generally useful classes and functions.
 *
 * This namespace is meant to group utilities that have little physics interest.
 */
namespace util
{

/**
 * \brief Transforms the name to match the convention for the given container.
 *
 * The default implementation returns its argument unchanged.
 */
template <class Container> std::string name_for(const std::string &name) { return name; }

/**
 * \brief Transforms the name to match the convention for the command line.
 *
 * This specialization turns whitespaces into dashes.
 */
template <> std::string name_for<po::variables_map>(const std::string &name);

/**
 * \brief Checks whether a value with `name` is present in the given `Container`.
 *
 * This function has specialization for \c YAML::Node and \c po::variables_map.
 */
template <class Container> bool is_present(const std::string &name, const Container &);

/**
 * \brief Sets the value of \c target according to the contents of \c container.
 *
 * If the value isn't present in the container, this function doesn't do anything. Else, it tries
 * to convert it to the right type, and to validate it using `check`. An exception is thrown in
 * case of error.
 *
 * \param container The container to get the value from.
 * \param target    The variable to store the value into.
 * \param name      The name of the variable. It will be translated using \ref name_for.
 * \param descr     A user-readable string describing the variable. Used for output.
 * \param check     A function that takes a value and returns \c true if it is valid.
 */
template <class Type, class Container, class Checker>
void set_value_safe(const Container &container,
                    Type &target,
                    const std::string &name,
                    const std::string &descr,
                    const Checker &check)
{
    std::string name_tr = name_for<Container>(name);
    if (is_present(name_tr, container)) {
        Type config_value = container[name_tr].template as<Type>();
        if (check(config_value)) {
            logging::debug << "Setting " << descr << " to " << config_value << std::endl;
            target = config_value;
        } else {
            throw std::runtime_error("Invalid " + descr + ": " + std::to_string(config_value));
        }
    }
}

/**
 * \brief Sets the value of \c target according to the contents of \c container.
 *
 * Same as \ref set_value_safe, but doesn't validate the value.
 *
 * \param container The container to get the value from.
 * \param target    The variable to store the value into.
 * \param name      The name of the variable. It will be translated using \ref name_for.
 * \param descr     A user-readable string describing the variable. Used for output.
 */
template <class Type, class Container>
void set_value_safe(const Container &container,
                    Type &target,
                    const std::string &name,
                    const std::string &descr)
{
    set_value_safe(container, target, name, descr, [](Type) { return true; });
}
}

#endif // OPTIONS_H
