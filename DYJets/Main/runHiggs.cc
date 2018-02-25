#include "catalog.h"
#include "higgs_analyzer.h"
#include "job.h"
#include "logging.h"
#include "looper.h"
#include "options.h"
#include "timer.h"

int main(int argc, char **argv)
{
    options opt;
    opt.default_init(argc, argv, "higgs.yml", {higgs_analyzer::options(), job::options()});

    job::settings settings;
    settings << opt.config["job"] << opt.map;

    std::string fileName(opt.config["catalog"].as<std::string>());
    std::string bonzaiDir(opt.config["bonzai dir"].as<std::string>());
    catalog c(fileName, bonzaiDir);

    job j(c, settings);
    j.run<higgs_analyzer>();

    return 0;
}
