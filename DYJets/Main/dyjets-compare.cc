
#include "logging.h"
#include "reco_compare_builder.h"

int main(int argc, char **argv)
{
    try {
        std::unique_ptr<util::compare_builder_base> builder;

        if (true) {
            builder = std::make_unique<util::reco_compare_builder>("dyjets");
        }

        builder->parse_options(argc, argv);
        builder->build();

        /*
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
                upper.SetBottomMargin(0.);

                ratio->Divide(den.get());

                ratio->SetMarkerStyle(20);
                ratio->SetMarkerColor(kBlack);
                ratio->SetLineColor(kBlack);

                ratio->GetXaxis()->SetTickLength(0.03);
                ratio->GetXaxis()->SetTitleSize(0.1);
                ratio->GetXaxis()->SetTitleOffset(1.2);
                ratio->GetXaxis()->SetLabelSize(0.10);
                ratio->GetXaxis()->SetLabelOffset(0.017);

                ratio->GetYaxis()->SetRangeUser(0.601, 1.399);
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
        */
    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
