#ifndef SAMPLE_H
#define SAMPLE_H

#include <string>

#include "catalog.h"
#include "options.h"

namespace data
{

/**
 * \brief Represents a data sample.
 *
 * Objects of this class can be loaded from the configuration file using \ref load.
 */
class sample
{
  public:
    /// \brief Identifies data, MC and background samples
    enum class type {
        data,      ///< \brief Data sample
        mc,        ///< \brief MC sample
        background ///< \brief Background sample
    };

  private:
    friend struct YAML::convert<data::sample>;

    type _type;
    std::string _bonzai_dir;
    std::string _catalog;
    std::string _name;

  public:
    /// \brief Retrives the catalog for this sample.
    data::catalog catalog() const;

    /// \brief Retrives the name of the sample.
    std::string name() const { return _name; }

    /// \brief Retrives the \ref type of the sample.
    enum type type() const { return _type; }

    /// \brief Loads the list of samples from the configuration file.
    static std::vector<sample> load(const util::options &opt);
};
} // namespace data

#endif // SAMPLE_H
