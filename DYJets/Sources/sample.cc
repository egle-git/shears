#include "sample.h"

#include <stdexcept>

#include <boost/filesystem.hpp>

#include <TFile.h>

namespace data
{

catalog sample::catalog() const { return data::catalog(_catalog, _bonzai_dir); }

std::shared_ptr<TFile> sample::histogram_file(const std::string &analyzer_name,
                                              const std::string &directory,
                                              const std::string &mode,
                                              int job_id) const
{
    using namespace boost::filesystem;

    // Create output directory if needed and it doesn't exist
    if (mode != "READ" && !is_directory(directory)) {
        if (exists(directory)) {
            // "directory" exists and is not a directory...
            throw std::runtime_error("Path " + directory + " exists and is not a directory");
        } else {
            util::logging::info << "Creating directory " << directory << std::endl;
            create_directories(directory);
        }
    }

    // Construct the filename
    std::string filename = directory + "/" + analyzer_name + "-" + name();
    if (job_id >= 0) {
        filename += "-" + std::to_string(job_id);
    }
    filename += ".root";

    util::logging::debug << "Histogram file for sample " << name() << ": " << filename << std::endl;

    // Open the file
    return std::shared_ptr<TFile>(TFile::Open(filename.c_str(), mode.c_str()));
}

std::vector<sample> sample::load(const util::options &opt)
{
    return opt.config["samples"].as<std::vector<sample>>();
}
} // namespace data

/// \cond
namespace YAML
{

template <> struct convert<data::sample>
{
    static bool decode(const Node &node, data::sample &sample)
    {
        if (!node["name"]) {
            throw std::runtime_error("Sample name is not set");
        }
        sample._name = node["name"].as<std::string>();

        if (!node["bonzai dir"]) {
            throw std::runtime_error("Sample bonzai dir is not set for " + sample._name);
        }
        sample._bonzai_dir = node["bonzai dir"].as<std::string>();

        if (!node["catalog"]) {
            throw std::runtime_error("Sample catalog is not set for " + sample._name);
        }
        sample._catalog = node["catalog"].as<std::string>();

        if (!node["type"]) {
            throw std::runtime_error("Sample type is not set for " + sample._name);
        }
        std::string type = node["type"].as<std::string>();
        if (type == "data") {
            sample._type = data::sample::data;
        } else if (type == "MC") {
            sample._type = data::sample::mc;
        } else if (type == "background") {
            sample._type = data::sample::background;
        } else {
            throw std::runtime_error("Sample type is not \"data\" or \"MC\" for " + sample._name);
        }

        return true;
    }
};
} // namespace YAML
/// \endcond
