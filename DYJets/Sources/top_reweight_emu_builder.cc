#include "top_reweight_emu_builder.h"

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
#include <TFitResult.h>

#include "mc_group.h"

namespace util
{

top_reweight_emu_builder::top_reweight_emu_builder(const std::string &analyzer_name) :
    _analyzer_name(analyzer_name),
    _default_config_file(analyzer_name + ".yml"),
    _preliminary(true)
{
}

void top_reweight_emu_builder::parse_options(int argc, char **argv)
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

void top_reweight_emu_builder::build()
{
    load();
    create_output_dir();
    filter_histogram_names();

    bool log = (_opt.map.count("lin") == 0);

    util::logging::debug << "Creating output file: " << _output_dir_name + "/dyjets-TopBkgs.root" << std::endl;
    std::unique_ptr<TFile> fout = std::unique_ptr<TFile>(new TFile((_output_dir_name + "/dyjets-TopBkgs.root").c_str(), "RECREATE"));
    // Creating a job info histogram to allow the data-driven backgrounds be used in reco comparison code
    std::unique_ptr<TH1D> job_info = std::unique_ptr<TH1D>(new TH1D("_job_info", "_job_info", 4 ,0, 4));
    job_info->GetXaxis()->SetBinLabel(1, "fraction_processed");
    job_info->GetXaxis()->SetBinLabel(2, "weights_sum");
    // job_info->SetBinContent(1, 1);
    // job_info->SetBinContent(2, _lumi); // This ensures that data-driven background does not get normalised the second time in reco comparison
    job_info->SetBinContent(1, 1);
    job_info->SetBinContent(2, 0);

    TVectorD job_info_average(2);
    // job_info_average[0] = -1;
    // job_info_average[1] = 1;
    job_info_average[0] = _lumi;
    job_info_average[1] = 0;
    fout->cd();
    job_info->Write("_job_info");
    job_info_average.Write("_job_info_average");

    for (const std::string &name : _histogram_names) {
        util::logging::debug << "Producing histogram: " << name << std::endl;

        // Apply style
        _current_histo_name = name;
        _logx = _style.get<bool>("log x", name, false);

        // EMU METHOD
        // Estimate all top backgrounds at once
        util::logging::debug << "Performing emu method on top backgrounds only" << std::endl;
        _bkg_estimation = emu_method();
        if (!_bkg_estimation)
        {
            util::logging::warn << "The current dilepton MC histogram was not found!" << std::endl;
            continue;
        }
        util::logging::debug << "Estimated top background events using emu method: " << _bkg_estimation->Integral() << std::endl;

        // Save the historgrams
        fout->cd();
        util::logging::debug << "Saving the estimated top background histogram" << std::endl;
        _bkg_estimation->Write(name.c_str());
        _bkg_estimation->SetDirectory(0);

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
}// void top_reweight_emu_builder::build()

std::unique_ptr<data::data_comparison_entry> top_reweight_emu_builder::load_data(
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

std::unique_ptr<data::mc_comparison_entry> top_reweight_emu_builder::load_mc(
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

void top_reweight_emu_builder::format_upper_x_axis(TAxis &axis) const
{
    if (_logx) {
        prepare_axis_for_log(axis);
    }
}

void top_reweight_emu_builder::format_upper_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetLabelSize(0.04);
    axis.SetLabelOffset(0.002);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.04);
    axis.SetTitleOffset(1.32);
}

void top_reweight_emu_builder::format_lower_x_axis(TAxis &axis) const
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

void top_reweight_emu_builder::format_lower_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetNdivisions(5, 5, 0);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.1);
    axis.SetTitleOffset(0.5);
    axis.CenterTitle();
    axis.SetLabelSize(0.08);
}

po::options_description top_reweight_emu_builder::options() const
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
    return options;
}

void top_reweight_emu_builder::load()
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

    _ll_data_entry = load_data(ll_input_dir);
    _ll_mc_entry = load_mc(ll_input_dir, 0, 1, "DY #rightarrow #tau#tau TauTau VV WW WZ ZZ #gamma#gamma W+Jets #gamma+Jets QCD Fakes");
    _emu_data_entry = load_data(emu_input_dir);
    _emu_mc_entry = load_mc(emu_input_dir, 0, 1, "DY #rightarrow #tau#tau TauTau VV WW WZ ZZ #gamma#gamma W+Jets #gamma+Jets QCD Fakes");
    _emu_mc_subtract_entry = load_mc(emu_input_dir, 0, 1, "t#bar{t} TT Single top W+Jets #gamma+Jets QCD Fakes");
    _emu_fakes_entry = load_mc(emu_input_dir, 0, 1, "DY #rightarrow #tau#tau TauTau VV WW WZ ZZ #gamma#gamma t#bar{t} TT Single top W+Jets #gamma+Jets QCD");

    _lumi = _ll_data_entry->lumi();
    util::logging::info << "Normalizing MC to " << (_lumi / 1000) << " fb^-1" << std::endl;
    if (_lumi != _emu_data_entry->lumi())
        util::logging::error << "Integrated luminosity does not match between different files!" << std::endl;
}

void top_reweight_emu_builder::fill_upper_panel(const std::string &name)
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

void top_reweight_emu_builder::fill_legend(TLegend &legend, const std::string &name)
{
    util::logging::debug << "Filling legend" << std::endl;
    legend.AddEntry(_bkg_estimation.get(), "e#mu method", "lp");
    _ll_mc_entry->add_to_legend(legend, name, _lumi);
}

bool top_reweight_emu_builder::fill_lower_panel(const std::string &name)
{
    util::logging::debug << "Filling lower pannel" << std::endl;
    std::unique_ptr<TH1> num = nullptr;
    std::unique_ptr<TH1> den = nullptr;
    den = _ll_mc_entry->get(name, _lumi);
    num = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(_bkg_estimation.get()->Clone()));
    

    if (num == nullptr || den == nullptr) {
        return false;
    }

    _ratio = std::move(num);
    num = nullptr;
    _ratio->Divide(den.get());

    if (name == "mass_wide_range_inc0jet")
    {
        std::unique_ptr<TF1> fit(new TF1("fit", "[0]+[1]*log10(x)"));
        fit->SetParameters(1.0, 1.0/1000.0);
        auto fit_result = _ratio->Fit(fit.get(), "S");
        fit_result->Print("V");
        auto cov_matrix = fit_result->GetCovarianceMatrix();

        // For systematic uncertainty
        std::unique_ptr<TH1> ratio_fakes_plus  = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(_bkg_estimation_fakes_plus->Clone()));
        std::unique_ptr<TH1> ratio_fakes_minus = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(_bkg_estimation_fakes_minus->Clone()));
        ratio_fakes_plus ->Divide(den.get());
        ratio_fakes_minus->Divide(den.get());
        std::unique_ptr<TF1> fit_fakes_plus(new TF1("fit_fakes_plus", "[0]+[1]*log10(x)"));
        std::unique_ptr<TF1> fit_fakes_minus(new TF1("fit_fakes_minus", "[0]+[1]*log10(x)"));
        fit_fakes_plus->SetParameters(1.0, 1.0/1000.0);
        fit_fakes_minus->SetParameters(1.0, 1.0/1000.0);
        auto fit_result_fakes_plus  = ratio_fakes_plus ->Fit(fit_fakes_plus .get(), "S");
        auto fit_result_fakes_minus = ratio_fakes_minus->Fit(fit_fakes_minus.get(), "S");

        // Producing a text file with content to copy into the yml file for reweighting
        std::ofstream fit_file(_output_dir_name + "/fitParams.txt");

        if (fit_file.is_open()) {
            fit_file << "============== USE THIS ==============\n" << std::endl;
            fit_file << "emu method reweighting:" << std::endl;
            fit_file << "  use: yes" << std::endl;
            fit_file << "  parameters: [" << fit->GetParameter(0) << ", "  << fit->GetParameter(1) << "]" << std::endl;
            fit_file << "  errors: [" << fit->GetParError(0) << ", "  << fit->GetParError(1) << "]" << std::endl;
            fit_file << "  fakes plus: [" << fit_fakes_plus->GetParameter(0) << ", "  << fit_fakes_plus->GetParameter(1) << "]" << std::endl;
            fit_file << "  fakes minus: [" << fit_fakes_minus->GetParameter(0) << ", "  << fit_fakes_minus->GetParameter(1) << "]" << std::endl;
            fit_file.close();
            std::cout << "Fit parameters saved to 'fitParams.txt' successfully." << std::endl;
        } else {
            std::cerr << "Unable to open the file for fit parameters." << std::endl;
        }
    }

    format_lower_x_axis(*_ratio->GetXaxis());
    format_lower_y_axis(*_ratio->GetYaxis(), "e#mu method/MC");
    double ratio_min = style().get<double>("ratio min", name, 0.601);
    double ratio_max = style().get<double>("ratio max", name, 1.399);
    _ratio->GetYaxis()->SetRangeUser(ratio_min, ratio_max);
    _ratio->SetMarkerStyle(20);
    _ratio->SetMarkerColor(kBlack);
    _ratio->SetLineColor(kBlack);
    _ratio->SetStats(0);
    _ratio->SetTitle("");
    _ratio->Draw("ep");

    return true;
}

void top_reweight_emu_builder::reset_drawing_state()
{
    util::logging::debug << "Resetting drawing state" << std::endl;
    _ll_mc_entry->reset_drawing_state();
    _ll_data_entry->reset_drawing_state();
    _emu_mc_entry->reset_drawing_state();
    _emu_mc_subtract_entry->reset_drawing_state();
    _emu_fakes_entry->reset_drawing_state();
    _emu_data_entry->reset_drawing_state();
    _ratio.reset();
}

void top_reweight_emu_builder::create_output_dir() const
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

void top_reweight_emu_builder::filter_histogram_names()
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
}// top_reweight_emu_builder::filter_histogram_names()

void top_reweight_emu_builder::remove_negative_bins(std::unique_ptr<TH1> &hist)
{
    util::logging::debug << "Removing negative bins from " << hist->GetTitle() << std::endl;
    for (int i=0; i<=hist->GetNbinsX()+1; ++i) {
        if (hist->GetBinContent(i) < 0)
            hist->SetBinContent(i, 0);
    }
}

std::unique_ptr<TH1> top_reweight_emu_builder::emu_method()
{
    std::unique_ptr<TH1> ll_mc_input = _ll_mc_entry->get(_current_histo_name, _lumi);
    if (!ll_mc_input) {
        util::logging::warn << "The current dilepton MC histogram was not found!" << std::endl;
        return nullptr;
    }
    util::logging::debug << "ll mc events: " << ll_mc_input->Integral() << std::endl;

    std::unique_ptr<TH1> num  = _emu_data_entry       ->get(_current_histo_name, _lumi);
    std::unique_ptr<TH1> den  = _emu_mc_entry         ->get(_current_histo_name, _lumi);
    std::unique_ptr<TH1> sub1 = _emu_mc_subtract_entry->get(_current_histo_name, _lumi);
    std::unique_ptr<TH1> sub2 = _emu_fakes_entry      ->get(_current_histo_name, _lumi);
    if (!num) {
        util::logging::warn << "The current EMu data histogram was not found!" << std::endl;
        return nullptr;
    }
    if (!sub1) {
        util::logging::warn << "The current non-top EMu MC histogram was not found!" << std::endl;
        return nullptr;
    }
    if (!sub2) {
        util::logging::warn << "The current fake EMu histogram was not found!" << std::endl;
        return nullptr;
    }
    if (!den) {
        util::logging::warn << "The current top EMu MC histogram was not found!" << std::endl;
        return nullptr;
    }

    util::logging::debug << "emu data events before MC subtraction: " << num->Integral() << std::endl;
    util::logging::debug << "non-top emu mc events to be subtracted from data: " << sub1->Integral() << std::endl;
    util::logging::debug << "fake emu events to be subtracted from data: " << sub2->Integral() << std::endl;

    num->Add(sub1.get(), -1);
    // For systematic uncertainty
    std::unique_ptr<TH1> num_fakes_plus  = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(num.get()->Clone(_current_histo_name.c_str())));
    std::unique_ptr<TH1> num_fakes_minus = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(num.get()->Clone(_current_histo_name.c_str())));

    num->Add(sub2.get(), -1);
    // For systematic uncertainty
    num_fakes_plus->Add(sub2.get(), -1.1);
    num_fakes_minus->Add(sub2.get(), -0.9);

    util::logging::debug << "emu data events after MC subtraction: " << num->Integral() << std::endl;
    util::logging::debug << "top emu mc events: " << den->Integral() << std::endl;

    std::unique_ptr<TH1> bkg_est = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(ll_mc_input->Clone(_current_histo_name.c_str())));
    // For systematic uncertainty
    _bkg_estimation_fakes_plus   = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(ll_mc_input->Clone(_current_histo_name.c_str())));
    _bkg_estimation_fakes_minus  = std::unique_ptr<TH1>(dynamic_cast<TH1 *>(ll_mc_input->Clone(_current_histo_name.c_str())));
    
    bkg_est->Multiply(num.get());
    bkg_est->Divide(den.get());

    // For systematic uncertainty
    _bkg_estimation_fakes_plus ->Multiply(num_fakes_plus.get());
    _bkg_estimation_fakes_plus ->Divide(den.get());
    _bkg_estimation_fakes_minus->Multiply(num_fakes_minus.get());
    _bkg_estimation_fakes_minus->Divide(den.get());

    return bkg_est;
}

} // namespace util
