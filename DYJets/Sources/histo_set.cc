#include "histo_set.h"

#include "logging.h"

namespace util
{

histo_set::histogram_type &histo_set::get(const std::string &name, const std::string &tag)
{
    auto it = _histograms.find({name, tag});
    if (it != _histograms.end()) {
        return it->second;
    } else {
        // Histogram doesn't exist (yet), create it
        auto model = _models.find(name);
        if (model != _models.end()) {
            // We rely on the model's copy constructor
            auto res = _histograms.emplace(std::make_pair(name, tag), model->second);
            return res.first->second;
        } else {
            throw std::logic_error("Histogram not declared: " + name);
        }
    }
}

void histo_set::write()
{
    logging::debug << "Writing histograms..." << std::endl;
    for (auto &pair : _histograms) {
        pair.second.Write(combined_name(pair.first.first, pair.first.second).c_str());
    }
    logging::debug << _histograms.size() << " histograms written." << std::endl;
}

std::string histo_set::combined_name(const std::string &name, const std::string &tag)
{
    if (tag.empty()) {
        return name;
    }
    return name + "_" + tag;
}
} // namespace util
