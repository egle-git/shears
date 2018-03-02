#ifndef MC_GROUP_H
#define MC_GROUP_H

#include <memory>
#include <set>
#include <string>

#include "sample.h"

class TFile;
class TH1;

namespace data
{

/// \brief Describes a group of MC samples that will be added together.
class mc_group
{
  private:
    friend struct YAML::convert<data::mc_group>;

    // Always available
    bool _required = false;
    double _scale_factor = 1;
    int _color;
    std::string _legend;
    std::vector<std::string> _sample_names;
    std::vector<sample> _samples;
    std::vector<std::shared_ptr<TFile>> _files;

  public:
    /// \brief Returns whether the group is required.
    bool required() const { return _required; }

    /// \brief Returns the group color (using \c ROOT conventions).
    int color() const { return _color; }

    /// \brief Returns the group legend.
    std::string legend() const { return _legend; }

    /// \brief Adds the name of all available histograms to \c histos
    void add_histograms(std::set<std::string> &histos);

    /**
     * \brief Retrieves the histogram named \c name, or \c nullptr if it's not available
     *
     * The histogram will be ready for drawing.
     */
    TH1 *get(const std::string &name);

    /// \brief Loads the list of groups from the configuration file.
    static std::vector<mc_group> load(const util::options &opt,
                                      const std::vector<data::sample> &all_samples);

  private:
    void init(const std::vector<sample> &all_samples);
};
}

#endif // MC_GROUP_H
