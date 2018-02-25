#include <algorithm>
#include <cstdio> // fileno, stdin
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

#include <unistd.h> // isatty

#include <boost/algorithm/string/replace.hpp>

#include <TChain.h>
#include <TFile.h>

#include <yaml-cpp/yaml.h>

#include "ansi_seq.h"
#include "catalog.h"
#include "logging.h"
#include "options.h"
#include "timer.h"

class TChain;

namespace po = boost::program_options;

namespace /* anonymous */
{

template <class Container> std::string name_for(const std::string &name)
{
    return name;
}

template <> std::string name_for<po::variables_map>(const std::string &name)
{
    std::string res = name;
    boost::replace_all(res, " ", "-");
    return res;
}

template <class Container> bool is_present(const std::string &name, const Container &);

template <> bool is_present<YAML::Node>(const std::string &name, const YAML::Node &node)
{
    return node[name];
}

template <>
bool is_present<po::variables_map>(const std::string &name, const po::variables_map &varmap)
{
    return varmap.count(name) > 0;
}

template <class Type, class Container, class Checker>
void set_value_safe(const std::string &name,
                    const Container &container,
                    Type &target,
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

template <class Type, class Container>
void set_value_safe(const std::string &name,
                    const Container &container,
                    Type &target,
                    const std::string &descr)
{
    set_value_safe(name, container, target, descr, [](Type) { return true; });
}
}

job::settings &job::settings::operator<<(const YAML::Node &node)
{
    set_common_options(node);
    return *this;
}

job::settings &job::settings::operator<<(const po::variables_map &varmap)
{
    set_common_options(varmap);
    set_value_safe(
        "job-count", varmap, job_count, "number of jobs", [](int val) { return val > 0; });
    set_value_safe("job-id", varmap, job_id, "job id", [&](int val) -> bool {
        return val >= 0 && val < job_count;
    });
    return *this;
}

template<class Container>
void job::settings::set_common_options(const Container &container)
{
    set_value_safe("max files", container, max_files, "maximum number of files");
    if (max_files < 0) {
        max_files = std::numeric_limits<int>::max();
    }
    set_value_safe("max events", container, max_events, "maximum number of events");
    if (max_events < 0) {
        max_events = std::numeric_limits<long long>::max();
    }
}
