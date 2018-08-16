#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <TCanvas.h>
#include <TFileIter.h>
#include <TGaxis.h>
#include <TH1.h>
#include <THStack.h>
#include <TLegend.h>
#include <TPad.h>
#include <TROOT.h>

#include "comparison_entry.h"
#include "logging.h"
#include "options.h"
#include "style_list.h"

namespace po = boost::program_options;

po::options_description options();

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "dyjets.yml", {options()});

        if (opt.map.count("os") != 1) {
            throw std::runtime_error("'os' argument must be given exactly once");
        }
        if (opt.map.count("ss") != 1) {
            throw std::runtime_error("'ss' argument must be given exactly once");
        }

        std::string os_input_dir = opt.map["os"].as<std::string>();
        std::string ss_input_dir = opt.map["ss"].as<std::string>();

        std::string output_dir = os_input_dir + "/ss-yield/";
        if (opt.map.count("output") > 0) {
            output_dir = opt.map["output"].as<std::string>();
        }

        // Read style
        util::style_list style;
        if (opt.config["plots"]) {
            style = util::style_list(opt.config["plots"]);
        }

        // Read plots
        data::mc_comparison_entry ss_mc_entry(opt, "dyjets", ss_input_dir);

        data::sample data;
        std::vector<data::sample> samples = data::sample::load(opt);
        for (data::sample &s : samples) {
            if (s.name() == "data") {
                data = s;
            }
        }
        data::data_comparison_entry os_data_entry("dyjets", data, os_input_dir);
        data::data_comparison_entry ss_data_entry("dyjets", data, ss_input_dir);

        double lumi = os_data_entry.lumi();
        util::logging::info << "Normalizing MC to " << (lumi / 1000) << " fb^-1" << std::endl;

        // Initialize list of histograms
        std::set<std::string> histogram_names;
        if (opt.map.count("histogram-name") > 0) {
            // Read from command line
            std::vector<std::string> names =
                opt.map["histogram-name"].as<std::vector<std::string>>();
            std::copy(names.begin(),
                      names.end(),
                      std::inserter(histogram_names, histogram_names.begin()));
        } else {
            // Detect automatically
            os_data_entry.add_histograms(histogram_names);
            ss_data_entry.add_histograms(histogram_names);
            ss_mc_entry.add_histograms(histogram_names);
            util::logging::info << "Found " << histogram_names.size() << " histograms."
                                << std::endl;

            // Remove histograms vetoed by style
            for (auto it = histogram_names.begin(); it != histogram_names.end(); ) {
                if (style.get<bool>("produce", *it, true)) {
                    ++it;
                } else {
                    util::logging::debug << "Not producing histogram " << *it
                                         << " due to plot rules." << std::endl;
                    it = histogram_names.erase(it);
                }
            }
        }

        bool log = (opt.map.count("lin") == 0);

        {
            using namespace boost::filesystem;

            // Create output directory if it doesn't exist
            if (!is_directory(output_dir)) {
                if (exists(output_dir)) {
                    // "output_dir" exists and is not a directory...
                    throw std::runtime_error("Path " + output_dir +
                                             " exists and is not a directory");
                } else {
                    util::logging::info << "Creating directory " << output_dir << std::endl;
                    create_directories(output_dir);
                }
            }
        }

        for (const std::string &name : histogram_names) {
            util::logging::debug << "Producing histogram: " << name << std::endl;

            TCanvas canvas(name.c_str(), "", 700, 900);

            TPad upper("upper", "upper", 0, 0.3, 1, 1);
            upper.SetTopMargin(0.11);
            upper.SetRightMargin(0.03);
            upper.SetTicks();
            if (log) {
                upper.SetLogy();
            }
            upper.Draw();
            upper.cd();

            ss_data_entry.draw(name, lumi);
            ss_mc_entry.draw(name, lumi, true);
            ss_data_entry.draw(name, lumi, true);

            // Legend
            TLegend legend(0.63, 0.60, 0.81, 0.87);
            legend.SetTextSize(0.042);
            legend.SetFillStyle(0);
            legend.SetBorderSize(0);
            legend.SetTextFont(42);
            legend.Draw();

            ss_data_entry.add_to_legend(legend, name, lumi);
            ss_mc_entry.add_to_legend(legend, name, lumi);

            // Get back to the canvas
            canvas.cd();

            TPad lower("lower", "lower", 0, 0.05, 1, 0.3);
            lower.SetTopMargin(0.);
            lower.SetBottomMargin(0.3);
            lower.SetRightMargin(0.03);
            lower.SetGridy();
            lower.SetLogy();
            lower.SetTicks();
            lower.Draw();
            lower.cd();

            std::unique_ptr<TH1> ratio = ss_data_entry.get(name, lumi);
            std::unique_ptr<TH1> ss_mc = ss_mc_entry.get(name, lumi);
            std::unique_ptr<TH1> os_data = os_data_entry.get(name, lumi);

            if (ratio != nullptr && os_data != nullptr) {
                if (ratio != nullptr) {
                    ratio->Add(ss_mc.get(), -1);
                }
                ratio->Divide(os_data.get());

                upper.SetBottomMargin(0.);

                ratio->SetMarkerStyle(20);

                ratio->GetXaxis()->SetTickLength(0.03);
                ratio->GetXaxis()->SetTitleSize(0.1);
                ratio->GetXaxis()->SetTitleOffset(1.2);
                ratio->GetXaxis()->SetLabelSize(0.10);
                ratio->GetXaxis()->SetLabelOffset(0.017);

                ratio->GetYaxis()->SetNdivisions(5, 5, 0);
                ratio->GetYaxis()->SetTitle("(SS Data - SS MC)/OS Data");
                ratio->GetYaxis()->SetTitleSize(0.1);
                ratio->GetYaxis()->SetTitleOffset(0.5);
                ratio->GetYaxis()->CenterTitle();
                ratio->GetYaxis()->SetLabelSize(0.08);

                ratio->SetStats(0);
                ratio->SetTitle("");
                ratio->Draw("ep");
            }

            // Apply style
            if (style.get<bool>("log x", name, false)) {
                upper.SetLogx();
                lower.SetLogx();
            }

            canvas.Print((output_dir + name + ".png").c_str());

            ss_data_entry.reset_drawing_state();
            ss_mc_entry.reset_drawing_state();
            os_data_entry.reset_drawing_state();
        }

    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

po::options_description options()
{
    po::options_description options = po::options_description("Comparison options");
    options.add_options()("os",
                          po::value<std::string>(),
                          "Sets the directory to search for OS histogram files");
    options.add_options()("ss",
                          po::value<std::string>(),
                          "Sets the directory to search for SS histogram files");
    options.add_options()("output,o", po::value<std::string>(), "Sets the output directory");
    options.add_options()("histogram-name,n",
                          po::value<std::vector<std::string>>(),
                          "Produce the given histogram (can be used several times)");
    options.add_options()("lin", "Use a linear scale for the y axis (the default is a log scale)");
    return options;
}
