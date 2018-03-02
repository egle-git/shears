#include <TCanvas.h>
#include <TFileIter.h>
#include <TGaxis.h>
#include <TH1.h>
#include <THStack.h>
#include <TPad.h>
#include <TROOT.h>

#include "logging.h"
#include "mc_group.h"
#include "options.h"
#include "sample.h"

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "higgs.yml", {});

        std::string input_dir = "higgs-histograms-max-files-1";

        // Initialize list of samples
        std::vector<data::sample> samples = data::sample::load(opt);

        // Initialize list of MC groups
        std::vector<data::mc_group> groups = data::mc_group::load(opt, samples);

        // Initialize list of histograms
        std::set<std::string> histogram_names;
        for (data::mc_group &group : groups) {
            group.add_histograms(histogram_names);
        }
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

            THStack stack("stack", "");
            for (auto it = groups.rbegin(); it != groups.rend(); ++it) {
                // Get the histogram
                TH1 *histo = it->get(name);

                // Add it to the stack
                if (histo != nullptr) {
                    stack.Add(histo);
                }
            }

            // Draw the MC stack
            stack.Draw("HIST");
            stack.GetYaxis()->SetLabelSize(0.04);
            stack.GetYaxis()->SetLabelOffset(0.002);
            stack.GetYaxis()->SetTitle("# Events");
            stack.GetYaxis()->SetTitleSize(0.04);
            stack.GetYaxis()->SetTitleOffset(1.32);
            stack.SetMinimum(8);
//            stack.SetMaximum(100 * stack.GetMaximum());

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
        }

    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
