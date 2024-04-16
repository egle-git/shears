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

    bool _interpolate = false;
    bool _smooth;
    int _smooth_amount;
    std::vector<std::string> _smooth_histograms = {};
    bool _rem_neg_bins;
    double _os_ss_ratio = 1.0;
    std::vector<double> _mass_bins = {40, 50, 76, 106, 170, 350, 1000, 3000};

    bool _doSyst_fakeSameSignInterp = false; // -- Z-peak interpolation for the same-sign method

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

    /// \brief Draws the histograms
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

    /// \brief Turns a histogram into a density plot
    void make_density(std::unique_ptr<TH1> &hist);

    /// \brief Turns a density plot back into a normal histogram
    void make_normal_from_density(std::unique_ptr<TH1> &hist);

    /// \brief Finds the linearly interpolated value at a given x (used to interpolate the mass histogram)
    double linear_interpolation(const double x, const double x0, const double x1, const double y0, const double y1);

    /// \brief Needed for cubic interpolation
    double get_m(const double x, const double x0, const double x1, const double y, const double y0, const double y1);

    /// \brief Finds the linearly interpolated value at a given x using cubic Hermite spline interpolation (used to interpolate the mass histogram)
    double cubic_interpolation(const double x, const double x0, const double x1, const double y0, const double y1, const double m0, const double m1);

    /// \brief Linear interpolation of the mass histogram
    void interpolate_mass_histo(std::unique_ptr<TH1> &hist, const bool &alt=false);

//     /// \brief Finds the linearly interpolated histogram (linearly interpolates each bin of the histogram w.r.t. two other histograms)
//     /// Not used anywhere yet, to be used for other mass-binned histograms
//     std::unique_ptr<TH1> linear_interpolation(const double &x,
//                                               const double &x0,
//                                               const double &x1,
//                                               const std::unique_ptr<TH1> &y0,
//                                               const std::unique_ptr<TH1> &y1,
//                                               const std::string &name);

//     /// \brief Needed for cubic interpolation (for interpolating the whole histogram)
//     /// Not used anywhere yet, to be used for other mass-binned histograms
//     std::unique_ptr<TH1> get_m(const double &x,
//                                const double &x0,
//                                const double &x1,
//                                const std::unique_ptr<TH1> &y,
//                                const std::unique_ptr<TH1> &y0,
//                                const std::unique_ptr<TH1> &y1,
//                                const std::string &name);

//     /// \brief Finds the cubically interpolated histogram (linearly interpolates each bin of the histogram w.r.t. two other histograms)
//     /// Not used anywhere yet, to be used for other mass-binned histograms
//     std::unique_ptr<TH1> cubic_interpolation(const double &x,
//                                              const double &x0,
//                                              const double &x1,
//                                              const std::unique_ptr<TH1> &y0,
//                                              const std::unique_ptr<TH1> &y1,
//                                              const std::unique_ptr<TH1> &m0,
//                                              const std::unique_ptr<TH1> &m1,
//                                              const std::string &name);

};

} // namespace util

#endif // FAKE_BKG_EST_BUILDER_H
