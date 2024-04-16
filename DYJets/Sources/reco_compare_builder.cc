#include "reco_compare_builder.h"
#include "TFile.h"
#include "TLatex.h"

namespace util
{

reco_compare_builder::reco_compare_builder(const std::string &analyzer_name) :
    compare_builder_base(analyzer_name, analyzer_name + ".yml"),
    _reversed(false)
{
}

po::options_description reco_compare_builder::options() const
{
    po::options_description options = po::options_description("Comparison options");
    options.add_options()("input,i",
                          po::value<std::string>()->default_value("dyjets-histograms"),
                          "Sets the directory to search for histogram files");
    options.add_options()("reversed,r", "Plot Data/MC instead of MC/Data");
    return options;
}

void reco_compare_builder::load()
{
    std::string input_dir = parsed_options().map["input"].as<std::string>();
    set_default_output_dir(input_dir + "/plots");

    _reversed = (parsed_options().map.count("reversed") > 0);

    _data_entry = load_data(input_dir);
    _mc_entry = load_mc(input_dir);

    _lumi = _data_entry->lumi();
    util::logging::info << "Normalizing MC to " << (_lumi / 1000) << " fb^-1" << std::endl;
}

void reco_compare_builder::fill_upper_panel(const std::string &name)
{
    _data_entry->draw(name, _lumi);
    _mc_entry->draw(name, _lumi, true);
    _data_entry->draw(name, _lumi, true);
    // Fix axis ranges so that both data and MC are visible
    if (_data_entry->get(name, _lumi) != nullptr && _mc_entry->get(name, _lumi) != nullptr) {
        
        if (_mc_entry->integral(name, _lumi) > _data_entry->integral(name, _lumi)) {
            double min = _data_entry->get(name, _lumi)->GetMinimum(0) < 100.0 ? _data_entry->get(name, _lumi)->GetMinimum(0) : 100.0;
            _data_entry->get_y_axis(name, _lumi)->SetRangeUser(min, _mc_entry->get(name, _lumi)->GetMaximum() * 1.5);
        } else {
            double min = _mc_entry->get(name, _lumi)->GetMinimum(0) < 100.0 ? _mc_entry->get(name, _lumi)->GetMinimum(0) : 100.0;
            _data_entry->get_y_axis(name, _lumi)->SetRangeUser(min, _data_entry->get(name, _lumi)->GetMaximum() * 1.5);
        }
    }

    if (auto axis = _mc_entry->get_x_axis(name, _lumi)) {
        format_upper_x_axis(*axis);
    }
    if (auto axis = _data_entry->get_x_axis(name, _lumi)) {
        format_upper_x_axis(*axis);
    }
}

void reco_compare_builder::fill_legend(TLegend &legend, const std::string &name)
{
    _data_entry->add_to_legend(legend, name, _lumi);
    _mc_entry->add_to_legend(legend, name, _lumi);
}

bool reco_compare_builder::fill_lower_panel(const std::string &name)
{
    std::unique_ptr<TH1> num = _mc_entry->get(name, _lumi);
    std::unique_ptr<TH1> den = _data_entry->get(name, _lumi);

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
                        _reversed ? "Data/Simulation" : "Simulation/Data");

    double ratio_min = style().get<double>("ratio min", name, 0.601);
    double ratio_max = style().get<double>("ratio max", name, 1.399);
    _ratio->GetYaxis()->SetRangeUser(ratio_min, ratio_max);

    _ratio->SetMarkerStyle(20);
    _ratio->SetMarkerColor(kBlack);
    _ratio->SetLineColor(kBlack);
    _ratio->SetStats(0);
    _ratio->SetTitle("");
    _ratio->Draw("ep");

    double int1, int2, myratio;
    int1 = _data_entry ->get(name, _lumi) ->Integral(1, _data_entry->get(name, _lumi)->GetNbinsX());
    int2 = _mc_entry ->get(name, _lumi) ->Integral(1, _mc_entry->get(name, _lumi)->GetNbinsX());
    myratio = int2/int1;
    TString name_h = _data_entry->get(name, _lumi)->GetName();
    if (name_h.Contains("mass_wide_range")){
        std::cout<< "MC event: "<< int2 << std::endl;
        std::cout<< "Data events: " << int1 << std::endl;
        std::cout<<"Average MC/data rate: " << myratio << std::endl;
        TLatex latex_ratio;
        TString ratioInfo = TString::Format("Overall MC/DATA = %.3lf", myratio);
        latex_ratio.DrawLatexNDC(0.16, 0.85, "#font[42]{#scale[1.5]{#color[2]{"+ratioInfo+"}}}");
    }

    return true;
}

void reco_compare_builder::reset_drawing_state()
{
    _mc_entry->reset_drawing_state();
    _data_entry->reset_drawing_state();
    _ratio.reset();
}

} // namespace util
