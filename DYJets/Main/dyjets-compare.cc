
#include "logging.h"
#include "reco_compare_builder.h"

int main(int argc, char **argv)
{
    try {
        std::unique_ptr<util::compare_builder_base> builder;

        if (true) {
            builder = std::make_unique<util::reco_compare_builder>("dyjets");
        }

        builder->parse_options(argc, argv);
        builder->build();

    } catch (std::exception &e) {
        util::logging::fatal << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
