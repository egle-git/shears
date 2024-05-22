#ifndef DYJETS_ANALYZER_SYST_H
#define DYJETS_ANALYZER_SYST_H

#include "dyjets_analyzer.h"

#include "TKey.h"
#include "TSystem.h"

// -- for adjusting PDF/scale weights for systematics
struct GenWeightInfo {
  double mean;
  double sigma;
  double lowerLimit;
  double upperLimit;
};

/// \brief Implements a \f$ Z \to 2l \f$ analysis.
class dyjets_analyzer_syst : public dyjets_analyzer {

private:
  // -- branches for MC theory uncertainties
  std::optional<TTreeReaderArray<float>> LHEPdfWeight;
  std::optional<TTreeReaderArray<float>> LHEScaleWeight;

  // -- class for a efficiency map (used in the unc. est. for the eff. SF)
  class EffMap {
  public:
    EffMap() {}

    EffMap(TString fileName, TString histName_effMap) {
      init_fromHistName(fileName, histName_effMap);
    }

    TString era() const { return _era; }
    TString lepType() const { return _lepType; }
    TString effType() const { return _effType; }
    TString dataType() const { return _dataType; }
    TString systType() const { return _systType; }
    TH2D* map() const { return _h2D_map; }
    TH2D* map_counter_cv() const { return _h2D_map_counter_cv; }

    int GetBinNumber(TString axis, double value) const {
      int i_bin = -1;

      int nBin;
      if( axis == "X" ) {
        nBin = _h2D_map->GetNbinsX();
        i_bin = _h2D_map->GetXaxis()->FindBin(value);
      }
      else if( axis == "Y" ) {
        nBin = _h2D_map->GetNbinsY();
        i_bin = _h2D_map->GetYaxis()->FindBin(value);
      }

      if( i_bin == 0 ) i_bin = 1; // -- extrapolate below the lowest bin edge
      if( i_bin == nBin+1) i_bin = nBin; // -- extrapolate above the highest bin edge

      return i_bin;
    }

  private:
    TString _histName = "";

    TString _era = ""; // -- 16pre, 16post, 17, 18
    TString _lepType = ""; // -- mu, el
    TString _effType = ""; // -- ID, ISO, STRIG, DTRIG (mu) / RECO, ID, TRIG_Leg1, TRIG_Leg2 (el)
    TString _dataType = ""; // -- data, mc, sf
    TString _systType = ""; // -- various types (cv, syst_altSig, syst_altBkg, ...)

    TH2D* _h2D_map = nullptr;

    // -- cv efficiency map of the counterpart (data -> mc, mc -> data)
    // -- for SF calculation, if dataType is efficiency, not SF
    TH2D* _h2D_map_counter_cv = nullptr;

    void init_fromHistName(TString fileName, TString histName) {
      _histName = histName;

      // -- histName format: (_era)-(_lepType)-(_effType)-(_dataType)-(_systType)
      // -- e.g. 18-mu-ID-data-syst_all_plus
      TObjArray* arr = _histName.Tokenize("-");
      if( arr->GetEntries() != 5 )
        throw std::runtime_error("name for the efficiency map (" + _histName + "): do not follow the rule");

      _era      = ( (TObjString*)(arr->At(0)) )->String();
      _lepType  = ( (TObjString*)(arr->At(1)) )->String();
      _effType  = ( (TObjString*)(arr->At(2)) )->String();
      _dataType = ( (TObjString*)(arr->At(3)) )->String();
      _systType = ( (TObjString*)(arr->At(4)) )->String();

      _h2D_map = get_map(fileName, _histName);

      if( _dataType != "sf" ) { // -- if map is efficiency, not sf, it also needs counterpart's efficiency (data or mc)
        TString _dataType_counter = _dataType == "data" ? "mc" : "data";

        TString histName_counter_cv = TString::Format(
          "%s-%s-%s-%s-cv", _era.Data(), _lepType.Data(), _effType.Data(), _dataType_counter.Data());

        _h2D_map_counter_cv = get_map(fileName, histName_counter_cv);
      }
    }

    TH2D* get_map(TString fileName, TString histName) {
      TH2D* h_return = nullptr;

      TH1::AddDirectory(false); // to keep histograms even after closing the input file

      TFile* f_input = !gSystem->AccessPathName(fileName) ? TFile::Open(fileName) : nullptr;
      if( f_input == nullptr )
        throw std::invalid_argument("File " + fileName + " under " + gSystem->GetWorkingDirectory() + " doesn't exist");

      h_return = f_input->GetListOfKeys()->Contains(histName) ?
                 (TH2D*)f_input->Get(histName)->Clone() : nullptr;
      if( h_return == nullptr )
        throw std::invalid_argument("Histogram " + histName + " doesn't exist in" + fileName);

      f_input->Close();

      return h_return;
    }
  };

public:
  /// \brief Constructor.
  dyjets_analyzer_syst(util::job::info &info, const util::options &opt);

  /// \brief Destructor.
  ~dyjets_analyzer_syst() = default;

  void operator()();


private:
  bool _isSignal = false;
  void readInfo_fromYAML(const util::options &opt);
  void sanity_check();

  bool _doSyst_effSF = false;
  bool _doSyst_muP = false; // -- Rochester correction (muon mommentum, p, correction)
  bool _doSyst_elE = false; // -- Electron energy correction
  bool _use_eRoccor = false; // -- which electron energy correction? (true if it is the Rochester one)
  bool _doSyst_pileup = false; // -- Pileup
  bool _doSyst_L1Pref = false; // -- L1 pre-firing
  bool _doSyst_theory = false; // -- MC theory uncertainty (PDF, alpha_s, scale)
  bool _doSyst_bVetoSF = false; // -- b-veto SF uncertainty
  bool _doSyst_emuMethodFit = false; // -- emu method reweighting fit parameter uncertainty
  bool _doSyst_emuMethodFakes = false; // -- emu method uncertainty related to the fake contribution to emu sample
  bool _doSyst_fakeSameSignFit = false; // -- same-sign method OS/SS reweighting fit parameter uncertainty
  bool _doSyst_fakeSameSignFitFun = false; // -- same-sign method OS/SS reweighting fit uncertainty (functional form)
  bool _doSyst_fakeSameSignEmuMeth = false; // -- using vs not using the emu method reweighting on same-sign EWK backgrounds
  bool _doSyst_fakeSameSignElChMisid = false; // -- electron charge misidentification correction for same-sign method

  std::string _fileName_effMap = ""; // -- .root file with systematic-varied efficiency maps

  std::string _channel = ""; // -- ee or mm
  double _sMuTrigPtCut = 0;

  // -- for the uncertainty from the eff. SF
  std::vector<EffMap> _vec_effMap;
  std::map<TString, EffMap> _map_type_effMapCV;

  // -- for the uncertainty from the theory
  vector<GenWeightInfo> vec_PDFWeightInfo_;
  // vector<GenWeightInfo> vec_scaleWeightInfo_;

  // -- for the uncertainty from the emu method fit parameters
  std::vector<double> _pars_emuMethodFitPlus, _pars_emuMethodFitMinus;

  //  -- for background reweighting uncertainties
  std::vector<double> _errs_emu_method = {};
  std::vector<double> _pars_fakesPlus_emu_method = {};
  std::vector<double> _pars_fakesMinus_emu_method = {};
  std::vector<std::vector<double>> _pars_plus_same_sign_method = {{}, {}, {}, {}, {}};
  std::vector<std::vector<double>> _pars_minus_same_sign_method = {{}, {}, {}, {}, {}};
  double _same_sign_reweight_const = 1;

  void fill_systHist_effSF(const util::matched<event_contents>& evt,
                           const util::matched<double> &value,
                           const util::matched<std::string>& tags_default);

  void calc_effSFRatio_systVariation(const std::vector<physics::lepton>& chosen_leptons,
                                     std::map<TString, double>& map_uncType_effSFRatio);

  double find_or_calculate_centralValueEffSF(const TString& type,
                                        const vector<physics::lepton>& chosen_leptons,
                                        std::map<TString, double>& map_type_effSFCV);

  double get_effSF(const std::vector<physics::lepton>& chosen_leptons, const EffMap& effMap);

  void get_abseta(const std::vector<physics::lepton>& chosen_leptons, double& abseta_lead, double& abseta_sub);

  double get_effSF_fromEff_mu(const std::vector<physics::lepton>& chosen_leptons, const EffMap& effMap);

  void init_effMap();

  // -- for the uncertainty from the muon momentum correction (Rochester correction)
  void fill_systHist_muP(const util::matched<event_contents>& evt,
                         const bool isLowQMuEvent,
                         const std::vector<physics::lepton>& genleps_fs,
                         const double rndm_forRoccor);

  void fill_systHist_muP_eachSystVar(
                             const util::matched<event_contents>& evt_default,
                             const bool isLowQMuEvent,
                             const std::vector<physics::lepton>& genleps_fs,
                             const double rndm_forRoccor,
                             const int s, const int m);

  std::string get_str_systInfo_muP(const int& s, const int& m);

  // -- for the uncertainty from the electron energy scale & smearing correction
  void fill_systHist_elE(const util::matched<event_contents>& evt_default,
                         const std::vector<physics::lepton>& genleps_dressed,
                         const std::vector<physics::lepton>& genleps_fs,
                         const double rndm_forRoccor);

  void fill_systHist_elE_RocCorr(const util::matched<event_contents>& evt_default,
                                 const std::vector<physics::lepton>& genleps_dressed,
                                 const std::vector<physics::lepton>& genleps_fs,
                                 const double rndm_forRoccor);

  void fill_systHist_elE_RocCorr_eachSystVar(const util::matched<event_contents>& evt_default,
                                             const std::vector<physics::lepton>& genleps_dressed,
                                             const std::vector<physics::lepton>& genleps_fs,
                                             const double rndm_forRoccor,
                                             int s, int m);

  void fill_systHist_elE_POGCorr(const util::matched<event_contents>& evt_default);
  void fill_systHist_elE_POGCorr_eachSystVar(const util::matched<event_contents>& evt_default, TString systMode);

  // -- for the uncertainty from the pileup
  void fill_systHist_pileup(const util::matched<event_contents>& evt,
                            const util::matched<double> &value,
                            const util::matched<std::string>& tags_default);

  // -- for the uncertainty from the L1 prefiring
  void fill_systHist_L1Pref(const util::matched<event_contents>& evt,
                            const util::matched<double> &value,
                            const util::matched<std::string>& tags_default);

  // -- for the uncertainty from theoretical inputs (PDF, alpha_s and scale)
  void fill_systHist_theory(const util::matched<event_contents>& evt,
                            const util::matched<double> &value,
                            const util::matched<std::string>& tags_default);

  // -- for the uncertainty from b-tagging SF used for b-veto
  void fill_systHist_bVetoSF(const util::matched<event_contents>& evt,
                             const util::matched<double> &value,
                             const util::matched<std::string>& tags_default);

  void Init_GenWeightInfo(const util::options &opt);
  void Adjust_PDFWeight(const int i_mem, double& ratio_weight);

  // -- for the background uncertainty related to the emu method fit parameters
  void fill_systHist_emuMethodFit(const util::matched<event_contents>& evt,
                                  const util::matched<double> &value,
                                  const std::string &sample_name,
                                  const util::matched<std::string>& tags_default);

  // -- for the background uncertainty related to the emu method fake backgrounds
  void fill_systHist_emuMethodFakes(const util::matched<event_contents>& evt,
                                    const util::matched<double> &value,
                                    const std::string &sample_name,
                                    const util::matched<std::string>& tags_default);

  // -- for the background uncertainty related to the same-sign method OS/SS reweighting fit parameter uncertainty
  void fill_systHist_fakeSameSignFit(const util::matched<event_contents> &evt,
                                     const util::matched<double> &value,
                                     const util::matched<std::string> &tags_default);
  void fill_systHist_fakeSameSignFit_eachSystVar(const util::matched<event_contents> &evt,
                                                 const util::matched<double> &value,
                                                 const util::matched<std::string> &tags_default,
                                                 const int ivar);

  // -- for the background uncertainty related to the same-sign method OS/SS reweighting fit uncertainty (functional form)
  void fill_systHist_fakeSameSignFitFun(const util::matched<event_contents> &evt,
                                        const util::matched<double> &value,
                                        const util::matched<std::string> &tags_default);

  // -- for the background uncertainty related to using vs not using the emu method reweighting on same-sign EWK backgrounds
  void fill_systHist_fakeSameSignEmuMeth(const util::matched<event_contents> &evt,
                                         const util::matched<double> &value,
                                         const std::string &sample_name,
                                         const util::matched<std::string> &tags_default);

  // -- for the background uncertainty related to the electron charge misidentification correction for same-sign method
  void fill_systHist_fakeSameSignElChMisid(const util::matched<event_contents> &evt,
                                           const util::matched<double> &value,
                                           const util::matched<std::string> &tags_default);

  // -- make a new tag with an additional string
  util::matched<std::string> make_newTag(const util::matched<event_contents>& evt,
                                         const util::matched<std::string>& tags_default, 
                                         TString uncType);
};

#endif // DYJETS_ANALYZER_SYST_H
