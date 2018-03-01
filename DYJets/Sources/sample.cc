#include "sample.h"

#include <stdexcept>

namespace data
{

catalog sample::catalog() const { return data::catalog(_catalog, _bonzai_dir); }

std::vector<sample> sample::load(const util::options &opt)
{
    return opt.config["samples"].as<std::vector<sample>>();
}
} // namespace data

/// \cond
namespace YAML
{

template <> struct convert<data::sample>
{
    static bool decode(const Node &node, data::sample &sample)
    {
        if (!node["name"]) {
            throw std::runtime_error("Sample name is not set");
        }
        sample._name = node["name"].as<std::string>();

        if (!node["bonzai dir"]) {
            throw std::runtime_error("Sample bonzai dir is not set for " + sample._name);
        }
        sample._bonzai_dir = node["bonzai dir"].as<std::string>();

        if (!node["catalog"]) {
            throw std::runtime_error("Sample catalog is not set for " + sample._name);
        }
        sample._catalog = node["catalog"].as<std::string>();

        if (!node["type"]) {
            throw std::runtime_error("Sample type is not set for " + sample._name);
        }
        std::string type = node["type"].as<std::string>();
        if (type == "data") {
            sample._type = data::sample::type::data;
        } else if (type == "MC") {
            sample._type = data::sample::type::mc;
        } else if (type == "background") {
            sample._type = data::sample::type::background;
        } else {
            throw std::runtime_error("Sample type is not \"data\" or \"MC\" for " + sample._name);
        }

        return true;
    }
};
} // namespace YAML
/// \endcond
