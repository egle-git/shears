#ifndef OS_SS_RATIO_COMPARE_BUILDER_H
#define OS_SS_RATIO_COMPARE_BUILDER_H

#include <memory>
#include <set>
#include <string>

#include "comparison_entry.h"
#include "style_list.h"
#include "zfinder.h"

class TAxis;
class TLegend;
class TPad;

class TH1;
class TFile;

namespace util
{

class os_ss_ratio_compare_builder
{
    std::string _analyzer_name;
    std::string _default_config_file;
    std::string _output_dir_name;
    std::string _output_format;
    std::set<std::string> _histogram_names;

    // To check which channel we are using
    std::unique_ptr<physics::zfinder> _zfinder;

    std::unique_ptr<data::data_comparison_entry> _os_data_entry;
    std::unique_ptr<data::mc_comparison_entry> _os_mc_entry;
    std::unique_ptr<data::data_comparison_entry> _ss_data_entry;
    std::unique_ptr<data::mc_comparison_entry> _ss_mc_entry;
    std::unique_ptr<TH1> _fakes_est_os;
    std::unique_ptr<TH1> _fakes_est_ss;
    std::unique_ptr<TH1> _ratio;

    util::options _opt;
    util::style_list _style;
    bool _preliminary;
    double _lumi;
    bool _logx;

    std::string _current_histo_name;

  public:
    /// \brief Constructor
    explicit os_ss_ratio_compare_builder(const std::string &analyzer_name);

    /// \brief Destructor
    virtual ~os_ss_ratio_compare_builder() = default;

    /// \brief Parses options passed to the program
    void parse_options(int argc, char **argv);

    /**
     * \brief Sets the default output directory
     *
     * It can be overriden using command line option \c -o.
     */
    void set_default_output_dir(const std::string &output)
    {
        if (_output_dir_name.empty()) {
            _output_dir_name = output;
        }
    }

    /// \brief Make all plots
    void build();

    /// \brief Gives access to the options object
    const util::options &parsed_options() const { return _opt; }

  protected:
    /// \brief Loads data samples
    std::unique_ptr<data::data_comparison_entry> load_data(const std::string &input_dir);

    /// \brief Loads MC samples
    std::unique_ptr<data::mc_comparison_entry> load_mc(const std::string &input_dir,
                                                       bool keep_signal = true,
                                                       bool keep_background = true,
                                                       const std::string &excluded_groups = "");

    /// \brief Gives access to the style
    const util::style_list &style() const { return _style; }

    /// \brief Formats the upper panel's x axis
    void format_upper_x_axis(TAxis &axis) const;

    /// \brief Formats the upper panel's y axis
    void format_upper_y_axis(TAxis &axis, const std::string &title = "# Events") const;

    /// \brief Formats the lower panel's x axis
    void format_lower_x_axis(TAxis &axis) const;

    /// \brief Formats the lower panel's y axis
    void format_lower_y_axis(TAxis &axis, const std::string &title = "") const;

    /// \brief Returns the options supported by this builder
    po::options_description options() const;

    /// \brief Loads all needed \ref comparison_entry
    void load();

    /// \brief Gets the luminosity
    double get_lumi() const { return _lumi; }

    /// \brief Fills the upper panel with plots
    void fill_upper_panel(const std::string &name);

    /// \brief Fills the legend with plots
    void fill_legend(TLegend &legend, const std::string &name);

    /// \brief Fills the lower panel with plots
    bool fill_lower_panel(const std::string &name);

    /// \brief Can be used to override lower \c TPad settings
    virtual void override_lower_panel_settings(TPad &) {}

    /// \brief Resets drawing state after a plot was made
    void reset_drawing_state();

    /// \brief Creates the directory results are written to
    void create_output_dir() const;

    /// \brief Filters the \c _histogram_names set to only contain plots that should be created
    void filter_histogram_names();

    /// \brief Removes negative bins from passed histogram
    void remove_negative_bins(std::unique_ptr<TH1> &hist);
};

} // namespace util

#endif // OS_SS_RATIO_COMPARE_BUILDER_H
