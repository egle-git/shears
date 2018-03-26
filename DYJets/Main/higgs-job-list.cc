#include "mc_group.h"
#include "sample.h"

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "higgs.yml", {/* No additional options */});

        const std::vector<data::sample> samples = data::sample::load(opt);
        const std::vector<data::mc_group> groups =
            data::mc_group::load(opt, "higgs", "", samples, false);

        for (const data::mc_group &group : groups) {
            for (const data::sample &sample : group.samples()) {
                std::cout << "Main/higgs-loop -s " << sample.name();
                std::cout << std::endl;
            }
        }
    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
