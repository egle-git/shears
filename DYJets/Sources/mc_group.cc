#include "mc_group.h"

#include <stdexcept>

#include <TFileIter.h>
#include <TH1.h>

#include "logging.h"

namespace data
{

void mc_group::add_histograms(std::set<std::string> &histos)
{
    for (sample_data &sd : _sample_data) {
        if (sd.file == nullptr) {
            continue;
        }

        // Iterate on keys
        for (TFileIter it(sd.file.get()); it < it.TotalKeys(); ++it) {
            histos.insert(it.GetKeyName());
        }
    }
}

TH1 *mc_group::get(const std::string &name)
{
    TH1 *res = nullptr;
    for (sample_data &sd : _sample_data) {
        if (sd.file == nullptr) {
            continue;
        }

        TH1 *histo = nullptr;
        sd.file->GetObject(name.c_str(), histo);
        if (histo == nullptr) {
            util::logging::warn << "Histogram " << name << " not found in file " << sd.file->GetName()
                                << std::endl;
            continue;
        }

        histo->Scale(sd.scale);

        if (res == nullptr) {
            res = histo;
        } else {
            res->Add(histo);
        }
    }
    if (res != nullptr) {
        res->SetStats(0);
        res->SetFillStyle(1001);
        res->SetFillColor(_color);
        res->SetLineColor(_color);
        res->Scale(_scale_factor);
    }
    return res;
}

void mc_group::init(const std::vector<sample> &all_samples,
                    const std::string &analyzer_name,
                    const std::string &input_dir)
{
    bool all_found = true;
    for (const std::string &name : _sample_names) {
        sample_data sd;

        // Find sample
        bool found = false;
        for (const sample &sample : all_samples) {
            if (name == sample.name()) {
                sd.sample = sample;
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error("Sample " + name + " (required by MC group " + _legend +
                                     ") doesn't exist.");
        }

        // Open file
        sd.file = sd.sample.histogram_file(analyzer_name, input_dir);

        if (sd.file == nullptr) {
            util::logging::warn << "File not found for sample " << name << std::endl;
            all_found = false;
        } else {
            // Read job info histograms
            TH1 *job_info = nullptr;
            sd.file->GetObject("_job_info", job_info);
            if (job_info == nullptr) {
                throw std::runtime_error("File " + std::string(sd.file->GetName()) +
                                         " doesn't have the _job_info histogram.");
            }

            TH1 *job_info_average = nullptr;
            sd.file->GetObject("_job_info_average", job_info_average);
            if (job_info_average == nullptr) {
                throw std::runtime_error("File " + std::string(sd.file->GetName()) +
                                         " doesn't have the _job_info_average histogram.");
            }

            double wsum = job_info->GetBinContent(2);
            double xsec = job_info_average->GetBinContent(2);

            sd.scale *= xsec / wsum;
        }

        _sample_data.push_back(sd);
    }
    if (!all_found) {
        if (_required) {
            throw std::runtime_error("Missing files for group " + _legend);
        } else {
            util::logging::warn << "Sample " << _legend << " is not complete" << std::endl;
        }
    }
}

std::vector<mc_group> mc_group::load(const util::options &opt,
                                     const std::string &analyzer_name,
                                     const std::string &input_dir,
                                     const std::vector<sample> &all_samples)
{
    std::vector<mc_group> groups = opt.config["MC grouping"].as<std::vector<mc_group>>();
    for (mc_group &g : groups) {
        g.init(all_samples, analyzer_name, input_dir);
    }
    return groups;
}
} // namespace data

/// \cond
namespace YAML
{

template <> struct convert<data::mc_group>
{
    static bool decode(const Node &node, data::mc_group &group)
    {
        if (!node["legend"]) {
            throw std::runtime_error("MC group legend is not set");
        }
        group._legend = node["legend"].as<std::string>();

        if (!node["color"]) {
            throw std::runtime_error("MC group color is not set for " + group._legend);
        }
        group._color = node["color"].as<int>();

        if (node["required"]) {
            group._required = node["required"].as<bool>();
        }

        if (node["scale factor"]) {
            group._scale_factor = node["scale factor"].as<double>();
        }

        if (!node["samples"]) {
            throw std::runtime_error("MC group list of samples is not set for " + group._legend);
        }
        group._sample_names = node["samples"].as<std::vector<std::string>>();

        return true;
    }
};
} // namespace YAML
/// \endcond
