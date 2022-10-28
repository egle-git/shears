#include "electrons.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"

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
      Electron_mvaFall17V2Iso_WPL(info.reader, "Electron_mvaFall17V2Iso_WPL")
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
    
}

std::vector<lepton> electrons::get(int & nVetoElecs)
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
