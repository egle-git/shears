#include "logging.h"
#include "options.h"

int main(int argc, char **argv)
{
    util::options opt;
    try {
        opt.default_init(argc, argv, "higgs.yml", {});

    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
