#ifndef STYLE_LIST_H
#define STYLE_LIST_H

#include <regex>
#include <string>

#include <yaml-cpp/yaml.h>

namespace util
{

/// \brief A poor man's style sheet system
class style_list
{
    struct rule
    {
        std::regex regex;
        YAML::Node node;
    };
    friend YAML::convert<rule>;

    std::vector<rule> _rules;

  public:
    /// \brief Constructor
    explicit style_list(const YAML::Node &specification);

    /// \brief Destructor
    virtual ~style_list() = default;

    /**
     * \brief Gets the value of a field according to the specification.
     *
     * \arg name The name of the field
     * \arg rule_match The string to match to the specification's rules
     * \arg default_value The default value
     */
    template <class T>
    T get(const std::string &name,
          const std::string &rule_match,
          const T &default_value = T()) const;
};

template <class T>
T style_list::get(const std::string &name,
                  const std::string &rule_match,
                  const T &default_value) const
{
    for (auto it = _rules.crbegin(); it != _rules.crend(); ++it) {
        if (it->node[name] && std::regex_match(rule_match, it->regex)) {
            return it->node[name].as<T>();
        }
    }
    return default_value;
}

} // namespace util

#endif // STYLE_LIST_H
