#include "emu_method_builder.h"

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

#include "mc_group.h"

namespace util
{

emu_method_builder::emu_method_builder(const std::string &analyzer_name) :
    _analyzer_name(analyzer_name),
    _default_config_file(analyzer_name + ".yml"),
    _preliminary(true),
    _reversed(true)
{
}

void emu_method_builder::parse_options(int argc, char **argv)
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

void emu_method_builder::build()
{
    load();
    create_output_dir();
    filter_histogram_names();

    bool log = (_opt.map.count("lin") == 0);

    util::logging::debug << "Creating output file: " << _output_dir_name + "/dyjets-PromptBkg.root" << std::endl;
    std::unique_ptr<TFile> fout = std::unique_ptr<TFile>(new TFile((_output_dir_name + "/dyjets-PromptBkg.root").c_str(), "RECREATE"));
    // Creating a job info histogram to allow the data-driven backgrounds be used in reco comparison code
    std::unique_ptr<TH1D> job_info = std::unique_ptr<TH1D>(new TH1D("_job_info", "_job_info", 4 ,0, 4));
    job_info->SetBinContent(1, 1);
    job_info->SetBinContent(2, _lumi); // This ensures that data-driven background does not get normalised the second time in reco comparison
    TVectorD job_info_average(2);
    job_info_average[0] = -1;
    job_info_average[1] = 1;
    fout->cd();
    job_info->Write("_job_info");
    job_info_average.Write("_job_info_average");

    for (const std::string &name : _histogram_names) {
        util::logging::debug << "Producing histogram: " << name << std::endl;

        // Apply style
        _current_histo_name = name;
        _logx = _style.get<bool>("log x", name, false);

        // EMU METHOD
        // Estimate all backgrounds at once
        util::logging::debug << "Performing emu method on all backgrounds at once" << std::endl;
        std::unique_ptr<TH1> all_bkg_mc = _ll_mc_entry->get(name, _lumi);
        if (!all_bkg_mc)
        {
            util::logging::warn << "The current dilepton MC histogram was not found!" << std::endl;
            continue;
        }
        util::logging::debug << "ll mc events: " << all_bkg_mc->Integral() << std::endl;
        _bkg_estimation = emu_method(all_bkg_mc.get());
        if (!_bkg_estimation)
        {
            util::logging::warn << "The current dilepton MC histogram was not found!" << std::endl;
            continue;
        }
        util::logging::debug << "Estimated background events using emu method: " << _bkg_estimation->Integral() << std::endl;

        // Save the historgrams
        fout->cd();
        util::logging::debug << "Saving the estimated background histogram" << std::endl;
        _bkg_estimation->Write(name.c_str());
        _bkg_estimation->SetDirectory(0);

        // Estimate backgrounds one by one (may be desired for histograms)
        util::logging::debug << "Performing emu method on all backgrounds one by one" << std::endl;
        for (data::mc_group &group : _ll_mc_entry->groups()) {
            for (data::mc_group::sample_data &sd : group.samples_data()) {
                if (sd.sample.name().find("DYJets") != std::string::npos ||
                    sd.sample.name().find("WZ") != std::string::npos ||
                    sd.sample.name().find("ZZ") != std::string::npos ||
                    sd.sample.name().find("WJetsToLNu") != std::string::npos ||
                    sd.sample.name().find("GammaGamma") != std::string::npos ||
                    sd.sample.name().find("Fakes") != std::string::npos)
                    continue;
                util::logging::debug << "  Processing " << sd.sample.name() << std::endl;
                std::unique_ptr<TH1> single_bkg_mc = sd.centry->get(name, _lumi*group.scale_factor());
                std::unique_ptr<TH1> single_bkg_est = emu_method(single_bkg_mc.get());
                if (!single_bkg_est) continue;

                // Save the histogram
                util::logging::debug << "    Saving into a file " + _output_dir_name + "/dyjets-"+sd.sample.name()+".root" << std::endl;
                std::unique_ptr<TFile> fout_single = std::unique_ptr<TFile>(new TFile((_output_dir_name + "/dyjets-"+sd.sample.name()+".root").c_str(), "UPDATE"));
                fout_single->cd();
                single_bkg_est->Write(name.c_str());
                fout_single->Close();
            }
        }

        // Draw comparison between MC and emu method
        util::logging::debug << "Drawing the comparison between MC and emu method" << std::endl;
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
                    _preliminary ? "#bf{CMS} #it{Preliminary}" : "#bf{CMS}");
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
}// void emu_method_builder::build()

std::unique_ptr<data::data_comparison_entry> emu_method_builder::load_data(
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

std::unique_ptr<data::mc_comparison_entry> emu_method_builder::load_mc(
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

void emu_method_builder::format_upper_x_axis(TAxis &axis) const
{
    if (_logx) {
        prepare_axis_for_log(axis);
    }
}

void emu_method_builder::format_upper_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetLabelSize(0.04);
    axis.SetLabelOffset(0.002);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.04);
    axis.SetTitleOffset(1.32);
}

void emu_method_builder::format_lower_x_axis(TAxis &axis) const
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

void emu_method_builder::format_lower_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetNdivisions(5, 5, 0);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.1);
    axis.SetTitleOffset(0.5);
    axis.CenterTitle();
    axis.SetLabelSize(0.08);
}

po::options_description emu_method_builder::options() const
{
    po::options_description options = po::options_description("EMu comparison options");
    options.add_options()("output,o", po::value<std::string>(), "Sets the output directory");
    options.add_options()("format,f",
                          po::value<std::string>()->default_value("png"),
                          "Sets the output format (png, pdf, ...)");
    options.add_options()("histogram-name,n",
                          po::value<std::vector<std::string>>(),
                          "Produce the given histogram (can be used several times)");
    options.add_options()("lin", "Use a linear scale for the y axis (the default is a log scale)");
    options.add_options()("reversed,r", "Plot Data/MC instead of MC/Data");
    return options;
}

void emu_method_builder::load()
{
    util::logging::debug << "Loading all samples" << std::endl;
    std::string ll_input_dir = _opt.config["ll location"].as<std::string>();
    std::string emu_input_dir = _opt.config["emu location"].as<std::string>();
    std::string output_dir = parsed_options().map["output"].as<std::string>();
    set_default_output_dir("./emu_method_plots");
    if (ll_input_dir == "" || ll_input_dir == "-" || boost::to_upper_copy<std::string>(ll_input_dir) == "NONE")
        throw std::runtime_error("ll location directory not specified!");
    if (emu_input_dir == "" || emu_input_dir == "-" || boost::to_upper_copy<std::string>(emu_input_dir) == "NONE")
        throw std::runtime_error("emu location directory not specified!");

    _reversed = (parsed_options().map.count("reversed") > 0);

    _ll_data_entry = load_data(ll_input_dir);
    _ll_mc_entry = load_mc(ll_input_dir, 0, 1, "DYJets WZ ZZ W+Jets #gamma#gamma #gamma+Jets QCD Fakes");
    _emu_data_entry = load_data(emu_input_dir);
    _emu_mc_entry = load_mc(emu_input_dir, 0, 1, "W+Jets #gamma+Jets QCD");

    _lumi = _ll_data_entry->lumi();
    util::logging::info << "Normalizing MC to " << (_lumi / 1000) << " fb^-1" << std::endl;
    if (_lumi != _emu_data_entry->lumi())
        util::logging::error << "Integrated luminosity does not match between different files!" << std::endl;
}

void emu_method_builder::fill_upper_panel(const std::string &name)
{
    util::logging::debug << "Filling upper pannel" << std::endl;
    _ll_mc_entry->draw(name, _lumi);
    _bkg_estimation->SetLineColor(1);
    _bkg_estimation->SetMarkerColor(1);
    _bkg_estimation->SetMarkerStyle(21);
    _bkg_estimation->Draw("same");

    if (auto axis = _ll_mc_entry->get_x_axis(name, _lumi)) {
        format_upper_x_axis(*axis);
    }
    if (auto axis = _bkg_estimation->GetXaxis()) {
        format_upper_x_axis(*axis);
    }
}

void emu_method_builder::fill_legend(TLegend &legend, const std::string &name)
{
    util::logging::debug << "Filling legend" << std::endl;
    legend.AddEntry(_bkg_estimation.get(), "e#mu method", "lp");
    _ll_mc_entry->add_to_legend(legend, name, _lumi);
}

bool emu_method_builder::fill_lower_panel(const std::string &name)
{
    util::logging::debug << "Filling lower pannel" << std::endl;
    std::unique_ptr<TH1> num = nullptr;
    std::unique_ptr<TH1> den = nullptr;
    num = _ll_mc_entry->get(name, _lumi);
    den = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(_bkg_estimation.get()->Clone()));

    if (num == nullptr || den == nullptr) {
        return false;
    }

    if (_reversed) {
        std::swap(num, den);
    }

    _ratio = std::move(num);
    num = nullptr;

    _ratio->Divide(den.get());

    format_lower_x_axis(*_ratio->GetXaxis());
    format_lower_y_axis(*_ratio->GetYaxis(),
                        _reversed ? "e#mu method/MC" : "MC/e#mu method");

    double ratio_min = style().get<double>("ratio min", name, 0.601);
    double ratio_max = style().get<double>("ratio max", name, 1.399);
    _ratio->GetYaxis()->SetRangeUser(ratio_min, ratio_max);

    _ratio->SetMarkerStyle(20);
    _ratio->SetMarkerColor(kBlack);
    _ratio->SetLineColor(kBlack);
    _ratio->SetStats(0);
    _ratio->SetTitle("");
    _ratio->Draw("ep");

    if (name == "mass_wide_range_inc0jet")
    {
        std::unique_ptr<TF1> fit(new TF1("fit", "[0]+[1]*log10(x)"));
        fit->SetParameters(1.0, 1.0/1000.0);
        _ratio->Fit(fit.get());

        std::ofstream fitFile(_output_dir_name + "/fitParams.txt");

        if (fitFile.is_open()) {
            fitFile << "emu method reweighting:\n  use: yes\n  offset: " <<
                fit->GetParameter(0) << "\n  slope: " << fit->GetParError(0) << std::endl;
            // fitFile << "offset: " << fit->GetParameter(0) << " +- " << fit->GetParError(0) << std::endl;
            // fitFile << "slope: " << fit->GetParameter(1) << " +- " << fit->GetParError(1) << std::endl << std::endl;
            // fitFile << "covMatrix:" << std::endl;

            // for (int i = 0; i < cov_matrix.GetNrows(); ++i) {
            //     for (int j = 0; j < cov_matrix.GetNcols(); ++j) {
            //         fitFile << cov_matrix(i, j) << "\t";
            //     }
            //     fitFile << std::endl;
            // }

            std::cout << "Fit parameters saved to 'fitParams.txt' successfully." << std::endl;
        } else {
            std::cerr << "Unable to open the file for fit parameters." << std::endl;
        }
    }

    return true;
}

void emu_method_builder::reset_drawing_state()
{
    util::logging::debug << "Resetting drawing state" << std::endl;
    _ll_mc_entry->reset_drawing_state();
    _ll_data_entry->reset_drawing_state();
    _emu_mc_entry->reset_drawing_state();
    _emu_data_entry->reset_drawing_state();
    _ratio.reset();
}

void emu_method_builder::create_output_dir() const
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

void emu_method_builder::filter_histogram_names()
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
}// emu_method_builder::filter_histogram_names()

void emu_method_builder::remove_negative_bins(std::unique_ptr<TH1> &hist)
{
    util::logging::debug << "Removing negative bins from " << hist->GetTitle() << std::endl;
    for (int i=0; i<=hist->GetNbinsX()+1; ++i) {
        if (hist->GetBinContent(i) < 0)
            hist->SetBinContent(i, 0);
    }
}

std::unique_ptr<TH1> emu_method_builder::emu_method(const TH1* ll_mc_input)
{
    util::logging::debug << "Getting emu data and MC for emu method" << std::endl;
    if (!ll_mc_input) {
        util::logging::warn << "The current dilepton MC histogram was not found!" << std::endl;
        return nullptr;
    }

    std::unique_ptr<TH1> num = _emu_data_entry->get(_current_histo_name, _lumi);
    std::unique_ptr<TH1> den = _emu_mc_entry->get(_current_histo_name, _lumi);

    if (!num) {
        util::logging::warn << "The current EMu data histogram was not found!" << std::endl;
        return nullptr;
    }
    if (!den) {
        util::logging::warn << "The current EMu MC histogram was not found!" << std::endl;
        return nullptr;
    }

    util::logging::debug << "emu data events: " << num->Integral() << std::endl;
    util::logging::debug << "emu mc events: " << den->Integral() << std::endl;

    std::unique_ptr<TH1> bkg_est = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(ll_mc_input->Clone()));

    bkg_est->Multiply(num.get());
    bkg_est->Divide(den.get());
    return bkg_est;
}

} // namespace util
