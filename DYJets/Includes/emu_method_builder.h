#ifndef EMU_METHOD_BUILDER_H
#define EMU_METHOD_BUILDER_H

#include <memory>
#include <set>
#include <string>

#include "comparison_entry.h"
#include "style_list.h"

class TAxis;
class TLegend;
class TPad;

class TH1;
class TFile;

namespace util
{

class emu_method_builder
{
    std::string _analyzer_name;
    std::string _default_config_file;
    std::string _output_dir_name;
    std::string _output_format;
    std::set<std::string> _histogram_names;

    std::unique_ptr<data::data_comparison_entry> _ll_data_entry;
    std::unique_ptr<data::mc_comparison_entry> _ll_mc_entry;
    std::unique_ptr<data::data_comparison_entry> _emu_data_entry;
    std::unique_ptr<data::mc_comparison_entry> _emu_mc_entry;
    std::unique_ptr<data::data_comparison_entry> _emu_same_sign_data_entry;
    std::unique_ptr<data::mc_comparison_entry> _emu_same_sign_mc_entry;
    std::unique_ptr<TH1> _bkg_estimation;
    std::unique_ptr<TH1> _emu_qcd_estimation;
    std::unique_ptr<TH1> _ratio;

    util::options _opt;
    util::style_list _style;
    bool _preliminary;
    bool _reversed;
    double _lumi;
    bool _logx;
    bool _est_qcd_from_same_sign;

    std::string _current_histo_name;
    std::string _current_channel;

  public:
    /// \brief Constructor
    explicit emu_method_builder(const std::string &analyzer_name);

    /// \brief Destructor
    virtual ~emu_method_builder() = default;

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
                                                       bool keep_background = true);

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

    /// \brief Passes the MC sum histogram with WJets properly removed
    std::unique_ptr<TH1> ll_mc_get_without_wjets(const std::string name, const double lumi);

    /// \brief EMu method calculation
    std::unique_ptr<TH1> emu_method(const TH1* ll_mc_input);
};

} // namespace util

#endif // EMU_METHOD_BUILDER_H
