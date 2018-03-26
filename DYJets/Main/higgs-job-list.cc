#include "sample.h"

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "higgs.yml", {/* No additional options */});

        const std::vector<data::sample> samples = data::sample::load(opt);

        for (const data::sample &sample : samples) {
            for (unsigned job = 0; job < sample.jobs(); ++job) {
                std::cout << "Main/higgs-loop -s " << sample.name();
                if (sample.jobs() > 1) {
                    std::cout << " --job-id " << job;
                    std::cout << " --job-count " << sample.jobs();
                }
                std::cout << std::endl;
            }
        }
    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
