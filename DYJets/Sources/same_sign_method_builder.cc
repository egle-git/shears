#include "same_sign_method_builder.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <TAxis.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TFile.h>
#include <TF1.h>
#include <TVectorD.h>

namespace util
{

same_sign_method_builder::same_sign_method_builder(const std::string &analyzer_name) :
    _analyzer_name(analyzer_name),
    _default_config_file(analyzer_name + ".yml"),
    _preliminary(true),
    _smooth(false),
    _smooth_amount(1),
    _rem_neg_bins(false)
{
}

void same_sign_method_builder::parse_options(int argc, char **argv)
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

    if (_opt.config["remove negative bins"]) {
        _rem_neg_bins = _opt.config["remove negative bins"].as<bool>();
    }

    if (_opt.config["conversion ratio"]) {
        _os_ss_ratio = _opt.config["conversion ratio"].as<double>();
    }

    if (_opt.config["smoothing"]) {
        const YAML::Node node = _opt.config["smoothing"];

        _smooth = node["use"].as<bool>();
        _smooth_amount = node["strength"].as<int>();

        std::string histnames = node["histograms"].as<std::string>();
        std::istringstream iss(histnames);
        std::string histo;

        while (iss >> histo) {
            _smooth_histograms.push_back(histo);
        }
    }
}

void same_sign_method_builder::build()
{
    load();
    create_output_dir();
    filter_histogram_names();

    std::shared_ptr<TFile> outfile = std::unique_ptr<TFile>(new TFile((_output_dir_name + "/" + _analyzer_name + "-Fakes.root").c_str(), "RECREATE"));

    // Creating a job info histogram to allow the data-driven backgrounds be used in reco comparison code
    std::unique_ptr<TH1D> job_info = std::unique_ptr<TH1D>(new TH1D("_job_info", "_job_info", 4 ,0, 4));
    job_info->SetBinContent(1, 1);
    job_info->SetBinContent(2, _lumi); // This ensures that data-driven background does not get normalised the second time in reco comparison
    TVectorD job_info_average(2);
    job_info_average[0] = -1;
    job_info_average[1] = 1;
    outfile->cd();
    job_info->Write("_job_info");
    job_info_average.Write("_job_info_average");

    for (const std::string &name : _histogram_names) {
        util::logging::debug << "Working with histograms: " << name << std::endl;

        _current_histo_name = name;
        // Apply style
        _logx = _style.get<bool>("log x", name, false);

        /// Fake rate by MC subtraction
        // Numerator and denominator for fake rate
        util::logging::debug << "Getting data for fake background estimation estimation by MC subtracion" << std::endl;
        if (_data_entry->get(name, _lumi) == nullptr ||
            _mc_entry->get(name, _lumi) == nullptr) {
            util::logging::warn << "Could not find all histograms with name " << name << std::endl;
            continue;
        }
        std::unique_ptr<TH1> fakes_est((TH1*)_data_entry->get(name, _lumi)->Clone(name.c_str())); // combined background

        util::logging::debug << "Estimating data-driven backgrounds" << std::endl;
        fakes_est->Add(_mc_entry->get(name, _lumi).get(), -1);

        // Smoothing histograms if required
        if (_smooth) {
            for (const std::string &histo : _smooth_histograms) {
                if (name.find(histo) != std::string::npos) {
                    fakes_est->Smooth(_smooth_amount);
                    break;
                }
            }
        }

        // Renivung negative bins if required
        if (_rem_neg_bins) remove_negative_bins(fakes_est, true);

        // Scaling the histograms by a constant OS/SS ratio if required
        fakes_est->Scale(_os_ss_ratio);

        // std::shared_ptr<TH1> fakes_est_density((TH1*)fakes_est->Clone((name+"density").c_str()));
        // for (unsigned ibin=1; ibin<=fakes_est_density->GetNbinsX(); ibin++) {
        //     fakes_est_density->SetBinContent(ibin, fakes_est_density->GetBinContent(ibin)/fakes_est_density->GetBinWidth(ibin));
        // }

        util::logging::debug << "Saving background histograms" << std::endl;
        outfile->cd();
        fakes_est->Write(name.c_str());

        // Draw the fake rates
        util::logging::debug << "Drawing the backgrounds" << std::endl;

        TCanvas canvas(("fakes_est_"+name).c_str(), "", 700, 900);
        canvas.SetTopMargin(0.07);
        canvas.SetLeftMargin(0.15);
        canvas.SetRightMargin(0.07);
        canvas.SetTicks();
        fakes_est->Draw("BAR");
        fakes_est->Draw("SAME E");
        fakes_est->SetTitle("");
        fakes_est->SetStats(0);
        if (!_logy) fakes_est->SetMinimum(0);
        else fakes_est->SetMinimum(0.01);
        fakes_est->SetLineColor(1);
        fakes_est->SetFillColor(805);
        if (auto axis = fakes_est->GetXaxis()) {
            format_x_axis(*axis);
        }
        if (auto axis = fakes_est->GetYaxis()) {
            format_y_axis(*axis, "Number of events");
        }
        if (_logx) {
            canvas.SetLogx();
        }
        TLegend leg_fakes(0.63, 0.83, 0.85, 0.91);
        leg_fakes.SetTextSize(0.042);
        leg_fakes.SetFillStyle(0);
        leg_fakes.SetBorderSize(0);
        leg_fakes.SetTextFont(42);
        leg_fakes.AddEntry(fakes_est.get(), "Fakes (est.)", "f");
        leg_fakes.Draw();
        // CMS label
        TLatex cms;
        cms.SetTextSize(0.04);
        cms.SetTextFont(42);
        cms.SetTextAlign(kHAlignLeft + kVAlignBottom);
        cms.SetNDC();
        cms.SetText(0.1, 0.95, _preliminary ? "#bf{CMS} #it{In progress}" : "#bf{CMS}");
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
            label.SetText(0.97, 0.95, ss.str().c_str());
            label.Draw();
        }
        if (_logy) canvas.SetLogy();
        canvas.Update();
        canvas.Print((_output_dir_name + "/" + "fakes_est_"+ name + "." + _output_format).c_str());

        // Combined background
        // TCanvas canvas_d(("fakes_est_"+name+"_density").c_str(), "", 700, 900);
        // canvas_d.SetTopMargin(0.07);
        // canvas_d.SetLeftMargin(0.15);
        // canvas_d.SetRightMargin(0.05);
        // canvas_d.SetTicks();
        // fakes_est_density->Draw("BAR");
        // fakes_est_density->Draw("SAME E");
        // fakes_est_density->SetTitle("");
        // fakes_est_density->SetStats(0);
        // fakes_est_density->SetMinimum(0);
        // fakes_est_density->SetLineColor(1);
        // fakes_est_density->SetFillColor(805);
        // if (auto axis = fakes_est_density->GetXaxis()) {
        //     format_x_axis(*axis);
        // }
        // if (auto axis = fakes_est_density->GetYaxis()) {
        //     format_y_axis(*axis, "Number of events / GeV");
        // }
        // if (_logx) {
        //     canvas_d.SetLogx();
        // }
        // leg_fakes.Draw();
        // cms.Draw();
        // label.Draw();
        // canvas_d.Update();
        // canvas_d.Print((_output_dir_name + "/" + "fakes_est_" + name + "_density." + _output_format).c_str());

        // Cleanup
        reset_drawing_state();

    }// for (const std::string &name : _histogram_names)
    
    // Cleanup
    reset_drawing_state();
}// void same_sign_method_builder::build()

std::unique_ptr<data::data_comparison_entry> same_sign_method_builder::load_data(
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

std::unique_ptr<data::mc_comparison_entry> same_sign_method_builder::load_mc(
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

void same_sign_method_builder::format_x_axis(TAxis &axis) const
{
    prepare_axis_for_log(axis);
    axis.SetTickLength(0.03);
    axis.SetTitleSize(0.04);
    axis.SetTitleOffset(1.0);
    axis.SetLabelSize(0.04);
    axis.SetLabelOffset(0.004);

    if (_logx) {
        prepare_axis_for_log(axis);
        axis.SetMoreLogLabels();
        axis.SetNoExponent();
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

void same_sign_method_builder::format_y_axis(TAxis &axis, const std::string &title) const
{
    axis.SetLabelSize(0.04);
    axis.SetLabelOffset(0.003);
    axis.SetTitle(title.c_str());
    axis.SetTitleSize(0.04);
    axis.SetTitleOffset(1.7);
}

po::options_description same_sign_method_builder::options() const
{
    po::options_description options = po::options_description("Fake rate estimation options");
    options.add_options()("input,i",
                          po::value<std::string>()->default_value("dyjets-histograms-same-sign"),
                          "Sets the directory to search same-sign histogram files");
    options.add_options()("output,o",
                          po::value<std::string>()->default_value("bkg-est-plots/"),
                          "Sets the output directory");
    options.add_options()("format,f",
                          po::value<std::string>()->default_value("png"),
                          "Sets the output format (png, pdf, ...)");
    options.add_options()("logy,l", "Use logarithmic scale on the y axis");                    
    return options;
}

void same_sign_method_builder::load()
{
    util::logging::debug << "Loading all samples" << std::endl;
    std::string input_dir = parsed_options().map["input"].as<std::string>();
    std::string output_dir = parsed_options().map["output"].as<std::string>();
    set_default_output_dir(output_dir + "/plots");

    _data_entry = load_data(input_dir);
    _mc_entry = load_mc(input_dir, 1, 1);

    _lumi = _data_entry->lumi();
    util::logging::info << "Normalizing MC to " << (_lumi / 1000) << " fb^-1" << std::endl;

    _logy = (parsed_options().map.count("logy") > 0);
}

void same_sign_method_builder::reset_drawing_state()
{
    util::logging::debug << "Resetting drawing state" << std::endl;
    _mc_entry->reset_drawing_state();
    _data_entry->reset_drawing_state();
}

void same_sign_method_builder::create_output_dir() const
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

void same_sign_method_builder::filter_histogram_names()
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

void same_sign_method_builder::remove_negative_bins(std::unique_ptr<TH1> &hist, bool no_zeroes)
{
    util::logging::debug << "Removing negative bins from " << hist->GetTitle() << std::endl;
    for (int i=0; i<=hist->GetNbinsX()+1; ++i) {
        if (hist->GetBinContent(i) < 0) {
            if (no_zeroes)
                hist->SetBinContent(i, 1e-9);
            else
                hist->SetBinContent(i, 0);
            hist->SetBinError(i, 0);
        }
    }
}

void same_sign_method_builder::smooth(std::unique_ptr<TH1> &hist, int smooth_amount)
{
    std::vector<double> input;
    for (int i=1; i<=hist->GetNbinsX(); ++i) {
        input.push_back(hist->GetBinContent(i));
    }

    int kernel_size = smooth_amount;
    int half_kernel_size = kernel_size / 2;
    // std::vector<double> kernel(kernel_size, 1 / kernel_size);

    for (int i = 0; i < input.size(); ++i) {
        double sum = 0.0;

        double kernel = 1.0 / double(kernel_size);
        if (i - half_kernel_size < 0) kernel = 1.0 / double(kernel_size - (i - half_kernel_size));

        for (int j = -half_kernel_size; j <= half_kernel_size; ++j) {
            int index = i + j;

            if (index >= 0 && index < input.size()) {
                sum += input[index] * kernel;
                // sum += input[index] * kernel[j + half_kernel_size];
            }
        }

        hist->SetBinContent(i+1, sum);
    }
}

} // namespace util
