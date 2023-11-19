#include "electrons.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"

#include "Aepcor.h"

#include "TRandom.h"

namespace physics
{

electrons::electrons(util::job::info &info, const util::options &opt, util::histo_set &h)
    : Electron_pt(info.reader, "Electron_pt"),
      Electron_eta(info.reader, "Electron_eta"),
      Electron_phi(info.reader, "Electron_phi"),
      Electron_mass(info.reader, "Electron_mass"),
      Electron_charge(info.reader, "Electron_charge"),
      Electron_deltaEtaSC(info.reader, "Electron_deltaEtaSC"), //Iti: check
      //ElPfIsoRho(info.reader, "ElPfIsoRho"),
      Electron_cutBased(info.reader, "Electron_cutBased"),
      Electron_mvaFall17V2Iso(info.reader, "Electron_mvaFall17V2Iso"),
      Electron_mvaFall17V2Iso_WP80(info.reader, "Electron_mvaFall17V2Iso_WP80"),
      Electron_mvaFall17V2Iso_WP90(info.reader, "Electron_mvaFall17V2Iso_WP90"),
      Electron_mvaFall17V2Iso_WPL(info.reader, "Electron_mvaFall17V2Iso_WPL"),
      Electron_eCorr(info.reader, "Electron_eCorr"),
      Electron_r9(info.reader, "Electron_r9")
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    h.declare("elPt", "Electron pt;Electron p_{T} [GeV]", 50, 0, 200);
    h.declare("elEta", "Electron eta;Electron #eta", 24, -2.5, 2.5);
    h.declare("elPhi", "Electron phi;Electron #phi", 24, -pi, pi);
}

void electrons::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["electrons"];
    util::set_value_safe(node, _pt_cut, "pt", "electron pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "electron eta cut", [](double val) { return val > 0; });
    util::set_value_safe(node, _iso_cut, "isolation", "electron isolation cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _id_sf_enabled, "use id scale factors", "id scale factors toggle");
    util::set_value_safe(node, _reco_sf_enabled, "use reconstruction scale factors", "reconstruction scale factors toggle");

    if (node["id"]) {
        std::string id = node["id"].as<std::string>();
        if (id == "veto") {
            _id_cut = electrons::id::veto;
        } else if (id == "loose") {
            _id_cut = electrons::id::loose;
        } else if (id == "medium") {
            _id_cut = electrons::id::medium;
        } else if (id == "tight") {
            _id_cut = electrons::id::tight;
        } else if (id == "mva_wp80"){
            _id_cut == electrons::id::mva_wp80;
        }else if (id == "mva_wp90"){
            _id_cut == electrons::id::mva_wp90;
        } else {
            throw std::invalid_argument("Unknown electron id: \"" + id + "\"");
        }
    }

    if (node["use charge misid sf"]) {
        _charge_misid_sf_enabled = node["use charge misid sf"].as<bool>();
        if (_charge_misid_sf_enabled) {
            _charge_misid = physics::charge_misid(node);
        }
    }

    if( node["use rochester electron energy correction"] ) {
        _eRoccor_enabled = node["use rochester electron energy correction"].as<bool>();
        if( _eRoccor_enabled ) {
            _eRoccor = std::make_shared<Aepcor>();
            std::string eRoccor_dir = node["rochester energy correction path"].as<std::string>();
            util::logging::info << "rochester energy correction path: " + eRoccor_dir << std::endl;
            _eRoccor->init(eRoccor_dir, Aepres::CB);
        }
    }

    
}

std::vector<lepton> electrons::get(bool isData, const unsigned int runNum,
                                   const vector<lepton>& vec_dressedGenLep,
                                   const vector<lepton>& vec_postFSRGenLep,
                                   int & nVetoElecs)
{
    nVetoElecs=0;
    std::vector<lepton> electrons;
    for (unsigned i = 0; i < Electron_pt.GetSize(); ++i) {
        lepton l;
        if (std::abs((Electron_deltaEtaSC[i]+Electron_eta[i])) > _eta_cut) {
            continue;
        } else if (std::abs((Electron_deltaEtaSC[i]+Electron_eta[i])) > 1.4442 && std::abs((Electron_deltaEtaSC[i]+Electron_eta[i])) < 1.566) {
            // Veto endcap-barrel transition
            continue;
        }

        // printf("Electron_mass[i] = %lf\n", Electron_mass[i]);
        // printf("Electron_eCorr[i] = %lf --> 1.0/Electron_eCorr[i] = %lf\n", Electron_eCorr[i], (1.0/Electron_eCorr[i]));
        // printf("(pT_POGCorr, pT_raw) = (%.3lf, %.3lf)\n", Electron_pt[i], Electron_pt[i]/Electron_eCorr[i]);

        l.v.SetPtEtaPhiM(Electron_pt[i], Electron_eta[i], Electron_phi[i], Electron_mass[i]);
        l.raw_v.SetPtEtaPhiM(Electron_pt[i], (Electron_deltaEtaSC[i]+Electron_eta[i]), Electron_phi[i], Electron_mass[i]);

        l.charge = Electron_charge[i];
        //l.id = Electron_cutBased[i];
                
        l.pdgid = 11;

        switch (_id_cut) {
        case id::veto:
            l.id = Electron_cutBased[i];
            l.passes_id = (Electron_cutBased[i] >= 1);
            break;
        case id::loose:
            l.id = Electron_cutBased[i];
            l.passes_id = (Electron_cutBased[i] >= 2);
            break;
        case id::medium:
            l.id = Electron_cutBased[i];
            l.passes_id = (Electron_cutBased[i] >= 3);
            break;
        case id::tight:
            l.id = Electron_cutBased[i];
            l.passes_id = (Electron_cutBased[i] >= 4);
            break;
        case id::mva_wp80:
            l.id = Electron_mvaFall17V2Iso_WP80[i];
            l.passes_id = (Electron_mvaFall17V2Iso_WP80[i] >= 1);
        case id::mva_wp90:
            l.id = Electron_mvaFall17V2Iso_WP80[i];
            l.passes_id = (Electron_mvaFall17V2Iso_WP80[i] >= 1);
        }
        
        
        if (!l.passes_id) {
            continue;
        }

        if( _eRoccor_enabled ) {
          apply_energyCorr_smp22010(l, isData, 1.0/Electron_eCorr[i],
                                    runNum, Electron_r9[i],
                                    vec_dressedGenLep, vec_postFSRGenLep);
        }

        // printf("[After corr.] (pt, eta, phi, mass) = (%.2lf, %.3lf, %.3lf, %lf)\n", l.v.Pt(), l.v.Eta(), l.v.Phi(), l.v.M());
        // printf("\n");

        // -- pt cut after the correction
        if (l.v.Pt() < _pt_cut) {
            continue;
        }
        electrons.push_back(l);
    }
    std::sort(electrons.begin(), electrons.end(), [](const lepton &lhs, const lepton &rhs)
        {
            return lhs.v.Pt() > rhs.v.Pt();
        }
    );
    return electrons;
}

void electrons::apply_energyCorr_smp22010(lepton& l, 
                                          const bool isData, const double factorToRawE,
                                          const unsigned int runNum, const double r9, 
                                          const vector<lepton>& vec_dressedGenLep,
                                          const vector<lepton>& vec_postFSRGenLep) {
    TVector3 vecP3_POGCorr = l.v.Vect();
    TLorentzVector vecP_raw;
    vecP_raw.SetVectM(vecP3_POGCorr*factorToRawE, l.v.M()); // -- scale the 3-momentum only

    // printf("--> POG corr: (pt, eta, phi, mass) = (%.2lf, %.3lf, %.3lf, %.3lf)\n", l.v.Pt(), l.v.Eta(), l.v.Phi(), l.v.M());
    // printf("--> no corr:  (pt, eta, phi, mass) = (%.2lf, %.3lf, %.3lf, %.3lf)\n", vecP_raw.Pt(), vecP_raw.Eta(), vecP_raw.Phi(), vecP_raw.M());
    // printf("----> r9 = %.3f\n", r9);

    double pt = vecP_raw.Pt(); // -- pt "before" POG correction
    double eta = vecP_raw.Eta(); // -- eta, not etaSC
    double phi = vecP_raw.Phi();

    double eCorr = 1.0;
    if( isData )
        eCorr = _eRoccor->kScaleDT(pt, eta, phi, r9, runNum);
    else { // -- MC
        lepton genLep_matched = matchedGenLepton(l, vec_dressedGenLep);
        double pt_gen = genLep_matched.v.Pt();
        if( pt_gen == 0 )  { // -- i.e. no matched dressed lepton is found: try with postFSR
            lepton genLep_postFSR_matched = matchedGenLepton(l, vec_postFSRGenLep);
            pt_gen = genLep_postFSR_matched.v.Pt();
            // -- if no maching is found even with post-FSR leptons
            // -- it can happen if the reco-electron is not from the true electron
            // -- anyway, most of these electrons will not be used in the analysis (fail to pass pt cut or dilepton selections)
            // if( pt_gen == 0 )
            //     util::logging::warn << "[electrons::apply_energyCorr_smp22010] no matched gen-lepton (dressed and post-FSR) is found for the electron ... correction factor is set to 1.0" << endl;
        }

        if( pt_gen == 0 ) eCorr = 1.0;
        else {
            double urnd = gRandom->Rndm(); // uniform between 0 and 1
            eCorr = _eRoccor->kSpreadMC(pt, eta, phi, r9, urnd, pt_gen);
        }

        // printf("(pt, pt_gen) = (%lf, %lf) --> corr = %lf\n", pt, pt_gen, eCorr);
    }

    // printf("-->corr = %lf\n", eCorr);

    double pt_corr = pt*eCorr;
    double mass = l.v.M();
    // -- pt: corrected pT
    // -- eta: default eta, not etaSC (same with before)
    l.v.SetPtEtaPhiM(pt_corr, eta, phi, mass);
}

lepton electrons::matchedGenLepton(const lepton& l, const vector<lepton>& vec_genLep) {
    int nGenLep = (int)vec_genLep.size();
    int i_matched = -1;
    double dR_min = 1e10;
    double dRCut = 0.1;
    // -- find the gen-lepton with the smallest dR
    // -- (but the dR shoudl be at least less than 0.1)
    for(int i=0; i<nGenLep; ++i) {
        const lepton& genLep = vec_genLep[i];
        double dR_ith = l.v.DeltaR(genLep.v);
        if( dR_ith < dRCut && dR_ith < dR_min ) {
            i_matched = i;
            dR_min = dR_ith;
        }
    }

    if( i_matched < 0 ) {
        // util::logging::warn << "[electrons::matchedGenLepton] no matched gen-lepton is found" << std::endl;
        // printf("  [Given reco-lepton] (pt, eta, phi) = (%.3lf, %.3lf, %.3lf)\n", l.v.Pt(), l.v.Eta(), l.v.Phi());
        // for(const auto& genLep : vec_genLep ) {
        //     double dR = l.v.DeltaR( genLep.v );
        //     printf("----> [gen-lepton] (pt, eta, phi, dR) = (%.3lf, %.3lf, %.3lf, %.3lf)\n", 
        //                                                      genLep.v.Pt(), genLep.v.Eta(), genLep.v.Phi(), dR);
        // }
        lepton l_null = l;
        l_null.v.SetPtEtaPhiM(0,0,0,0);
        l_null.raw_v.SetPtEtaPhiM(0,0,0,0);
        return l_null;
    }

    return vec_genLep[i_matched];
}

void electrons::apply_sf(weights &w,
                         const std::vector<lepton> &electrons,
                         const util::tables &tab) const
{
    if (w.ismc()) {
        for (const lepton &el : electrons) {
            if (_reco_sf_enabled) {
                w.use_weight(
                    tab.at("electron reco").getEfficiency(el.v.Pt(), el.raw_v.Eta()));
            }
            if (_id_sf_enabled) {
                w.use_weight(tab.at("electron id")
                                .getEfficiency(el.v.Pt(), el.raw_v.Eta()));
            }
            
        }
    }
}

void electrons::apply_charge_misid_sf(physics::weights &weights,
                                      const std::vector<physics::lepton> &_electrons,
                                      const std::vector<physics::lepton> &_genleps)
{
    if (_charge_misid_sf_enabled && weights.ismc()) {
        if (_electrons.size() && _genleps.size()) {
            auto electrons = _electrons;
            auto genleps = _genleps;
            std::sort(electrons.begin(),
                      electrons.end(),
                      [](const physics::lepton &a, const physics::lepton &b) {
                          return a.v.Pt() > b.v.Pt(); // Sort in descending order
                      });

            std::sort(genleps.begin(),
                      genleps.end(),
                      [](const physics::lepton &a, const physics::lepton &b) {
                          return a.v.Pt() > b.v.Pt(); // Sort in descending order
                      });

            std::vector<int> matches(electrons.size(), -1);
            std::vector<double> drmins(electrons.size(), 99999.9);

            for (unsigned iel = 0; iel < electrons.size(); iel++) {
                if (matches[iel] >= 0 && drmins[iel] < 99999) continue; // We already found a match

                for (unsigned igen = 0; igen < genleps.size(); igen++) {
                    if (std::abs(genleps[igen].pdgid) != 11) continue;

                    double dr = electrons[iel].raw_v.DeltaR(genleps[igen].v);

                    if (dr < drmins[iel]) {
                        auto it = std::find(matches.begin(), matches.end(), igen);
                        if (it == matches.end()) {
                            drmins[iel] = dr;
                            matches[iel] = igen;
                        } else {
                            int index = std::distance(matches.begin(), it);
                            if (dr < drmins[index]) {
                                // We have found a better match for one of the previous gen leptons
                                // Rerun matching for a previous reco lepton
                                drmins[iel] = dr;
                                matches[iel] = igen;
                                drmins[index] = 99999.9;
                                matches[index] = -1;
                                if (index > 0) iel = index - 1;
                                break;
                            }
                        }
                    }
                }
            }// for (electrons)

            if (std::find(matches.begin(), matches.end(), -1) != matches.end())
                util::logging::warn << "Not all gen leptons found for charge_misid!" << std::endl;

            for (unsigned iel = 0; iel < electrons.size(); iel++) {
                if (matches[iel] >= 0) {
                    if (electrons[iel].charge != genleps[matches[iel]].charge) {
                        weights.use_weight(_charge_misid.get_sf(electrons[iel]));
                    }
                }
            }
        } // if(electrons.size())
    } // if (ismc)
} // apply_charge_misid_sf()

void electrons::fill(util::histo_set &h,
                     const std::string &tag,
                     const std::vector<lepton> &electrons,
                     const weights &w)
{
    for (const lepton &el : electrons) {
        h.fill("elPt", tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 0) {
        const lepton &el = electrons[0];
        h.fill("elPt", "leading_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "leading_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "leading_" + tag, el.v.Phi(), w.global_weight());
        
    }
    if (electrons.size() > 1) {
        const lepton &el = electrons[1];
        h.fill("elPt", "subleading_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "subleading_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "subleading_" + tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 2) {
        const lepton &el = electrons[2];
        h.fill("elPt", "third_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "third_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "third_" + tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 3) {
        const lepton &el = electrons[3];
        h.fill("elPt", "fourth_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "fourth_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "fourth_" + tag, el.v.Phi(), w.global_weight());
    }
}
} // namespace physics
