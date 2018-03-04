#include "comparison_entry.h"

#include <TList.h>

#include "sample.h"

namespace data
{

mc_comparison_entry::mc_comparison_entry(const util::options &opt, const std::string &input_dir)
{
    std::vector<data::sample> samples = data::sample::load(opt);
    _groups = data::mc_group::load(opt, samples);
}

void mc_comparison_entry::add_histograms(std::set<std::string> &histos)
{
    for (data::mc_group &group : _groups) {
        group.add_histograms(histos);
    }
}

void mc_comparison_entry::draw(const std::string &name, double lumi)
{
    if (_stack == nullptr) {
        create_stack(name, lumi);
    }
    _stack->Draw("HIST");
    _stack->GetYaxis()->SetLabelSize(0.04);
    _stack->GetYaxis()->SetLabelOffset(0.002);
    _stack->GetYaxis()->SetTitle("# Events");
    _stack->GetYaxis()->SetTitleSize(0.04);
    _stack->GetYaxis()->SetTitleOffset(1.32);
}

std::unique_ptr<TH1> mc_comparison_entry::get(const std::string &name, double lumi)
{
    if (_stack == nullptr) {
        create_stack(name, lumi);
    }
    if (_stack->GetNhists() == 0) {
        return nullptr;
    }
    std::unique_ptr<TH1> res(dynamic_cast<TH1 *>(_stack->GetHists()->Last()));
    return res;
}

void mc_comparison_entry::reset_drawing_state()
{
    _stack.reset();
}

void mc_comparison_entry::create_stack(const std::string &name, double lumi)
{
    _stack = std::make_unique<THStack>("stack", "");
    for (auto it = _groups.rbegin(); it != _groups.rend(); ++it) {
        // Get the histogram
        TH1 *histo = it->get(name);

        // Add it to the stack
        if (histo != nullptr) {
            histo->Scale(lumi);
            _stack->Add(histo);
        }
    }
}
} // namespace data
