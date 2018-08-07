#include "boson_jets_analyzer.h"

namespace physics
{

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info,
                                         const util::options &opt) :
    _rng(std::random_device()())
{}

void boson_jets_analyzer::operator()()
{
    analyze();
}

} // namespace physics
