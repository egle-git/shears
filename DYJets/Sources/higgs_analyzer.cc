#include "higgs_analyzer.h"

#include <algorithm>
#include <array>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "functions.h"
#include "lepton.h"

higgs_analyzer::higgs_analyzer(util::job::info &info, const util::options &opt)
    : EvtRunNum(info.reader, "EvtRunNum"),
      _jets(info, opt),
      _muons(info, opt, *this),
      _pileup(info, opt),
      _triggers(info),
      _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
      _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
      _weights(info),
      _zfinder_good(opt, "good Z"),
      _zfinder_bad(opt, "bad Z")
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraGH = opt.config["tables G-H"].as<util::tables>();
    }

    _jets.declare_histograms(*this);
    _pileup.declare_histograms(*this);
    declare("mass", "Dilepton mass", 40, 71, 111);
    declare("m4l", "Four-lepton mass", 48, 70, 310);
}

namespace /* anonymous */
{

void apply_trigger_sf(physics::weights &w,
                      const physics::lepton &mu1,
                      const physics::lepton &mu2,
                      const util::tables &tab)
{
    if (w.ismc()) {
        w.use_weight(tab.at("dimu trigger")
                         .getEfficiency(std::abs(mu1.raw_v.Eta()), std::abs(mu2.raw_v.Eta())));
    }
}
} // namespace anonymous

void higgs_analyzer::operator()()
{
    using namespace physics;

    _weights.process_event();

    if (!passes_trigger()) {
        return;
    }


    _pileup.fill(*this, "Zinc0jet_noweight", _weights);
    _pileup.reweight(_weights);

    std::vector<lepton> muons = _muons.get(_weights.isdata());

    TLorentzVector zz_p;
    bool found = false;

    std::vector<dilepton> good = _zfinder_good.find(muons);
    for (const dilepton &z1 : good) {
        // Build a lepton list without the ones used to build the good Z candidate
        std::vector<lepton> vetoed_muons = muons;
        vetoed_muons.erase(std::find(vetoed_muons.begin(), vetoed_muons.end(), z1.a));
        vetoed_muons.erase(std::find(vetoed_muons.begin(), vetoed_muons.end(), z1.b));

        // Loop on all bad Z candidates
        std::vector<dilepton> bad = _zfinder_bad.find(vetoed_muons);
        for (const dilepton &z2 : bad) {
            if (!dilepton::zmass_ordering(z1, z2)) {
                continue;
            }
            // List of leptons, sorted by ascending pt
            std::array<lepton, 4> used{ z1.a, z1.b, z2.a, z2.b };
            std::sort(used.begin(),
                      used.end(),
                      [](const lepton &a, const lepton &b) { return a.v.Pt() < b.v.Pt(); });

            // Lepton pt cuts
            if (used[3].v.Pt() < 20 || used[2].v.Pt() < 10) {
                continue;
            }

            // All M_ll > 4 GeV
            bool pass = true;
            for (auto it1 = used.begin(); it1 != used.end(); ++it1) {
                for (auto it2 = it1 + 1; it2 != used.end(); ++it2) {
                    const TLorentzVector &p1 = it1->v;
                    const TLorentzVector &p2 = it2->v;
                    if ((p1 + p2).M() < 4 || deltaR(p1, p2) < 0.02) {
                        pass = false;
                        break;
                    }
                }
                if (!pass) {
                    break;
                }
            }
            if (!pass) {
                continue;
            }

            // Other Z candidate
            /// @todo If 4e/4mu
            /*if (TODO)*/ {
                dilepton other1(z1.a, z1.a.charge == z2.a.charge ? z2.b : z2.a);
                dilepton other2(z1.b, z1.b.charge == z2.b.charge ? z2.a : z2.b);
                if (other2.v.M() < 12 && other1.distance_to_z() < z1.distance_to_z()) {
                    continue;
                }
            }

            zz_p = z1.v + z2.v;
            found = true;

            _muons.apply_sf(_weights,
                            {z1.a, z1.b, z2.a, z2.b},
                            select(_tables_eraBF, _tables_eraGH));
            break;
        }
        if (found) {
            break;
        }
    }

    if (found) {
        _muons.fill(*this, "Zinc0jet", muons, _weights);
        _pileup.fill(*this, "Zinc0jet", _weights);
        fill("m4l", "Hinc0jet", zz_p.M(), _weights.global_weight());
    }

#if 0
    // Only read jets once we have a Z
    std::vector<jet> jets = _jets.get();
    _jets.veto(jets, {best_candidate.a, best_candidate.b});

    _jets.fill(*this, "Zinc0jet_noweight", jets, _weights);
    _pileup.fill(*this, "Zinc0jet_noweight", _weights);
    _pileup.reweight(_weights);

    _muons.apply_sf(
        _weights, {best_candidate.a, best_candidate.b}, select(_tables_eraBF, _tables_eraGH));
    apply_trigger_sf(
        _weights, best_candidate.a, best_candidate.b, select(_tables_eraBF, _tables_eraGH));

    _jets.fill(*this, "Zinc0jet", jets, _weights);
    _muons.fill(*this, "Zinc0jet", muons, _weights);
    _pileup.fill(*this, "Zinc0jet", _weights);
    fill("mass", "Zinc0jet", best_candidate.v.M(), _weights.global_weight());
#endif
}

bool higgs_analyzer::passes_trigger() { return select(_mask_eraBG, _mask_eraH).passes(_triggers); }

void higgs_analyzer::write()
{
    _weights.write(this);
    histo_set::write();
}

po::options_description higgs_analyzer::options()
{
    return po::options_description("Physics options");
}

template <class T> T &higgs_analyzer::select(T &eraBG, T &eraGH)
{
    const unsigned run_threshold = 278820u; // start of Run G

    if (_weights.isdata() && *EvtRunNum < run_threshold) {
        return eraBG;
    } else {
        return eraGH;
    }
}
