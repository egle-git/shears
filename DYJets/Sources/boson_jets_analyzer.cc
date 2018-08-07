#include "boson_jets_analyzer.h"

namespace physics
{

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info,
                                         const util::options &opt) :
    _rng(std::random_device()()),
    _weights(info)
{}

void boson_jets_analyzer::operator()()
{
    _weights.process_event();

    analyze();
}

} // namespace physics
