#include "os_ss_ratio_compare_builder.h"

#include <algorithm>
#include <iomanip>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <TAxis.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TPad.h>
#include <TFile.h>
#include <TVectorD.h>
#include <TF1.h>
#include <TF2.h>
#include <TFitResult.h>

#include "mc_group.h"

namespace util
{

os_ss_ratio_compare_builder::os_ss_ratio_compare_builder(const std::string &analyzer_name) :
    _analyzer_name(analyzer_name),
    _default_config_file(analyzer_name + ".yml"),
    _preliminary(true)
{
}

void os_ss_ratio_compare_builder::parse_options(int argc, char **argv)
{
    auto options = this->options();

    _opt.default_init(argc, argv, _default_config_file, {options});

    if (_opt.map.count("output") > 0) {
        _output_dir_name = _opt.map["output"].as<std::string>();
    }
    _output_format = _opt.map["format"].as<std::string>();

    if (_opt.config["plots"]) {
        _style = util::style_list(_opt.config["plots"]);
    }

    if (_opt.config["preliminary"]) {
        _preliminary = _opt.config["preliminary"].as<bool>();
    }
}

void os_ss_ratio_compare_builder::build()
{
    load();
    create_output_dir();
    filter_histogram_names();

    bool log = (_opt.map.count("lin") == 0);

    for (const std::string &name : _histogram_names) {
        util::logging::debug << "Producing histogram: " << name << std::endl;

        // Apply style
        _current_histo_name = name;
        _logx = _style.get<bool>("log x", name, false);

        /// Fake rate by MC subtraction
        // Numerator and denominator for fake rate
        util::logging::debug << "Getting data for fake background estimation estimation by MC subtracion" << std::endl;
        if (_os_data_entry->get(name, _lumi) == nullptr ||
            _os_mc_entry->get(name, _lumi) == nullptr) {
            util::logging::warn << "Could not find all opposite-sign histograms with name " << name << std::endl;
            continue;
        }
        if (_ss_data_entry->get(name, _lumi) == nullptr ||
            _ss_mc_entry->get(name, _lumi) == nullptr) {
            util::logging::warn << "Could not find all same-sign histograms with name " << name << std::endl;
            continue;
        }
        // _fakes_est_os = std::unique_ptr<TH1>((TH1*)_os_data_entry->get(name, _lumi)->Clone(name.c_str()));
        // _fakes_est_ss = std::unique_ptr<TH1>((TH1*)_ss_data_entry->get(name, _lumi)->Clone(name.c_str()));
        _fakes_est_os = _os_data_entry->get(name, _lumi);
        _fakes_est_ss = _ss_data_entry->get(name, _lumi);

        util::logging::debug << "Estimating data-driven backgrounds" << std::endl;
        _fakes_est_os->Add(_os_mc_entry->get(name, _lumi).get(), -1);
        _fakes_est_ss->Add(_ss_mc_entry->get(name, _lumi).get(), -1);

        // Draw comparison between OS and SS
        util::logging::debug << "Drawing the comparison between same-sign and opposite-sign fakes" << std::endl;
        TCanvas canvas(name.c_str(), "", 700, 900);

        // Upper panel
        TPad upper("upper", "upper", 0, 0.3, 1, 1);
        upper.SetTopMargin(0.11);
        upper.SetRightMargin(0.03);
        upper.SetTicks();
        if (log) {
            upper.SetLogy();
        }
        upper.Draw();
        upper.cd();

        fill_upper_panel(name);

        // CMS label
        TLatex cms;
        cms.SetTextSize(0.04);
        cms.SetTextFont(42);
        cms.SetTextAlign(kHAlignLeft + kVAlignBottom);
        cms.SetNDC();
        cms.SetText(0.1,
                    0.9,
                    _preliminary ? "#bf{CMS} #it{in progress}" : "#bf{CMS}");
        cms.Draw();

        // Lumi label
        double lumi = get_lumi();
        TLatex label;
        if (lumi > 0) {
            label.SetTextSize(0.04);
            label.SetTextFont(42);
            label.SetTextAlign(kHAlignRight + kVAlignBottom);
            label.SetNDC();

            std::stringstream ss;
            ss << std::setprecision(3) << (lumi / 1000);
            ss << " fb^{-1} (13 TeV)";
            label.SetText(0.97, 0.9, ss.str().c_str());
            label.Draw();
        }

        // Legend
        TLegend legend(0.63, 0.60, 0.81, 0.87);
        legend.SetTextSize(0.042);
        legend.SetFillStyle(0);
        legend.SetBorderSize(0);
        legend.SetTextFont(42);
        legend.Draw();

        fill_legend(legend, name);

        // Display total ratio of integrals
        double os_int, ss_int, os_err, ss_err;
        os_int = _fakes_est_os->IntegralAndError(1, _fakes_est_os->GetNbinsX(), os_err);
        ss_int = _fakes_est_ss->IntegralAndError(1, _fakes_est_ss->GetNbinsX(), ss_err);
        double tot_ratio = os_int / ss_int;
        double tot_ratio_error = tot_ratio * std::sqrt(std::pow(os_err/os_int, 2) + std::pow(ss_err/ss_int, 2));
        std::stringstream ssratio;
        ssratio << std::setprecision(3) << "Avg. ratio: " << tot_ratio << "#pm" << tot_ratio_error;
        TLatex lratio;
        lratio.SetTextSize(0.04);
        lratio.SetTextFont(42);
        lratio.SetTextAlign(kHAlignLeft + kVAlignTop);
        lratio.SetNDC();
        lratio.SetText(0.12, 0.85, ssratio.str().c_str());
        lratio.Draw();

        // Get back to the canvas
        canvas.cd();

        // Lower panel
        TPad lower("lower", "lower", 0, 0.05, 1, 0.3);
        lower.SetTopMargin(0.);
        lower.SetBottomMargin(0.3);
        lower.SetRightMargin(0.03);
        lower.SetGridy();
        lower.SetTicks();
        lower.Draw();
        lower.cd();

        override_lower_panel_settings(lower);

        if (fill_lower_panel(name)) {
            upper.SetBottomMargin(0.);
        }

        // Apply style
        if (_logx) {
            upper.SetLogx();
            lower.SetLogx();
        }
        // Write file
        canvas.Print(
            (_output_dir_name + "/" + name + "." + _output_format).c_str());

        // Cleanup
        reset_drawing_state();

    }// for (const std::string &name : _histogram_names)
}// void os_ss_ratio_compare_builder::build()

std::unique_ptr<data::data_comparison_entry> os_ss_ratio_compare_builder::load_data(
        const std::string &input_dir)
{
    util::logging::debug << " Loading data from" + input_dir << std::endl;
    data::sample data;
    std::vector<data::sample> samples = data::sample::load(_opt);
    for (data::sample &s : samples) {
        if (s.name() == "data") {
            data = s;
        }
    }

    auto ptr = std::make_unique<data::data_comparison_entry>(_analyzer_name, data, input_dir);
    ptr->add_histograms(_histogram_names);
    return ptr;
}

std::unique_ptr<data::mc_comparison_entry> os_ss_ratio_compare_builder::load_mc(
        const std::string &input_dir,
        bool keep_signal,
        bool keep_background,
        const std::string &excluded_groups)
{
    util::logging::debug << " Loading MC from" + input_dir << std::endl;
    auto ptr = std::make_unique<data::mc_comparison_entry>(_opt,
                                                           _analyzer_name,
                                                           input_dir,
                                                           keep_signal,
                                                           keep_background,
                                                           excluded_groups);
    ptr->add_histograms(_histogram_names);
    return ptr;
}

namespace /* anonymous */ {
    /**
     * \brief Tunes an axis to be used on a log scale.
     *
     * If the first bin extends to 0, it is modified to start from a higher
     * value. This is needed because otherwise ROOT will make it span half of
     * the plots.
     *
     * \note Histograms with a uniform binning are not supported.
     */
    void prepare_axis_for_log(TAxis &axis)
    {
        if (axis.GetXmin() > 0 || axis.GetNbins() < 2) {
            // Nothing to do
            return;
        }

        // Get the bin edges in a safe container (std::vector over TArray).
        const double *edges_ptr = axis.GetXbins()->GetArray();
        if (edges_ptr == nullptr) {
            // Happens when the histogram has a uniform binning.
            return;
        }
        std::vector<double> edges(edges_ptr, edges_ptr + axis.GetNbins() + 1);

        // First, we get the extent of the second bin. It's proportional to the
        // ratio of the edges.
        double ratio = edges[2] / edges[1];

        // Then, we modify the first bin boundaries so that it gets the same
        // displayed size.
        edges[0] = edges[1] / ratio;

        // We want the lower bound to be a round number, so we round it. It's
        // more complicated that a simple floor() because we want 0.25 to become
        // 0.2 and not 0.0.
        double logfactor = std::pow(10, std::ceil(std::log10(edges[0])) - 1);
        edges[0] = logfactor * std::floor(edges[0] / logfactor);
        edges[0] *= 1.001; // Avoid tick labels.

        // Modify the axis to use the new bin edges.
        axis.Set(edges.size() - 1, edges.data());
    }
} // namespace anonymous

void os_ss_ratio_compare_builder::format_upper_x_axis(TAxis &axis) const
{
    if (_logx) {
        prepare_axis_for_log(axis);
    }
}

void os_ss_ratio_compare_builder::format_upper_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetLabelSize(0.04);
    axis.SetLabelOffset(0.002);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.04);
    axis.SetTitleOffset(1.32);
}

void os_ss_ratio_compare_builder::format_lower_x_axis(TAxis &axis) const
{
    axis.SetTickLength(0.03);
    axis.SetTitleSize(0.1);
    axis.SetTitleOffset(1.2);
    axis.SetLabelSize(0.10);
    axis.SetLabelOffset(0.017);

    if (_logx) {
        prepare_axis_for_log(axis);
    }

    std::string label = _style.get_formatted("x axis label", _current_histo_name, "");
    if (!label.empty()) {
        auto precisions = _style.get_formatted_all("x axis detail", _current_histo_name);
        if (!precisions.empty()) {
            label += " (" + boost::algorithm::join(precisions, ", ") + ")";
        }
        auto unit = _style.get_formatted("x axis unit", _current_histo_name, "");
        if (!unit.empty()) {
            label += " [" + unit + "]";
        }
        axis.SetTitle(label.c_str());
    } else if (std::strlen(axis.GetTitle()) == 0) {
        axis.SetTitle(_current_histo_name.c_str());
    }
}

void os_ss_ratio_compare_builder::format_lower_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetNdivisions(5, 5, 0);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.1);
    axis.SetTitleOffset(0.5);
    axis.CenterTitle();
    axis.SetLabelSize(0.08);
}

po::options_description os_ss_ratio_compare_builder::options() const
{
    po::options_description options = po::options_description("OS/SS comparison options"); 
    options.add_options()("output,o", po::value<std::string>(), "Sets the output directory");
    options.add_options()("format,f",
                          po::value<std::string>()->default_value("png"),
                          "Sets the output format (png, pdf, ...)");
    options.add_options()("histogram-name,n",
                          po::value<std::vector<std::string>>(),
                          "Produce the given histogram (can be used several times)");
    options.add_options()("lin", "Use a linear scale for the y axis (the default is a log scale)");
    return options;
}

void os_ss_ratio_compare_builder::load()
{
    util::logging::debug << "Loading all samples" << std::endl;
    std::string os_input_dir = _opt.config["os location"].as<std::string>();
    std::string ss_input_dir = _opt.config["ss location"].as<std::string>();
    std::string output_dir = parsed_options().map["output"].as<std::string>();
    set_default_output_dir("./OS_vs_SS");
    if (os_input_dir == "" || os_input_dir == "-" || boost::to_upper_copy<std::string>(os_input_dir) == "NONE")
        throw std::runtime_error("opposite-sign location directory not specified!");
    if (ss_input_dir == "" || ss_input_dir == "-" || boost::to_upper_copy<std::string>(ss_input_dir) == "NONE")
        throw std::runtime_error("same-sign location directory not specified!");

    _os_data_entry = load_data(os_input_dir);
    _os_mc_entry = load_mc(os_input_dir, 1, 1, "W+Jets #gamma+Jets QCD Fakes");
    _ss_data_entry = load_data(ss_input_dir);
    _ss_mc_entry = load_mc(ss_input_dir, 1, 1, "W+Jets #gamma+Jets QCD Fakes");
 
    _lumi = _os_data_entry->lumi();
    util::logging::info << "Normalizing MC to " << (_lumi / 1000) << " fb^-1" << std::endl;
    if (_lumi != _ss_data_entry->lumi())
        util::logging::error << "Integrated luminosity does not match between different files!" << std::endl;
}

void os_ss_ratio_compare_builder::fill_upper_panel(const std::string &name)
{
    util::logging::debug << "Filling upper pannel" << std::endl;
    _fakes_est_os->SetLineColor(1);
    _fakes_est_os->SetMarkerColor(1);
    _fakes_est_os->SetMarkerStyle(21);
    _fakes_est_os->Draw("PE1");
    _fakes_est_ss->SetLineColor(2);
    _fakes_est_ss->SetMarkerColor(2);
    _fakes_est_ss->SetMarkerStyle(21);
    _fakes_est_ss->Draw("same PE1");

    if (auto axis = _fakes_est_os->GetXaxis()) {
        format_upper_x_axis(*axis);
    }
    if (auto axis = _fakes_est_ss->GetXaxis()) {
        format_upper_x_axis(*axis);
    }
}

void os_ss_ratio_compare_builder::fill_legend(TLegend &legend, const std::string &name)
{
    util::logging::debug << "Filling legend" << std::endl;
    legend.AddEntry(_fakes_est_os.get(), "Opposite-sign fakes", "lp");
    legend.AddEntry(_fakes_est_ss.get(), "Same-sign fakes", "lp");
}

bool os_ss_ratio_compare_builder::fill_lower_panel(const std::string &name)
{
    util::logging::debug << "Filling lower pannel" << std::endl;
    std::unique_ptr<TH1> num = nullptr;
    std::unique_ptr<TH1> den = nullptr;
    num = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(_fakes_est_os->Clone("num")));
    den = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(_fakes_est_ss->Clone("den")));

    if (num == nullptr || den == nullptr) {
        util::logging::error << "OS or SS histogram was not created! Cannot calculate ratio." << std::endl;
        return false;
    }

    _ratio = std::move(num);
    num = nullptr;

    _ratio->Divide(den.get());

    format_lower_x_axis(*_ratio->GetXaxis());
    format_lower_y_axis(*_ratio->GetYaxis(), "e#mu method/MC");

    double ratio_min = style().get<double>("ratio min", name, 0.501);
    double ratio_max = style().get<double>("ratio max", name, 2.499);
    _ratio->GetYaxis()->SetRangeUser(ratio_min, ratio_max);

    _ratio->SetMarkerStyle(20);
    _ratio->SetMarkerColor(kBlack);
    _ratio->SetLineColor(kBlack);
    _ratio->SetStats(0);
    _ratio->SetTitle("");
    _ratio->Draw("ep");

    if (name == "MET_pt_inc0jet_mass40_3000") {
        std::unique_ptr<TF1> fit(new TF1("fit", "[0]-[1]*x-exp([2]-[3]*x)"));
        fit->SetParameters(2.13477, 4.73643e-03, -1.71259e-01, 6.51280e-02);
        auto fit_result = _ratio->Fit(fit.get(), "S");
        auto correlation_matrix = fit_result->GetCorrelationMatrix();
        fit_result->Print("V");

        // Producing a text file with content to copy into the yml file for reweighting
        std::ofstream fit_file(_output_dir_name + "/fitParams_ee.txt");

        if (fit_file.is_open()) {
            fit_file << "============== USE THIS ==============\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;            
            fit_file << "  MET offset: " << fit->GetParameter(0) << std::endl;
            fit_file << "  MET slope: " << fit->GetParameter(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(3) << std::endl;
            fit_file << "\n============= UP VARIATION 0 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) + correlation_matrix(0,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) + correlation_matrix(0,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) + correlation_matrix(0,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) + correlation_matrix(0,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= UP VARIATION 1 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) + correlation_matrix(1,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) + correlation_matrix(1,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) + correlation_matrix(1,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) + correlation_matrix(1,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= UP VARIATION 2 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) + correlation_matrix(2,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) + correlation_matrix(2,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) + correlation_matrix(2,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) + correlation_matrix(2,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= UP VARIATION 3 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) + correlation_matrix(3,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) + correlation_matrix(3,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) + correlation_matrix(3,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) + correlation_matrix(3,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= DOWN VARIATION 0 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) - correlation_matrix(0,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) - correlation_matrix(0,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) - correlation_matrix(0,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) - correlation_matrix(0,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= DOWN VARIATION 1 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) - correlation_matrix(1,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) - correlation_matrix(1,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) - correlation_matrix(1,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) - correlation_matrix(1,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= DOWN VARIATION 2 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) - correlation_matrix(2,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) - correlation_matrix(2,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) - correlation_matrix(2,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) - correlation_matrix(2,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n============= DOWN VARIATION 3 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  MET offset: "     << fit->GetParameter(0) - correlation_matrix(3,0) * fit->GetParError(0) << std::endl;
            fit_file << "  MET slope: "      << fit->GetParameter(1) - correlation_matrix(3,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET exp offset: " << fit->GetParameter(2) - correlation_matrix(3,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET exp slope: "  << fit->GetParameter(3) - correlation_matrix(3,3) * fit->GetParError(3) << std::endl;
            fit_file << "\n=========================================\n" << std::endl;
            fit_file.close();
            util::logging::info << "Fit parameters saved to 'fitParams_ee.txt'." << std::endl;
        } else {
            std::cerr << "Unable to open the file for fit parameters." << std::endl;
        }
    }

    if (name == "MET_vs_mass_inc0jet_mass40_3000") {
        auto func = [](double *x, double *par) {
            double p1 = par[0] + par[1] * std::log10(x[1]);
            double p2 = par[2] - par[3] * x[0] * std::exp(-par[4]*x[0]);
            return  p1 * p2;
        };
        std::unique_ptr<TF2> fit(new TF2("fit", func, 0, 200, 40, 400, 5));
        fit->SetParameters(1.27, -0.277, 1.767, -0.07, 0.037);
        auto fit_result = _ratio->Fit(fit.get(), "S");
        auto correlation_matrix = fit_result->GetCorrelationMatrix();
        fit_result->Print("V");

        // Producing a text file with content to copy into the yml file for reweighting
        std::ofstream fit_file(_output_dir_name + "/fitParams_mm.txt");

        if (fit_file.is_open()) {
            fit_file << "============== USE THIS ==============\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) << std::endl;
            fit_file << "\n============= UP VARIATION 0 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) + correlation_matrix(0,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) + correlation_matrix(0,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) + correlation_matrix(0,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) + correlation_matrix(0,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) + correlation_matrix(0,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= UP VARIATION 1 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) + correlation_matrix(1,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) + correlation_matrix(1,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) + correlation_matrix(1,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) + correlation_matrix(1,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) + correlation_matrix(1,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= UP VARIATION 2 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) + correlation_matrix(2,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) + correlation_matrix(2,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) + correlation_matrix(2,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) + correlation_matrix(2,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) + correlation_matrix(2,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= UP VARIATION 3 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) + correlation_matrix(3,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) + correlation_matrix(3,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) + correlation_matrix(3,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) + correlation_matrix(3,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) + correlation_matrix(3,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= UP VARIATION 4 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) + correlation_matrix(4,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) + correlation_matrix(4,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) + correlation_matrix(4,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) + correlation_matrix(4,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) + correlation_matrix(4,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= DOWN VARIATION 0 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) - correlation_matrix(0,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) - correlation_matrix(0,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) - correlation_matrix(0,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) - correlation_matrix(0,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) - correlation_matrix(0,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= DOWN VARIATION 1 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) - correlation_matrix(1,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) - correlation_matrix(1,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) - correlation_matrix(1,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) - correlation_matrix(1,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) - correlation_matrix(1,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= DOWN VARIATION 2 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) - correlation_matrix(2,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) - correlation_matrix(2,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) - correlation_matrix(2,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) - correlation_matrix(2,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) - correlation_matrix(2,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= DOWN VARIATION 3 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) - correlation_matrix(3,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) - correlation_matrix(3,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) - correlation_matrix(3,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) - correlation_matrix(3,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) - correlation_matrix(3,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n============= DOWN VARIATION 4 ==========\n" << std::endl;
            fit_file << "same sign method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  mass offset: "   << fit->GetParameter(0) - correlation_matrix(4,0) * fit->GetParError(0) << std::endl;
            fit_file << "  mass slope: "    << fit->GetParameter(1) - correlation_matrix(4,1) * fit->GetParError(1) << std::endl;
            fit_file << "  MET offset: "    << fit->GetParameter(2) - correlation_matrix(4,2) * fit->GetParError(2) << std::endl;
            fit_file << "  MET slope: "     << fit->GetParameter(3) - correlation_matrix(4,3) * fit->GetParError(3) << std::endl;
            fit_file << "  MET exp slope: " << fit->GetParameter(4) - correlation_matrix(4,4) * fit->GetParError(4) << std::endl;
            fit_file << "\n=========================================\n" << std::endl;
            fit_file.close();
            util::logging::info << "Fit parameters saved to 'fitParams_mm.txt'." << std::endl;
        } else {
            std::cerr << "Unable to open the file for fit parameters." << std::endl;
        }
    }

    return true;
}

void os_ss_ratio_compare_builder::reset_drawing_state()
{
    util::logging::debug << "Resetting drawing state" << std::endl;
    _os_mc_entry->reset_drawing_state();
    _os_data_entry->reset_drawing_state();
    _ss_mc_entry->reset_drawing_state();
    _ss_data_entry->reset_drawing_state();
    _fakes_est_os.reset();
    _fakes_est_ss.reset();
    _ratio.reset();
}

void os_ss_ratio_compare_builder::create_output_dir() const
{
    using namespace boost::filesystem;

    // Create output directory if it doesn't exist
    if (!is_directory(_output_dir_name)) {
        if (exists(_output_dir_name)) {
            // "_output_dir_name" exists and is not a directory...
            throw std::runtime_error("Path " + _output_dir_name +
                                     " exists and is not a directory");
        } else {
            util::logging::info << "Creating directory " << _output_dir_name << std::endl;
            create_directories(_output_dir_name);
        }
    }
}

void os_ss_ratio_compare_builder::filter_histogram_names()
{
    util::logging::debug << "Filtering histogram names" << std::endl;
    if (_opt.map.count("histogram-name") > 0) {
        // Read from command line
        std::vector<std::string> names =
            _opt.map["histogram-name"].as<std::vector<std::string>>();

        // Check that names exist
        for (auto name : names) {
            if (std::find(_histogram_names.begin(),
                          _histogram_names.end(),
                          name) == _histogram_names.end()) {
                throw std::runtime_error("Histogram " + name + " doesn't exist");
            }
        }

        _histogram_names.clear();
        std::copy(names.begin(),
                  names.end(),
                  std::inserter(_histogram_names, _histogram_names.begin()));
    } else {
        // Produce everything
        auto before_filter = _histogram_names.size();

        util::logging::info << "Found " << _histogram_names.size() << " histograms."
                            << std::endl;

        // Remove histograms vetoed by style
        for (auto it = _histogram_names.begin(); it != _histogram_names.end(); ) {
            if (_style.get<bool>("produce", *it, true)) {
                ++it;
            } else {
                util::logging::debug << "Not producing histogram " << *it
                                     << " due to plot rules." << std::endl;
                it = _histogram_names.erase(it);
            }
        }

        auto removed = before_filter - _histogram_names.size();
        util::logging::info << removed << " histograms skipped due to style rules."
                            << std::endl;
    }
}// os_ss_ratio_compare_builder::filter_histogram_names()

void os_ss_ratio_compare_builder::remove_negative_bins(std::unique_ptr<TH1> &hist)
{
    util::logging::debug << "Removing negative bins from " << hist->GetTitle() << std::endl;
    for (int i=0; i<=hist->GetNbinsX()+1; ++i) {
        if (hist->GetBinContent(i) < 0)
            hist->SetBinContent(i, 0);
    }
}

} // namespace util
