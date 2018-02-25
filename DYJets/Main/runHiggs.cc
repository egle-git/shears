#include "catalog.h"
#include "higgs_analyzer.h"
#include "job.h"
#include "logging.h"
#include "options.h"
#include "timer.h"

int main(int argc, char **argv)
{
    try {
        options opt;
        opt.default_init(argc, argv, "higgs.yml", {higgs_analyzer::options(), job::options()});

        std::string fileName(opt.config["catalog"].as<std::string>());
        std::string bonzaiDir(opt.config["bonzai dir"].as<std::string>());
        catalog c(fileName, bonzaiDir);

        job j(c);
        j.configure(opt);
        j.run<higgs_analyzer>();

    } catch (std::exception &e) {
        logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
