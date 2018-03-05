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

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "higgs.yml", {});

        std::string input_dir = "higgs-histograms-max-files-1";
        double lumi = 1;

        data::mc_comparison_entry mc_entry(opt, input_dir);


        data::sample data;
        std::vector<data::sample> samples = data::sample::load(opt);
        for (data::sample &s : samples) {
            if (s.name() == "data") {
                data = s;
            }
        }
        data::data_comparison_entry data_entry(data, input_dir);

        // Initialize list of histograms
        std::set<std::string> histogram_names;
        mc_entry.add_histograms(histogram_names);
        data_entry.add_histograms(histogram_names);

        util::logging::info << "Found " << histogram_names.size() << " histograms." << std::endl;

        for (const std::string &name : histogram_names) {
            util::logging::debug << "Producing histogram: " << name << std::endl;

            TCanvas canvas(name.c_str(), "", 692, 844);

            /*
            TPad upper("upper", "upper", 0, 0.3, 1, 1);
            upper.SetTopMargin(0.11);
            upper.SetBottomMargin(0.);
            upper.SetRightMargin(0.03);
            upper.SetTicks();
            upper.SetLogy();
            upper.Draw();
            upper.cd();
            */
            canvas.SetLogy();

            data_entry.draw(name, lumi);
            mc_entry.draw(name, lumi, true);

            /*
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

            stack.Draw();
            */

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
