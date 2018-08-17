#include "compare_builder_base.h"

#include <boost/filesystem.hpp>

#include <TCanvas.h>
#include <TLegend.h>
#include <TPad.h>

namespace util
{

compare_builder_base::compare_builder_base(const std::string &analyzer_name,
                                           const std::string &default_config_file) :
    _analyzer_name(analyzer_name),
    _default_config_file(default_config_file)
{
}

void compare_builder_base::parse_options(int argc, char **argv)
{
    auto options = this->options();

    options.add_options()("output,o", po::value<std::string>(), "Sets the output directory");
    options.add_options()("histogram-name,n",
                          po::value<std::vector<std::string>>(),
                          "Produce the given histogram (can be used several times)");
    options.add_options()("lin", "Use a linear scale for the y axis (the default is a log scale)");

    _opt.default_init(argc, argv, _default_config_file, {options});

    if (_opt.map.count("output") > 0) {
        _output_dir_name = _opt.map["output"].as<std::string>();
    }

    if (_opt.config["plots"]) {
        _style = util::style_list(_opt.config["plots"]);
    }
}

void compare_builder_base::build()
{
    load();
    create_output_dir();
    filter_histogram_names();

    bool log = (_opt.map.count("lin") == 0);

    for (const std::string &name : _histogram_names) {
        util::logging::debug << "Producing histogram: " << name << std::endl;

        TCanvas canvas(name.c_str(), "", 700, 900);

        // Upper panel
        TPad upper("upper", "upper", 0, 0.3, 1, 1);
        upper.SetTopMargin(0.11);
        upper.SetRightMargin(0.03);
        upper.SetTicks();
        if (log) {
            upper.SetLogy();
        }
        upper.Draw();
        upper.cd();

        fill_upper_panel(name);

        // Legend
        TLegend legend(0.63, 0.60, 0.81, 0.87);
        legend.SetTextSize(0.042);
        legend.SetFillStyle(0);
        legend.SetBorderSize(0);
        legend.SetTextFont(42);
        legend.Draw();

        fill_legend(legend, name);

        // Get back to the canvas
        canvas.cd();

        // Lower panel
        TPad lower("lower", "lower", 0, 0.05, 1, 0.3);
        lower.SetTopMargin(0.);
        lower.SetBottomMargin(0.3);
        lower.SetRightMargin(0.03);
        lower.SetGridy();
        lower.SetTicks();
        lower.Draw();
        lower.cd();

        fill_lower_panel(name);

        // Apply style
        if (_style.get<bool>("log x", name, false)) {
            upper.SetLogx();
            lower.SetLogx();
        }

        // Write file
        canvas.Print((_output_dir_name + "/" + name + ".png").c_str());

        // Cleanup
        reset_drawing_state();
    }
}

std::unique_ptr<data::data_comparison_entry> compare_builder_base::load_data(
        const std::string &input_dir)
{
    data::sample data;
    std::vector<data::sample> samples = data::sample::load(_opt);
    for (data::sample &s : samples) {
        if (s.name() == "data") {
            data = s;
        }
    }

    auto ptr = std::make_unique<data::data_comparison_entry>(_analyzer_name, data, input_dir);
    ptr->add_histograms(_histogram_names);
    return ptr;
}

std::unique_ptr<data::mc_comparison_entry> compare_builder_base::load_mc(
        const std::string &input_dir)
{
    auto ptr = std::make_unique<data::mc_comparison_entry>(_opt, _analyzer_name, input_dir);
    ptr->add_histograms(_histogram_names);
    return ptr;
}

void compare_builder_base::create_output_dir() const
{
    using namespace boost::filesystem;

    // Create output directory if it doesn't exist
    if (!is_directory(_output_dir_name)) {
        if (exists(_output_dir_name)) {
            // "_output_dir_name" exists and is not a directory...
            throw std::runtime_error("Path " + _output_dir_name +
                                     " exists and is not a directory");
        } else {
            util::logging::info << "Creating directory " << _output_dir_name << std::endl;
            create_directories(_output_dir_name);
        }
    }
}

void compare_builder_base::filter_histogram_names()
{
    if (_opt.map.count("histogram-name") > 0) {
        // Read from command line
        std::vector<std::string> names =
            _opt.map["histogram-name"].as<std::vector<std::string>>();

        // Check that names exist
        for (auto name : names) {
            if (std::find(_histogram_names.begin(),
                          _histogram_names.end(),
                          name) == _histogram_names.end()) {
                throw std::runtime_error("Histogram " + name + " doesn't exist");
            }
        }

        _histogram_names.clear();
        std::copy(names.begin(),
                  names.end(),
                  std::inserter(_histogram_names, _histogram_names.begin()));
    } else {
        // Produce everything
        auto before_filter = _histogram_names.size();

        util::logging::info << "Found " << _histogram_names.size() << " histograms."
                            << std::endl;

        // Remove histograms vetoed by style
        for (auto it = _histogram_names.begin(); it != _histogram_names.end(); ) {
            if (_style.get<bool>("produce", *it, true)) {
                ++it;
            } else {
                util::logging::debug << "Not producing histogram " << *it
                                     << " due to plot rules." << std::endl;
                it = _histogram_names.erase(it);
            }
        }

        auto removed = before_filter - _histogram_names.size();
        util::logging::info << removed << " histograms excluded due to style rules."
                            << std::endl;
    }
}

} // namespace util
