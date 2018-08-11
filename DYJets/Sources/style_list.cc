#include "style_list.h"

#include <stdexcept>

namespace util
{

style_list::style_list(const YAML::Node &specification)
    : _rules(specification.as<std::vector<style_list::rule>>())
{
}
}

/// \cond
namespace YAML
{

template <> struct convert<util::style_list::rule>
{
    static bool decode(const Node &node, util::style_list::rule &rule)
    {
        if (!node["selector"]) {
            throw std::runtime_error("Style list item without selector");
        }

        std::string selector = node["selector"].as<std::string>();
        rule.regex = std::regex(selector, std::regex_constants::extended);
        rule.node = node;

        return true;
    }
};
} // namespace YAML
/// \endcond
