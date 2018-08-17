#ifndef COMPARE_BUILDER_BASE_h
#define COMPARE_BUILDER_BASE_h

#include <memory>
#include <set>
#include <string>

#include "comparison_entry.h"
#include "style_list.h"

namespace po = boost::program_options;

namespace util
{

class compare_builder_base
{
    std::string _analyzer_name;
    std::string _default_config_file;
    std::string _output_dir_name;
    std::set<std::string> _histogram_names;

    util::options _opt;
    util::style_list _style;

  public:
    /// \brief Constructor
    explicit compare_builder_base(const std::string &analyzer_name,
                                  const std::string &default_config_file,
                                  const std::string &default_output_dir);

    /// \brief Destructor
    virtual ~compare_builder_base() = default;

    /// \brief Parses options passed to the program
    void parse_options(int argc, char **argv);

    /// \brief Make all plots
    void build();

  protected:
    /// \brief Loads data samples
    std::unique_ptr<data::data_comparison_entry> load_data(const std::string &input_dir);

    /// \brief Loads MC samples
    std::unique_ptr<data::mc_comparison_entry> load_mc(const std::string &input_dir);

    /// \brief Returns the options supported by this builder
    virtual po::options_description  options() = 0;

    /// \brief Loads all needed \ref comparison_entry
    virtual void load() = 0;

    /// \brief Fills the upper panel with plots
    virtual void fill_upper_panel() = 0;

    /// \brief Fills the legend with plots
    virtual void fill_legend_panel() = 0;

    /// \brief Fills the lower panel with plots
    virtual void fill_lower_panel() = 0;

    /// \brief Resets drawing state after a plot was made
    virtual void reset_drawing_state() = 0;

  private:
    /// \brief Creates the directory results are written to
    void create_output_dir() const;

    /// \brief Filters the \c _histogram_names set to only contain plots that should be created
    void filter_histogram_names();
};

} // namespace util

#endif // COMPARE_BUILDER_BASE_h
