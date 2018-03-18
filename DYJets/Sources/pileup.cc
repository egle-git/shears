#include "pileup.h"

#include "histo_set.h"
#include "weights.h"

namespace physics
{

pileup::pileup(util::job::info &info, int year, int mode)
    : EvtPuCntTruth(info.reader, "EvtPuCntTruth"),
      EvtVtxCnt(info.reader, "EvtVtxCnt"),
      _standalone_lrw(year, mode)
{
}

void pileup::reweight(weights &w)
{
    if (w.ismc()) {
        w.use_weight(_standalone_lrw.weight(*EvtPuCntTruth));
    }
}

void pileup::declare_histograms(util::histo_set &h) const
{
    h.declare("nvtx", "Number of vertices;#Vtx", 60, 0.5, 60.5);
}

void pileup::fill(util::histo_set &h, const std::string &tag, const weights &w)
{
    h.fill("nvtx", tag, *EvtVtxCnt, w.global_weight());
}
} // namespace physics
