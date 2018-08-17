#include "reco_compare_builder.h"

namespace util
{

reco_compare_builder::reco_compare_builder(const std::string &analyzer_name) :
    compare_builder_base(analyzer_name, analyzer_name + ".yml", "plots")
{
}

po::options_description reco_compare_builder::options() const
{
    po::options_description options = po::options_description("Comparison options");
    options.add_options()("input,i",
                          po::value<std::string>()->default_value("dyjets-histograms"),
                          "Sets the directory to search for histogram files");
    return options;
}

void reco_compare_builder::load()
{
    std::string input_dir = parsed_options().map["input"].as<std::string>();
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
}

void reco_compare_builder::fill_legend(TLegend &legend, const std::string &name)
{
    _data_entry->add_to_legend(legend, name, _lumi);
    _mc_entry->add_to_legend(legend, name, _lumi);
}

void reco_compare_builder::fill_lower_panel(const std::string &name)
{
    std::unique_ptr<TH1> ratio = _mc_entry->get(name, _lumi);
    std::unique_ptr<TH1> den = _data_entry->get(name, _lumi);

    if (ratio != nullptr && den != nullptr) {
        ratio->Divide(den.get());
        ratio->GetYaxis()->SetTitle("Simulation/Data");
        ratio->Draw("ep");
    }
}

void reco_compare_builder::reset_drawing_state()
{
    _mc_entry->reset_drawing_state();
    _data_entry->reset_drawing_state();
}

} // namespace util
