#include "dyjets_analyzer.h"

#include <algorithm>
#include <array>
#include <random>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "functions.h"
#include "lepton.h"

namespace /* anonymous */
{
    double zpt_binning[] = {0.1,  1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,
                            10.,  11.,  12.,  13.,  14.,  16.,  18.,  20.,  22.,  25.,
                            28.,  32.,  37.,  43.,  52.,  65.,  85.,  120., 160., 190.,
                            220., 250., 300., 350., 400., 450., 500., 1000.};
}

dyjets_analyzer::dyjets_analyzer(util::job::info &info, const util::options &opt)
    : boson_jets_analyzer(info, opt),
      EvtRunNum(info.reader, "EvtRunNum"),
      _electrons(info, opt, *this),
      _jets(info, opt),
      _muons(info, opt, *this),
      _pileup(info, opt),
      _triggers(info),
      _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
      _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
      _weights(info),
      _zfinder(opt, "Z")
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraGH = opt.config["tables G-H"].as<util::tables>();
    }

    _jets.declare_histograms(*this);
    _pileup.declare_histograms(*this);

    _counter.declare("Total");
    _counter.declare("Passing the trigger");
    _counter.declare("With two good leptons");
    _counter.declare("With two good electrons");
    _counter.declare("With two good muons");
    _counter.declare("With a good Z boson");

    declare("mass", "Dilepton mass;M(ll) [GeV]", 40, 71, 111);
    declare("pt", "Dilepton p_{T};p_{T}(ll) [GeV]", sizeof(zpt_binning) / sizeof(double) - 1, zpt_binning);
}

namespace /* anonymous */
{

void apply_mu_trigger_sf(physics::weights &w,
                         const physics::lepton &mu1,
                         const physics::lepton &mu2,
                         const util::tables &tab)
{
    if (w.ismc()) {
        w.use_weight(tab.at("dimu trigger")
                        .getEfficiency(std::abs(mu1.raw_v.Eta()), std::abs(mu2.raw_v.Eta())));
    }
}

void apply_el_trigger_sf(physics::weights &w,
                         const physics::lepton &e1,
                         const physics::lepton &e2,
                         const util::tables &tab)
{
    if (w.ismc()) {
        w.use_weight(tab.at("diel trigger")
                        .getEfficiency(std::abs(e1.raw_v.Eta()), std::abs(e2.raw_v.Eta())));
    }
}
} // namespace anonymous

void dyjets_analyzer::operator()()
{
    using namespace physics;

    _weights.process_event();
    _counter.count("Total", _weights.global_weight());

    if (!passes_trigger()) {
        return;
    }
    _counter.count("Passing the trigger", _weights.global_weight());

    std::vector<lepton> muons = _muons.get(_weights.isdata());
    std::vector<lepton> electrons = _electrons.get();

    std::vector<lepton> leptons = find_boson(muons, electrons);
    if (leptons.size() != 2) {
        return;
    }

    // Only read jets once we have a Z
    std::vector<jet> jets = _jets.get();
    _jets.veto(jets, leptons);

    _jets.fill(*this, "Zinc0jet_noweight", jets, _weights);
    _pileup.fill(*this, "Zinc0jet_noweight", _weights);
    _pileup.reweight(_weights);

    if (muons.size() >= 2) {
        _muons.apply_sf(_weights, leptons, select(_tables_eraBF, _tables_eraGH));
        apply_mu_trigger_sf(_weights, leptons[0], leptons[1], select(_tables_eraBF, _tables_eraGH));
    } else {
        _electrons.apply_sf(_weights, leptons, select(_tables_eraBF, _tables_eraGH));
        apply_el_trigger_sf(_weights, leptons[0], leptons[1], select(_tables_eraBF, _tables_eraGH));
    }

    for (unsigned njets = 0; njets < 3; ++njets) {
        std::stringstream ss;
        ss << "Zinc" << njets << "jet";
        std::string tag = ss.str();

        _jets.fill(*this, tag, jets, _weights);
        if (muons.size() >= 2) {
            _muons.fill(*this, tag, leptons, _weights);
        } else {
            _electrons.fill(*this, tag, leptons, _weights);
        }
        _pileup.fill(*this, tag, _weights);
        fill("mass", tag, Z.v.M(), _weights.global_weight());
        fill("pt", tag, Z.v.Pt(), _weights.global_weight());

        if (jets.size() == njets) {
            ss.str("");
            ss << "Zexc" << njets << "jet";
            std::string tag = ss.str();

            _jets.fill(*this, tag, jets, _weights);
            if (muons.size() >= 2) {
                _muons.fill(*this, tag, leptons, _weights);
            } else {
                _electrons.fill(*this, tag, leptons, _weights);
            }
            _pileup.fill(*this, tag, _weights);
            fill("mass", tag, Z.v.M(), _weights.global_weight());
            fill("pt", tag, Z.v.Pt(), _weights.global_weight());

            break;
        }
    }
}

std::vector<physics::lepton> dyjets_analyzer::find_boson(
    const std::vector<physics::lepton> &muons,
    const std::vector<physics::lepton> &electrons)
{
    if (muons.size() < 2 && electrons.size() < 2) {
        return {};
    }
    _counter.count("With two good leptons", _weights.global_weight());

    std::vector<physics::lepton> leptons;
    if (muons.size() >= 2) {
        _counter.count("With two good muons", _weights.global_weight());
        leptons = muons;
    } else {
        _counter.count("With two good electrons", _weights.global_weight());
        leptons = electrons;
    }

    std::vector<physics::dilepton> candidates = _zfinder.find({leptons[0], leptons[1]});
    if (candidates.size() == 0) {
        return {};
    }
    _counter.count("With a good Z boson", _weights.global_weight());

    std::sort(candidates.begin(), candidates.end(), physics::dilepton::zmass_ordering);
    physics::dilepton Z = candidates[0];
    return { Z.a, Z.b };
}

bool dyjets_analyzer::passes_trigger() { return select(_mask_eraBG, _mask_eraH).passes(_triggers); }

void dyjets_analyzer::write()
{
    _counter.print();
    _weights.write(this);
    histo_set::write();
}

po::options_description dyjets_analyzer::options()
{
    return po::options_description("Physics options");
}

template <class T> T &dyjets_analyzer::select(T &eraBG, T &eraGH)
{
    const unsigned run_threshold = 278820u; // start of Run G
    const double run_lumi_fraction = 0.5493217216546642; // lumi fraction before run G

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> uniform(0.0, 1.0);

    if (_weights.isdata()) {
        if (*EvtRunNum < run_threshold) {
            return eraBG;
        } else {
            return eraGH;
        }
    } else {
        // Monte-Carlo based era selection
        if (uniform(gen) < run_lumi_fraction) {
            return eraBG;
        } else {
            return eraGH;
        }
    }
}
