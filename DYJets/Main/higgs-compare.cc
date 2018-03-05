#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <TCanvas.h>
#include <TFileIter.h>
#include <TGaxis.h>
#include <TH1.h>
#include <THStack.h>
#include <TPad.h>
#include <TROOT.h>

#include "comparison_entry.h"
#include "logging.h"
#include "options.h"

namespace po = boost::program_options;

po::options_description options();

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "higgs.yml", {options()});

        std::string input_dir = opt.map["input"].as<std::string>();

        data::mc_comparison_entry mc_entry(opt, "higgs", input_dir);

        data::sample data;
        std::vector<data::sample> samples = data::sample::load(opt);
        for (data::sample &s : samples) {
            if (s.name() == "data") {
                data = s;
            }
        }
        data::data_comparison_entry data_entry("higgs", data, input_dir);

        double lumi = data_entry.lumi();

        // Initialize list of histograms
        std::set<std::string> histogram_names;
        mc_entry.add_histograms(histogram_names);
        data_entry.add_histograms(histogram_names);

        util::logging::info << "Found " << histogram_names.size() << " histograms." << std::endl;

        for (const std::string &name : histogram_names) {
            util::logging::debug << "Producing histogram: " << name << std::endl;

            TCanvas canvas(name.c_str(), "", 692, 844);

            TPad upper("upper", "upper", 0, 0.3, 1, 1);
            upper.SetTopMargin(0.11);
            upper.SetBottomMargin(0.);
            upper.SetRightMargin(0.03);
            upper.SetTicks();
            upper.SetLogy();
            upper.Draw();
            upper.cd();

            mc_entry.draw(name, lumi);
            data_entry.draw(name, lumi, true);

            // Get back to the canvas
            canvas.cd();

            TPad lower("lower", "lower", 0, 0.05, 1, 0.3);
            lower.SetTopMargin(0.);
            lower.SetBottomMargin(0.3);
            lower.SetRightMargin(0.03);
            lower.SetGridy();
            lower.SetTicks();
            lower.Draw();
            lower.cd();

            std::unique_ptr<TH1> ratio = mc_entry.get(name, lumi);
            std::unique_ptr<TH1> den = data_entry.get(name, lumi);

            if (ratio != nullptr && den != nullptr) {
                ratio->Divide(den.get());

                ratio->SetMarkerStyle(20);
                ratio->SetMarkerColor(kBlack);
                ratio->SetLineColor(kBlack);

                ratio->GetXaxis()->SetTickLength(0.03);
                ratio->GetXaxis()->SetTitleSize(0.1);
                ratio->GetXaxis()->SetTitleOffset(1.2);
                ratio->GetXaxis()->SetLabelSize(0.10);
                ratio->GetXaxis()->SetLabelOffset(0.017);

                ratio->GetYaxis()->SetRangeUser(0.801, 1.199);
                ratio->GetYaxis()->SetNdivisions(5, 5, 0);
                ratio->GetYaxis()->SetTitle("Simulation/Data");
                ratio->GetYaxis()->SetTitleSize(0.1);
                ratio->GetYaxis()->SetTitleOffset(0.5);
                ratio->GetYaxis()->CenterTitle();
                ratio->GetYaxis()->SetLabelSize(0.08);

                ratio->SetStats(0);
                ratio->SetTitle("");
                ratio->Draw("ep");
            }

            canvas.Print((name + ".png").c_str());

            mc_entry.reset_drawing_state();
            data_entry.reset_drawing_state();
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
    options.add_options()("input,i",
                          po::value<std::string>()->default_value("higgs-histograms"),
                          "Sets the directory to search for histogram files");
    options.add_options()("output,o", po::value<std::string>(), "Sets the output directory");
    options.add_options()("histo,h",
                          po::value<std::vector<std::string>>(),
                          "Enable the only given histogram (can be used several times)");
    return options;
}
