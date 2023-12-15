#ifndef SAME_SIGN_METHOD_BUILDER_H
#define SAME_SIGN_METHOD_BUILDER_H

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

class same_sign_method_builder
{
    std::string _analyzer_name;
    std::string _default_config_file;
    std::string _output_dir_name;
    std::string _output_format;
    std::string _current_histo_name;
    std::set<std::string> _histogram_names;

    std::unique_ptr<data::data_comparison_entry> _data_entry;
    std::unique_ptr<data::mc_comparison_entry> _mc_entry;

    util::options _opt;
    util::style_list _style;
    bool _preliminary;
    double _lumi;
    bool _logx;
    bool _logy = false;

    bool _smooth;
    int _smooth_amount;
    std::vector<std::string> _smooth_histograms = {};
    bool _rem_neg_bins;
    double _os_ss_ratio = 1.0;

  public:
    /// \brief Constructor
    explicit same_sign_method_builder(const std::string &analyzer_name);

    /// \brief Destructor
    virtual ~same_sign_method_builder() = default;

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

    /// \brief Formats the x axis
    void format_x_axis(TAxis &axis) const;

    /// \brief Formats the y axis
    void format_y_axis(TAxis &axis, const std::string &title = "Fake rate") const;

    /// \brief Returns the options supported by this builder
    po::options_description options() const;

    /// \brief Loads all needed \ref comparison_entry
    void load();

    /// \brief Gets the luminosity
    double get_lumi() const { return _lumi; }

    /// \brief Sets up the historgrams
    void setup_histos();

    void draw_histos();

    /// \brief Resets drawing state after a plot was made
    void reset_drawing_state();

    /// \brief Creates the directory results are written to
    void create_output_dir() const;

    /// \brief Filters the \c _histogram_names set to only contain plots that should be created
    void filter_histogram_names();

    /// \brief Removes negative bins from passed histogram
    void remove_negative_bins(std::unique_ptr<TH1> &hist, bool no_zeroes = false);

    /// \brief Smoothes the histogram by convolution
    void smooth(std::unique_ptr<TH1> &hist, int smooth_amount = 5);
};

} // namespace util

#endif // FAKE_BKG_EST_BUILDER_H
