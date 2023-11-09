#include "logging.h"
#include "emu_method_builder.h"
#include "top_reweight_emu_builder.h"

void usage(const std::string &program_name);

int main(int argc, char **argv)
{
    try {
        std::string tool = (argc >= 2 ? argv[1] : "emu-method");
        if (tool == "-h" || tool == "--help") {
            usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (tool[0] == '-' /* option */ || tool == "emu-method") {
            std::unique_ptr<util::emu_method_builder> builder;
            builder = std::make_unique<util::emu_method_builder>("dyjets");
            builder->parse_options(argc, argv);
            builder->build();
        } else if (tool == "emu-top") {
            std::unique_ptr<util::top_reweight_emu_builder> builder;
            builder = std::make_unique<util::top_reweight_emu_builder>("dyjets");
            builder->parse_options(argc, argv);
            builder->build();
        } else {
            throw std::runtime_error("Unknown tool '" + tool + "'");
        }
    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    } 
    return EXIT_SUCCESS;
}

void usage(const std::string &program_name)
{
    std::cerr << "Usage: " << program_name << " [tool] [options...]" << std::endl
              << std::endl
              << "Available tools:" << std::endl
              << "\temu-method (prompt lepton bkg estimation)" << std::endl
              << "\temu-top (emu method only on top backgrounds)" << std::endl
              << std::endl
              << "Use " << program_name << " <tool> --help for the corresponding list of options."
              << std::endl;
}
