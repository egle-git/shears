#include "comparison_entry.h"

#include <TFile.h>
#include <TFileIter.h>
#include <TH1.h>
#include <THStack.h>
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

void mc_comparison_entry::draw(const std::string &name, double lumi, bool same)
{
    if (_stack == nullptr) {
        create_stack(name, lumi);
    }
    _stack->Draw(same ? "hist same" : "hist");
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

///////////////////////////////////////////////////////////////////////////////////////////////////

data_comparison_entry::data_comparison_entry(const std::string &analyzer_name,
                                             const sample &sample,
                                             const std::string &input_dir)
    : _file(sample.histogram_file(analyzer_name, input_dir)),
      _histo(nullptr)
{
}

void data_comparison_entry::add_histograms(std::set<std::string> &histos)
{
    if (_file == nullptr) {
        return;
    }
    // Iterate on keys
    for (TFileIter it(_file.get()); it < it.TotalKeys(); ++it) {
        histos.insert(it.GetKeyName());
    }
}

void data_comparison_entry::draw(const std::string &name, double lumi, bool same)
{
    if (_histo == nullptr) {
        create_histo(name);
        if (_histo == nullptr) {
            return;
        }
    }
    _histo->Draw(same ? "e same" : "e");
    _histo->SetMarkerStyle(20);
    _histo->SetMarkerColor(kBlack);
    _histo->SetLineColor(kBlack);
}

std::unique_ptr<TH1> data_comparison_entry::get(const std::string &name, double lumi)
{
    if (_histo == nullptr) {
        create_histo(name);
        if (_histo == nullptr) {
            return nullptr;
        }
    }
    std::unique_ptr<TH1> res(dynamic_cast<TH1 *>(_histo->Clone()));
    return res;
}

void data_comparison_entry::reset_drawing_state()
{
    _histo = nullptr; // Will be deleted by TFile
}

double data_comparison_entry::lumi() const
{
    if (_file == nullptr) {
        throw std::runtime_error("Trying to get the lumi from a non-existing sample.");
    }

    // Read job info histograms
    TH1 *job_info = nullptr;
    _file->GetObject("_job_info", job_info);
    if (job_info == nullptr) {
        throw std::runtime_error("File " + std::string(_file->GetName()) +
                                 " doesn't have the _job_info histogram.");
    }

    TH1 *job_info_average = nullptr;
    _file->GetObject("_job_info_average", job_info_average);
    if (job_info_average == nullptr) {
        throw std::runtime_error("File " + std::string(_file->GetName()) +
                                 " doesn't have the _job_info_average histogram.");
    }

    double frac = job_info->GetBinContent(1);
    double lumi = job_info_average->GetBinContent(1);

    return frac * lumi;
}

void data_comparison_entry::create_histo(const std::string &name)
{
    if (_file != nullptr) {
        _histo = nullptr;
        _file->GetObject(name.c_str(), _histo);
        if (_histo == nullptr) {
            util::logging::warn << "Histogram " << name << " not found in file " << _file->GetName()
                                << std::endl;
            return;
        }

    }
}
} // namespace data
