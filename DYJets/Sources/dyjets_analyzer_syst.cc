#include "dyjets_analyzer_syst.h"

dyjets_analyzer_syst::dyjets_analyzer_syst(util::job::info &info, const util::options &opt):
  dyjets_analyzer(info, opt),
  LHEPdfWeight(info.init_optional_branch<decltype(LHEPdfWeight)>("LHEPdfWeight")),
  LHEScaleWeight(info.init_optional_branch<decltype(LHEScaleWeight)>("LHEScaleWeight")) {
    TString sampleName = get_sample_name();
    _isSignal = sampleName.Contains("DYJets_M") ? true : false;

    using physics::zfinder;
    if( _zfinder.get_flavor_mode() == zfinder::flavor_mode::ee )   _channel = "ee";
    if( _zfinder.get_flavor_mode() == zfinder::flavor_mode::mumu ) _channel = "mm";

    if( get_era() == 2 ) _sMuTrigPtCut = 27.0; // -- 2017 data
    else                 _sMuTrigPtCut = 24.0;

    readInfo_fromYAML(opt);
    if( _doSyst_effSF ) init_effMap();

    sanity_check();

    if( _doSyst_theory ) {
      // histo_set.declare("pdfWRatio", "PDF weights ratio;PDF weights ratio", 5000, 0, 5);
      // histo_set.declare("pdfWRatio", "PDF weights ratio;PDF weights ratio", 10000, 0, 10000);
      histo_set.declare("pdfWRatio",      "PDF weights ratio;PDF weights ratio", 8000, -3, 5);
      histo_set.declare("pdfWRatio_wide", "PDF weights ratio;PDF weights ratio", 2000, -1000000, 1000000);
      Init_GenWeightInfo(opt);
    }
}

void dyjets_analyzer_syst::Init_GenWeightInfo(const util::options &opt) {
  const YAML::Node node = opt.config["uncertainties"];
  TString baseDir = node["GenWeightInfo directory"].as<std::string>();

  TString tag_era;
  if( get_era() == 0 ) tag_era = "16pre";
  if( get_era() == 1 ) tag_era = "16post";
  if( get_era() == 2 ) tag_era = "17";
  if( get_era() == 3 ) tag_era = "18";
  TString tstr_channel = _channel;
  TString fileName = TString::Format("%s/WeightInfo_%s_%s.root", baseDir.Data(), tstr_channel.Data(), tag_era.Data());
  util::logging::info << "Gen weight information is from " << fileName << std::endl;

  TFile *f_input = TFile::Open(fileName);
  
  TString sampleName = get_sample_name();

  // -- only when the information for the given sample is available
  if( f_input->Get("h_mean_"+sampleName) == nullptr ) {
    f_input->Close();
    vec_PDFWeightInfo_.clear();
    return;
  }

  TH1D* h_mean       = (TH1D*)f_input->Get("h_mean_"+sampleName)->Clone();
  TH1D* h_sigma      = (TH1D*)f_input->Get("h_sigma_"+sampleName)->Clone();
  TH1D* h_lowerLimit = (TH1D*)f_input->Get("h_lowerLimit_"+sampleName)->Clone();
  TH1D* h_upperLimit = (TH1D*)f_input->Get("h_upperLimit_"+sampleName)->Clone();

  Int_t nWeight = h_mean->GetNbinsX();
  for(Int_t i=0; i<nWeight; ++i) {
    Int_t i_bin = i+1;

    TString label = h_mean->GetXaxis()->GetBinLabel(i_bin);

    Double_t mean       = h_mean->GetBinContent(i_bin);
    Double_t sigma      = h_sigma->GetBinContent(i_bin);
    Double_t lowerLimit = h_lowerLimit->GetBinContent(i_bin);
    Double_t upperLimit = h_upperLimit->GetBinContent(i_bin);

    if( label.Contains("PDFVar_") ) {
      GenWeightInfo info;
      info.mean = mean;
      info.sigma = sigma;
      info.lowerLimit = lowerLimit;
      info.upperLimit = upperLimit;

      vec_PDFWeightInfo_.push_back(info); // -- same order with the one of histogram bins (0th element = 1st bin)
    }

    // // -- if necessary...
    // if( label.Contains("scaleVar_") ) {

    // }
  }

  f_input->Close();
}


void dyjets_analyzer_syst::readInfo_fromYAML(const util::options &opt) {
  if( !opt.config["uncertainties"] ) {
    util::logging::warn << "no node for uncertainties in .yml file" << std::endl;
    return;
  }

  const YAML::Node node = opt.config["uncertainties"];

  util::set_value_safe(node, _doSyst_theory, "PDF and scale", "calculate systematic variations from the theoretical inputs (PDF, alpha_s and scale)");
  util::set_value_safe(node, _doSyst_pileup, "pileup", "calculate systematic variations from min-bias x-section unc. used in pileup reweighting correction");
  util::set_value_safe(node, _doSyst_L1Pref, "L1 prefiring", "calculate systematic variations from L1 prefiring unc.");
  util::set_value_safe(node, _doSyst_muP,    "Muon Rochester correction", "calculate systematic variations from muon Roccor. unc. (mm channel only)");
  util::set_value_safe(node, _doSyst_elE,    "Electron energy correction", "calculate systematic variations from electron energy correction unc. (ee channel only)");
  util::set_value_safe(node, _doSyst_effSF,  "efficiency SF", "calculate systematic variations from the uncertainty of the efficiency SF");
  if( _channel == "ee" && _doSyst_elE )
    _use_eRoccor = (opt.config["electrons"])["use rochester electron energy correction"].as<bool>();

  if( _doSyst_effSF )
    _fileName_effMap = node["efficiency file"].as<std::string>();

  if( _doSyst_theory ) util::logging::info << "Systematic variation for PDF and scale is ON" << std::endl;
  if( _doSyst_pileup ) util::logging::info << "Systematic variation for the pileup is ON" << std::endl;
  if( _doSyst_L1Pref ) util::logging::info << "Systematic variation for the L1 prefiring is ON" << std::endl;
  if( _doSyst_muP )    util::logging::info << "Systematic variation for the muon Roccor. is ON (mm-only)" << std::endl;
  if( _doSyst_elE )    util::logging::info << "Systematic variation for the electron energy correction is ON (ee-only)" << std::endl;
  if( _doSyst_elE ) {
    if( _use_eRoccor ) util::logging::info << "--> Variation on the Rochester electron energy correction" << std::endl;
    else               util::logging::info << "--> Variation on the E/gamma POG electron energy correction" << std::endl;
  }
  if( _doSyst_effSF )  util::logging::info << "Systematic variation for the efficiency SF is ON" << std::endl;
}

void dyjets_analyzer_syst::sanity_check() {
  if( _channel == "" )
    throw std::invalid_argument("_channel = " + _channel + " is not properly set");
}

// -- selection procedure: identical with dyjets_analyzer::operator()
// ---- if there is an update in dyjets_analyzer::operator(), this should also be updated accordingly
// -- keep the necessary histograms only for the systematic uncertainty estimation
void dyjets_analyzer_syst::operator()() {
  // -- pre-processing steps -- //
  // -- check before counting this event
  if( _selectDYLL ) {
      if( !_genleps.IsGivenFlavorDileptonEvent(_selectDYLL_flavor) ) return;
  }

  // -- check before counting this event
  if( _apply_M100Cut && _sample_name == "DYJets_M-50to100" ) {
      if( DileptonMass_LHE() > 100.0 ) return;
  }

  _weights.process_event();
  counter.count("Total", weights().global_weight());

  util::matched<event_contents> evt; // -- event info.

  // -- generator-level selection -- //
  // -- run only when it is signal MC sample (to save time)
  if( _isSignal ) {
    std::vector<physics::lepton> genleptons = find_gen_boson( _genleps.get() );

    if (!genleptons.empty()) {
        // Gen boson found
        evt.gen = event_contents();
        evt.gen->leptons = genleptons;
        evt.gen->boson_p = std::accumulate(
            genleptons.begin(),
            genleptons.end(),
            TLorentzVector(),
            [](const TLorentzVector &p, const physics::lepton &lep) { return p + lep.v; });
    }
  }

  // -- reco-level selection -- //
  // -- trigger
  bool triggered = passes_trigger();
  if( !evt.gen && !triggered ) return; // -- early termination

  double rndm_forRoccor;
  std::vector<physics::lepton> genleps_finalState;
  std::vector<physics::lepton> genleps_dressed_noCut;

  bool isLowQMuEvent = false;
  if( triggered ) { // -- if triggered: proceed to the event selection
    counter.count("Passing the trigger", weights().global_weight());

    // -- not used; dummy
    int nVetoMuons=0;
    int nVetoElecs=0;

    // final state (post-FSR) gen-muons used for the Rochester correction
    if( _weights.ismc() ) genleps_finalState = _genleps.get_leptons_finalState();
    else                  genleps_finalState.clear(); // -- data: no gen-leptons

    // -- dressed leptons, without any pt or eta cut (for the electron energy correction)
    if( _weights.ismc() ) genleps_dressed_noCut = _genleps.get_leptons_dressed_noCut();
    else                  genleps_dressed_noCut.clear(); // data: no gen-leptons

    rndm_forRoccor = gRandom->Rndm();
    std::vector<physics::lepton> muons = _muons.get(weights().isdata(), genleps_finalState, rndm_forRoccor);    
    if( _select_bestMuonTrigSF && _reject_lowQMu ) 
      isLowQMuEvent = check_lowQualityMuon(muons);

    std::vector<physics::lepton> electrons = _electrons.get(weights().isdata(), *run,
                                                            genleps_dressed_noCut, genleps_finalState, 
                                                            rndm_forRoccor, nVetoElecs);

    std::vector<physics::lepton> leptons = find_boson(muons, electrons);
    if( !leptons.empty() && (nVetoMuons+nVetoElecs)<=2 && !isLowQMuEvent ) {
        // Rec boson found
        evt.rec = event_contents();
        evt.rec->leptons = leptons;
        evt.rec->boson_p = std::accumulate(
            leptons.begin(),
            leptons.end(),
            TLorentzVector(),
            [](const TLorentzVector &p, const physics::lepton &lep) { return p + lep.v; });
    }
  }

  if( !evt.gen && !evt.rec ) return; // -- early termination

  // remove events with m(reco) < 10 GeV in both data and MC
  // because we only have the DY MC down to m=10 GeV, but data can go down below GeV
  // without this cut, the # underflow events in the response matrix could be significant different between data and MC
  if( evt.rec && evt.rec->boson_p.M() < 10.0 ) return;

  // -- now, at least one boson is found in either gen or rec.
  // -- apply corrections
  if( evt.rec ) {
    // -- PU reweighting
    if( _weights.ismc() ) _pileup.reweight(_weights);

    // -- L1 pre-firing weight
    if( _weights.ismc() && _applyPref_ )
      _weights.use_weight(*L1PreFiringWeight_Nom);

    // -- efficiency SF
    std::vector<physics::lepton> chosen_muons, chosen_electrons;
    std::copy_if(evt.rec->leptons.begin(),
                 evt.rec->leptons.end(),
                 std::back_inserter(chosen_muons),
                 [](const physics::lepton &lep) { return lep.pdgid == 13; });
    std::copy_if(evt.rec->leptons.begin(),
                 evt.rec->leptons.end(),
                 std::back_inserter(chosen_electrons),
                 [](const physics::lepton &lep) { return lep.pdgid == 11; });

    // -- Check the pt of the leading muon to know which SF we will use
    if( _select_bestMuonTrigSF )
        _use_smu_triggerSF = check_whichTriggerSF(chosen_muons);
    else
        _use_smu_triggerSF = true; // always use single muon trigger SF

    _muons.apply_sf(_weights, chosen_muons, tables());
    _electrons.apply_sf(_weights, chosen_electrons, tables());

    // -- apply charge mis-ID weights for background estimation
    if (_weights.ismc())
        _electrons.apply_charge_misid_sf(_weights, chosen_electrons, genleps_finalState);

    // -- reweight top-quark backgrounds using the factor from the emu method
    reweight_backgrounds(_weights, _sample_name, evt.rec->get_boson_p(), _met.v());

    if( _apply_triggerSF )
      apply_trigger_sf(_weights, evt.rec->leptons, _use_smu_triggerSF);
  }

  // -- fill histograms
  auto mass = evt.apply(&event_contents::get_boson_p).apply(&TLorentzVector::M);
  // auto pt = evt.apply(&event_contents::get_boson_p).apply((double (TLorentzVector::*)() const) &TLorentzVector::Pt);
  // auto rapidity = evt.apply(&event_contents::get_boson_p).apply(&TLorentzVector::Rapidity);

  util::matched<std::string> tags_default;
  if( evt.gen ) tags_default.gen = "inc0jet";
  else          tags_default.gen = boost::none;
  if( evt.rec ) tags_default.rec = "inc0jet";
  else          tags_default.rec = boost::none;
  fill_unfolded("mass_wide_range", tags_default, mass);

  ///////////////////////////////////////////  
  // -- fill with systematic variations -- //
  ///////////////////////////////////////////
  if( _weights.ismc() ) {
    if( _doSyst_effSF )  fill_systHist_effSF(evt, mass, tags_default);
    if( _doSyst_pileup ) fill_systHist_pileup(evt, mass, tags_default);
    if( _doSyst_L1Pref ) fill_systHist_L1Pref(evt, mass, tags_default);
    if( _doSyst_theory ) fill_systHist_theory(evt, mass, tags_default);
  }
  if( _doSyst_muP && _channel == "mm" )
    fill_systHist_muP(evt, isLowQMuEvent, genleps_finalState, rndm_forRoccor);
  if( _doSyst_elE && _channel == "ee" )
    fill_systHist_elE(evt, genleps_dressed_noCut, genleps_finalState, rndm_forRoccor);
}

void dyjets_analyzer_syst::fill_systHist_theory(const util::matched<event_contents>& evt,
                                                const util::matched<double> &value,
                                                const util::matched<std::string>& tags_default) {
  
  // -- some samples do not have these branches
  if( !LHEPdfWeight || !LHEScaleWeight ) return;

  double gen_weight_cv    = weights().gen_weight();
  double global_weight_cv = weights().global_weight();

  // -- PDF & alpha_s uncertainty
  // -- i_mem = 0:           central value (alpha_s = 0.118)
  // -- i_mem = 1 to 100:    PDF replicas
  // -- i_mem = 101 and 102: alpha_s variation (0.116 and 0.120)
  for(unsigned int i_mem=0; i_mem<LHEPdfWeight->GetSize(); ++i_mem) {
    TString tstr_PDFVarInfo = TString::Format("PDFVar_%03d", i_mem);

    double ratio_weight = LHEPdfWeight->At(i_mem);

    // -- fill before fixing the ratio
    // -- this is not a physical distribution; no need to use gen-weight to fill the histogram
    histo_set.fill("pdfWRatio",      tstr_PDFVarInfo.Data(), ratio_weight, 1.0);
    histo_set.fill("pdfWRatio_wide", tstr_PDFVarInfo.Data(), ratio_weight, 1.0);

    // if( std::abs(ratio_weight) > 1000.0 ) {
    //   //   TString massInfo = "";
    //   //   if( !value.gen ) massInfo = "no gen-mass";
    //   //   else             massInfo = TString::Format("mass = %lf", *value.gen);
    //   //   TString info = TString::Format("[%s (%s, gen_weight = %.2lf)] ratio_PDFWeight = %lf", tstr_PDFVarInfo.Data(), massInfo.Data(), gen_weight_cv, ratio_weight);
    //   //   util::logging::info << info.Data() << std::endl;
    //   ratio_weight = 1.0; // -- force it to be 1.0
    // }

    // -- if ratio_weight is outside of 5-sigma range w.r.t mean -> force it to be the mean value
    // -- to remove unphysical effect due to huge weight (e.g. >10000) 
    Adjust_PDFWeight(i_mem, ratio_weight);

    double gen_weight_PDFVar    = gen_weight_cv    * ratio_weight;
    double global_weight_PDFVar = global_weight_cv * ratio_weight;

    util::matched<std::string> tags_PDFVar;
    if( evt.gen ) tags_PDFVar.gen = *tags_default.gen + "_" + tstr_PDFVarInfo.Data();
    else          tags_PDFVar.gen = boost::none;
    if( evt.rec ) tags_PDFVar.rec = *tags_default.rec + "_" + tstr_PDFVarInfo.Data();
    else          tags_PDFVar.rec = boost::none;
    fill_unfolded("mass_wide_range", tags_PDFVar, value, gen_weight_PDFVar, global_weight_PDFVar);
  }

  // -- scale uncertainty
  // -- definition of each case:
  // -- https://cms-nanoaod-integration.web.cern.ch/autoDoc/NanoAODv9/2016ULpreVFP/doc_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8_RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v1.html#LHEScaleWeight
  for(unsigned int i_case=0; i_case<LHEScaleWeight->GetSize(); ++i_case) {
    TString tstr_scaleVarInfo = TString::Format("scaleVar_%03d", i_case);

    double ratio_weight = LHEScaleWeight->At(i_case);

    // -- fill before fixing the ratio
    // -- this is not a physical distribution; no need to use gen-weight to fill the histogram
    histo_set.fill("pdfWRatio",      tstr_scaleVarInfo.Data(), ratio_weight, 1.0);
    histo_set.fill("pdfWRatio_wide", tstr_scaleVarInfo.Data(), ratio_weight, 1.0);

    if( std::abs(ratio_weight) > 1000.0 ) {
      //   TString massInfo = "";
      //   if( !value.gen ) massInfo = "no gen-mass";
      //   else             massInfo = TString::Format("mass = %lf", *value.gen);
      //   TString info = TString::Format("[%s (%s, gen_weight = %.2lf)] ratio_PDFWeight = %lf", tstr_PDFVarInfo.Data(), massInfo.Data(), gen_weight_cv, ratio_weight);
      //   util::logging::info << info.Data() << std::endl;
      ratio_weight = 1.0; // -- force it to be 1.0
    }

    double gen_weight_scaleVar    = gen_weight_cv    * ratio_weight;
    double global_weight_scaleVar = global_weight_cv * ratio_weight;

    
    util::matched<std::string> tags_scaleVar;
    if( evt.gen ) tags_scaleVar.gen = *tags_default.gen + "_" + tstr_scaleVarInfo.Data();
    else          tags_scaleVar.gen = boost::none;
    if( evt.rec ) tags_scaleVar.rec = *tags_default.rec + "_" + tstr_scaleVarInfo.Data();
    else          tags_scaleVar.rec = boost::none;

    fill_unfolded("mass_wide_range", tags_scaleVar, value, gen_weight_scaleVar, global_weight_scaleVar);
  }
}

void dyjets_analyzer_syst::Adjust_PDFWeight(const int i_mem, double& ratio_weight) {
  if( vec_PDFWeightInfo_.size() == 0 ) return; // -- no info was saved? --> no adjustment is needed
  if( i_mem == 0 ) return; // -- central value
  if( i_mem > 102 ) return; // -- no info above 102

  int index = i_mem-1; // -- 0th element: i_mem = 1 i.e. PDFVar_001 (not 000)
  GenWeightInfo& info = vec_PDFWeightInfo_[index];

  Bool_t isNominal = (info.lowerLimit < ratio_weight && ratio_weight < info.upperLimit);

  // printf("[i_mem = %d]\n", i_mem);
  // printf("(mean, sigma, lowerLimit, upperLimit) = (%.3lf, %.3lf, %.3lf, %.3lf)\n",
  //          info.mean, info.sigma, info.lowerLimit, info.upperLimit);
  // printf("ratio_weight = %lf --> isNominal? = %d\n", ratio_weight, isNominal);

  if( !isNominal )
    ratio_weight = info.mean;
}

void dyjets_analyzer_syst::fill_systHist_elE(const util::matched<event_contents>& evt_default,
                                             const std::vector<physics::lepton>& genleps_dressed,
                                             const std::vector<physics::lepton>& genleps_fs,
                                             const double rndm_forRoccor) {
  if( _use_eRoccor ) fill_systHist_elE_RocCorr(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor);
  else               fill_systHist_elE_POGCorr(evt_default);
}

void dyjets_analyzer_syst::fill_systHist_elE_RocCorr(const util::matched<event_contents>& evt_default,
                                                     const std::vector<physics::lepton>& genleps_dressed,
                                                     const std::vector<physics::lepton>& genleps_fs,
                                                     const double rndm_forRoccor) {
  // -- re-do event selection because electrons can be rejected due to the pT cut
  // -- details on each variation
  // ---- https://gitlab.cern.ch/akhukhun/aepcor
  // ---- https://cms.cern.ch/iCMS/jsp/db_notes/noteInfo.jsp?cmsnoteid=CMS AN-2021/034
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 0, 0); // -- default
  for(int i_mem=0; i_mem<100; ++i_mem) // -- m = 1: stat. replicas (up to 400 replicas are available; use 100 for now)
    fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 1, i_mem);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 2, 0);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 3, 0);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 4, 0);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 5, 0);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 6, 0);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 7, 0);
  fill_systHist_elE_RocCorr_eachSystVar(evt_default, genleps_dressed, genleps_fs, rndm_forRoccor, 8, 0);
}

void dyjets_analyzer_syst::fill_systHist_elE_RocCorr_eachSystVar(const util::matched<event_contents>& evt_default,
                                                                 const std::vector<physics::lepton>& genleps_dressed,
                                                                 const std::vector<physics::lepton>& genleps_fs,
                                                                 const double rndm_forRoccor,
                                                                 int s, int m) {
  // -- re-do the event selection
  // -- because the event itself can be rejected by the pT cut due to the electron energy variation

  // -- event for this systematic variation
  util::matched<event_contents> evt_systVar;

  // -- generator level: just copy the default value (no change with this systematic variation)
  if( evt_default.gen ) {
    evt_systVar.gen = event_contents();
    evt_systVar.gen->leptons = evt_default.gen->leptons;
    evt_systVar.gen->boson_p = evt_default.gen->boson_p;
  }

  // -- reco-level selection -- //
  // -- trigger
  bool triggered = passes_trigger();
  if( !evt_systVar.gen && !triggered ) return; // -- early termination

  if( triggered ) { // -- if triggered: proceed to the event selection
    // -- not used; dummy
    int nVetoMuons=0;
    int nVetoElecs=0;

    std::vector<physics::lepton> muons = {}; // -- no need to collect muons (electron channel only)

    std::vector<physics::lepton> electrons = _electrons.get(weights().isdata(), *run,
                                                            genleps_dressed, genleps_fs,
                                                            rndm_forRoccor, nVetoElecs, 
                                                            "default", s, m);

    std::vector<physics::lepton> leptons = find_boson(muons, electrons);
    if( !leptons.empty() ) {
      double mass = (leptons[0].v + leptons[1].v).M();
      if( mass > 10.0 ) {
        // Rec boson found
        evt_systVar.rec = event_contents();
        evt_systVar.rec->leptons = leptons;
        evt_systVar.rec->boson_p = std::accumulate(
            leptons.begin(),
            leptons.end(),
            TLorentzVector(),
            [](const TLorentzVector &p, const physics::lepton &lep) { return p + lep.v; });

      } // -- if( mass > 10.0 )
    } // -- if( !leptons.empty() )
  } // -- if( triggered )

  if( !evt_systVar.gen && !evt_systVar.rec ) return; // -- early termination

  auto mass_systVar = evt_systVar.apply(&event_contents::get_boson_p).apply(&TLorentzVector::M);

  std::string str_systInfo = get_str_systInfo_muP(s, m); // -- same format with muP case
  util::matched<std::string> tags_systVar;
  if( evt_systVar.gen ) tags_systVar.gen = "inc0jet_elE_"+str_systInfo;
  else                  tags_systVar.gen = boost::none;
  if( evt_systVar.rec ) tags_systVar.rec = "inc0jet_elE_"+str_systInfo;
  else                  tags_systVar.rec = boost::none;

  fill_unfolded("mass_wide_range", tags_systVar, mass_systVar); // -- weight: default value
}

void dyjets_analyzer_syst::fill_systHist_elE_POGCorr(const util::matched<event_contents>& evt_default) {
  if( _weights.ismc() ) {
    fill_systHist_elE_POGCorr_eachSystVar(evt_default, "smearing_up");
    fill_systHist_elE_POGCorr_eachSystVar(evt_default, "smearing_down");
  }
  else { // -- data
    fill_systHist_elE_POGCorr_eachSystVar(evt_default, "scale_up");
    fill_systHist_elE_POGCorr_eachSystVar(evt_default, "scale_down");
  }
}

void dyjets_analyzer_syst::fill_systHist_elE_POGCorr_eachSystVar(const util::matched<event_contents>& evt_default, TString systMode) {
  // -- re-do the event selection
  // -- because the event itself can be rejected by the pT cut due to the electron energy variation

  // -- event for this systematic variation
  util::matched<event_contents> evt_systVar;

  // -- generator level: just copy the default value (no change with this systematic variation)
  if( evt_default.gen ) {
    evt_systVar.gen = event_contents();
    evt_systVar.gen->leptons = evt_default.gen->leptons;
    evt_systVar.gen->boson_p = evt_default.gen->boson_p;
  }

  // -- reco-level selection -- //
  // -- trigger
  bool triggered = passes_trigger();
  if( !evt_systVar.gen && !triggered ) return; // -- early termination

  if( triggered ) { // -- if triggered: proceed to the event selection
    // -- not used; dummy
    int nVetoMuons=0;
    int nVetoElecs=0;

    std::vector<physics::lepton> muons = {}; // -- no need to collect muons (electron channel only)

    // -- POG correction variation: the first 5 parameters are not used
    // ---- (isData, runNum, dressedLeptons, postFSRLeptons, randomValue)
    vector<physics::lepton> vec_empty;
    std::vector<physics::lepton> electrons = _electrons.get(0, 0, vec_empty, vec_empty, 0, nVetoElecs, systMode);

    std::vector<physics::lepton> leptons = find_boson(muons, electrons);
    if( !leptons.empty() ) {
      double mass = (leptons[0].v + leptons[1].v).M();
      if( mass > 10.0 ) {
        // Rec boson found
        evt_systVar.rec = event_contents();
        evt_systVar.rec->leptons = leptons;
        evt_systVar.rec->boson_p = std::accumulate(
            leptons.begin(),
            leptons.end(),
            TLorentzVector(),
            [](const TLorentzVector &p, const physics::lepton &lep) { return p + lep.v; });
      } // -- if( mass > 10.0 )
    } // -- if( !leptons.empty() )
  } // -- if( triggered )

  if( !evt_systVar.gen && !evt_systVar.rec ) return; // -- early termination

  auto mass_systVar = evt_systVar.apply(&event_contents::get_boson_p).apply(&TLorentzVector::M);

  std::string str_systMode = systMode.Data();
  util::matched<std::string> tags_systVar;
  if( evt_systVar.gen ) tags_systVar.gen = "inc0jet_elE_"+str_systMode;
  else                  tags_systVar.gen = boost::none;
  if( evt_systVar.rec ) tags_systVar.rec = "inc0jet_elE_"+str_systMode;
  else                  tags_systVar.rec = boost::none;

  fill_unfolded("mass_wide_range", tags_systVar, mass_systVar); // -- weight: default value
}


void dyjets_analyzer_syst::fill_systHist_muP(const util::matched<event_contents>& evt, 
                                             const bool isLowQMuEvent,
                                             const std::vector<physics::lepton>& genleps_fs,
                                             const double rndm_forRoccor) {
  // -- re-do event selection because muons can be rejected due to the pT cut
  // -- details on each variation: https://gitlab.cern.ch/akhukhun/roccor
  fill_systHist_muP_eachSystVar(evt, isLowQMuEvent, genleps_fs, rndm_forRoccor, 0, 0);
  for(int i_mem=0; i_mem<100; ++i_mem) // -- m = 1: stat. replicas
    fill_systHist_muP_eachSystVar(evt, isLowQMuEvent, genleps_fs, rndm_forRoccor, 1, i_mem);
  fill_systHist_muP_eachSystVar(evt, isLowQMuEvent, genleps_fs, rndm_forRoccor, 2, 0);
  fill_systHist_muP_eachSystVar(evt, isLowQMuEvent, genleps_fs, rndm_forRoccor, 3, 0);
  fill_systHist_muP_eachSystVar(evt, isLowQMuEvent, genleps_fs, rndm_forRoccor, 4, 0);
  fill_systHist_muP_eachSystVar(evt, isLowQMuEvent, genleps_fs, rndm_forRoccor, 5, 0); // -- our default value
}

void dyjets_analyzer_syst::fill_systHist_muP_eachSystVar(
                           const util::matched<event_contents>& evt_default, 
                           const bool isLowQMuEvent,
                           const std::vector<physics::lepton>& genleps_fs,
                           const double rndm_forRoccor,
                           const int s, const int m) {
  // -- event for this systematic variation
  util::matched<event_contents> evt_systVar;

  // -- generator level: just copy the default value (no change with this systematic variation)
  if( evt_default.gen ) {
    evt_systVar.gen = event_contents();
    evt_systVar.gen->leptons = evt_default.gen->leptons;
    evt_systVar.gen->boson_p = evt_default.gen->boson_p;
  }

  // -- reco-level: re-do the selection (muon only)
  // -- this part should be updated accordingly if the selection for the central value is updated
  if( passes_trigger() && !isLowQMuEvent ) {
    // -- isLowQMuEvent: not necessary to check here
    // -- as it is determined using raw_v; the low-quality event is already rejected
    std::vector<physics::lepton> muons = _muons.get(weights().isdata(), genleps_fs, rndm_forRoccor, s, m);
    std::vector<physics::lepton> electrons = {}; // -- dummy for find_boson
    std::vector<physics::lepton> leptons = find_boson(muons, electrons);
    if( !leptons.empty() ) {
      double mass = (leptons[0].v + leptons[1].v).M();
      if( mass > 10.0 ) {
        // Rec boson found
        evt_systVar.rec = event_contents();
        evt_systVar.rec->leptons = leptons;
        evt_systVar.rec->boson_p = std::accumulate(
          leptons.begin(),
          leptons.end(),
          TLorentzVector(),
          [](const TLorentzVector &p, const physics::lepton &lep) { return p + lep.v; } );
      } // -- if( mass > 10.0 )
    } // -- if( !leptons.empty() )
  } // -- if( passes_trigger() && !isLowQMuEvent )

  if( !evt_systVar.gen && !evt_systVar.rec ) return; // -- early termination

  std::string str_systInfo = get_str_systInfo_muP(s, m);

  auto mass_systVar = evt_systVar.apply(&event_contents::get_boson_p).apply(&TLorentzVector::M);

  util::matched<std::string> tags_systVar;
  if( evt_systVar.gen ) tags_systVar.gen = "inc0jet_muP_"+str_systInfo;
  else                  tags_systVar.gen = boost::none;
  if( evt_systVar.rec ) tags_systVar.rec = "inc0jet_muP_"+str_systInfo;
  else                  tags_systVar.rec = boost::none;

  fill_unfolded("mass_wide_range", tags_systVar, mass_systVar); // -- weight: default value
}

std::string dyjets_analyzer_syst::get_str_systInfo_muP(const int& s, const int& m) {
  TString tstr_systInfo = "";

  if( s == 1 )
    tstr_systInfo = TString::Format("set%d_%03d", s, m);
  else
    tstr_systInfo = TString::Format("set%d", s);

  return tstr_systInfo.Data();
}

void dyjets_analyzer_syst::fill_systHist_L1Pref(const util::matched<event_contents>& evt,
                                                const util::matched<double> &value,
                                                const util::matched<std::string>& tags_default) {

  // -- cv = central value
  double global_weight_cv = weights().global_weight();
  double global_weight_plus  = global_weight_cv * ( (*L1PreFiringWeight_Up) / (*L1PreFiringWeight_Nom) );
  double global_weight_minus = global_weight_cv * ( (*L1PreFiringWeight_Dn) / (*L1PreFiringWeight_Nom) );

  util::matched<std::string> tags_plus;
  if( evt.gen ) tags_plus.gen = *tags_default.gen + "_L1Pref_plus";
  else          tags_plus.gen = boost::none;
  if( evt.rec ) tags_plus.rec = *tags_default.rec + "_L1Pref_plus";
  else          tags_plus.rec = boost::none;
  fill_unfolded("mass_wide_range", tags_plus, value, weights().gen_weight(), global_weight_plus);

  util::matched<std::string> tags_minus;
  if( evt.gen ) tags_minus.gen = *tags_default.gen + "_L1Pref_minus";
  else          tags_minus.gen = boost::none;
  if( evt.rec ) tags_minus.rec = *tags_default.rec + "_L1Pref_minus";
  else          tags_minus.rec = boost::none;
  fill_unfolded("mass_wide_range", tags_minus, value, weights().gen_weight(), global_weight_minus);
}

void dyjets_analyzer_syst::fill_systHist_pileup(const util::matched<event_contents>& evt,
                                                const util::matched<double> &value,
                                                const util::matched<std::string>& tags_default) {

  // -- cv = central value
  double global_weight_cv = weights().global_weight();
  double global_weight_plus  = global_weight_cv * (_pileup.weight(1)/_pileup.weight(0));
  double global_weight_minus = global_weight_cv * (_pileup.weight(-1)/_pileup.weight(0));

  util::matched<std::string> tags_plus;
  if( evt.gen ) tags_plus.gen = *tags_default.gen + "_pileup_plus";
  else          tags_plus.gen = boost::none;
  if( evt.rec ) tags_plus.rec = *tags_default.rec + "_pileup_plus";
  else          tags_plus.rec = boost::none;
  fill_unfolded("mass_wide_range", tags_plus, value, weights().gen_weight(), global_weight_plus);

  util::matched<std::string> tags_minus;
  if( evt.gen ) tags_minus.gen = *tags_default.gen + "_pileup_minus";
  else          tags_minus.gen = boost::none;
  if( evt.rec ) tags_minus.rec = *tags_default.rec + "_pileup_minus";
  else          tags_minus.rec = boost::none;
  fill_unfolded("mass_wide_range", tags_minus, value, weights().gen_weight(), global_weight_minus);
}

void dyjets_analyzer_syst::fill_systHist_effSF(const util::matched<event_contents>& evt,
                                               const util::matched<double> &value,
                                               const util::matched<std::string>& tags_default) {
  std::vector<physics::lepton> chosen_leptons;
  if( evt.rec ) {
    if( _channel == "ee" ) {
      std::copy_if(evt.rec->leptons.begin(),
                   evt.rec->leptons.end(),
                   std::back_inserter(chosen_leptons),
                   [](const physics::lepton &lep) { return lep.pdgid == 11; });
    }
    else if( _channel == "mm" ) {
      std::copy_if(evt.rec->leptons.begin(),
                   evt.rec->leptons.end(),
                   std::back_inserter(chosen_leptons),
                   [](const physics::lepton &lep) { return lep.pdgid == 13; });
    }
  }
  std::map<TString, double> map_uncType_effSFRatio;
  // -- effSFRatio: SF(syst.variation) / SF(central value) (i.e. SF change due to each systematic variation)
  // -- if chosen_leptons.empty() (i.e. no reco): effSFRatio = 1.0 for all uncType
  calc_effSFRatio_systVariation(chosen_leptons, map_uncType_effSFRatio);

  // -- fill the histograms
  // -- if reco'd event: fill histograms with each systematic varied weights
  // -- if not reco'd event: fill gen-histograms anyway
  for(const auto& pair : map_uncType_effSFRatio ) {
    TString uncType = pair.first;
    double effSFRatio = pair.second;
    double global_weight_systVar = weights().global_weight() * effSFRatio;

    util::matched<std::string> tags;
    if( evt.gen ) tags.gen = *tags_default.gen + "_" + uncType.Data();
    else          tags.gen = boost::none;
    if( evt.rec ) tags.rec = *tags_default.rec + "_" + uncType.Data();
    else          tags.rec = boost::none;

    fill_unfolded("mass_wide_range", tags, value, weights().gen_weight(), global_weight_systVar);
  }
}

void dyjets_analyzer_syst::calc_effSFRatio_systVariation(const vector<physics::lepton>& chosen_leptons, 
                                                         std::map<TString, double>& map_uncType_effSFRatio) {

  // -- maybe std::unordered_map improves the performance
  // -- but the expected # elements is < ~ 10, which means search time for the std::map is log(N) < ~ 1
  // -- i.e. just std::map should be also fine
  std::map<TString, double> map_type_effSFCV;
  for(const auto& effMap : _vec_effMap ) {

    TString type = TString::Format("%s-%s", effMap.effType().Data(), effMap.dataType().Data() );
    double effSFRatio;
    if( chosen_leptons.empty() )  // -- no reco.
      effSFRatio = 1.0;
    else {
      double effSF_cv = find_or_calculate_centralValueEffSF(type, chosen_leptons, map_type_effSFCV);
      double effSF_systVar = get_effSF(chosen_leptons, effMap);
      effSFRatio = effSF_systVar / effSF_cv;
    }

    TString uncType = TString::Format("%s-%s-%s", effMap.effType().Data(), effMap.dataType().Data(), effMap.systType().Data());
    map_uncType_effSFRatio.insert( std::make_pair(uncType, effSFRatio) );
  }
}

double dyjets_analyzer_syst::find_or_calculate_centralValueEffSF(const TString& type, 
                                                            const vector<physics::lepton>& chosen_leptons,
                                                            std::map<TString, double>& map_type_effSFCV) {
  double effSF_cv = 1.0;

  auto it_pair = map_type_effSFCV.find(type);
  // -- to reduce the calculation time: calculate the cv SF *only* when it was not calculated before
  if( it_pair == map_type_effSFCV.end() ) {
    const EffMap& effMap_cv = _map_type_effMapCV[type];
    effSF_cv = get_effSF(chosen_leptons, effMap_cv);
    map_type_effSFCV.insert( std::make_pair(type, effSF_cv) );
  }
  else { // -- already exist in the map
    effSF_cv = it_pair->second; // -- do not recalculate; use the existing value
  }

  return effSF_cv;
}

double dyjets_analyzer_syst::get_effSF(const vector<physics::lepton>& chosen_leptons, const EffMap& effMap) {
  double effSF = 1.0;

  TString effType = effMap.effType();

  if( _channel == "mm" ) {

    if( effType == "STRIG" ) {
      if( !_use_smu_triggerSF ) effSF = 1.0;
      else                      effSF = get_effSF_fromEff_mu(chosen_leptons, effMap);
    }
    else if( effType == "DTRIG" ) {
      if( _use_smu_triggerSF ) effSF = 1.0;
      else {
        double abseta_lead, abseta_sub;
        get_abseta(chosen_leptons, abseta_lead, abseta_sub);
        int i_binX = effMap.GetBinNumber( "X", abseta_lead );
        int i_binY = effMap.GetBinNumber( "Y", abseta_sub );
        effSF = effMap.map()->GetBinContent(i_binX, i_binY);
      }
    }
    else // -- ID or ISO
      effSF = get_effSF_fromEff_mu(chosen_leptons, effMap);

  }
  else if( _channel == "ee" ) { // -- all maps: SF, not eff.

    double pt_lep1 = chosen_leptons[0].v.Pt();
    double eta_lep1 = chosen_leptons[0].raw_v.Eta(); // -- etaSC, keep sign
    double pt_lep2 = chosen_leptons[1].v.Pt();
    double eta_lep2 = chosen_leptons[1].raw_v.Eta(); // -- etaSC, keep sign

    int i_binX_lep1 = effMap.GetBinNumber("X", eta_lep1);
    int i_binY_lep1 = effMap.GetBinNumber("Y", pt_lep1);
    int i_binX_lep2 = effMap.GetBinNumber("X", eta_lep2);
    int i_binY_lep2 = effMap.GetBinNumber("Y", pt_lep2);

    double sf_lep1 = effMap.map()->GetBinContent(i_binX_lep1, i_binY_lep1);
    double sf_lep2 = effMap.map()->GetBinContent(i_binX_lep2, i_binY_lep2);

    if( effMap.effType() == "TRIG_Leg1" )      effSF = sf_lep1 * 1.0;
    else if( effMap.effType() == "TRIG_Leg2" ) effSF = 1.0 * sf_lep2;
    else                                       effSF = sf_lep1*sf_lep2; // -- RECO or ID
  }

  return effSF;
}

void dyjets_analyzer_syst::get_abseta(const vector<physics::lepton>& chosen_leptons, double& abseta_lead, double& abseta_sub) {
  if( chosen_leptons[0].raw_v.Pt() > chosen_leptons[1].raw_v.Pt() ) {
    abseta_lead = std::abs(chosen_leptons[0].raw_v.Eta());
    abseta_sub  = std::abs(chosen_leptons[1].raw_v.Eta());
  }
  else {
    abseta_lead = std::abs(chosen_leptons[1].raw_v.Eta());
    abseta_sub  = std::abs(chosen_leptons[0].raw_v.Eta());
  }
}

double dyjets_analyzer_syst::get_effSF_fromEff_mu(const vector<physics::lepton>& chosen_leptons, const EffMap& effMap) {

  double pt_lep1 = chosen_leptons[0].raw_v.Pt();
  double eta_lep1 = chosen_leptons[0].raw_v.Eta();
  double pt_lep2 = chosen_leptons[1].raw_v.Pt();
  double eta_lep2 = chosen_leptons[1].raw_v.Eta();

  // -- all muon eff. map: use |eta|
  eta_lep1 = std::abs(eta_lep1);
  eta_lep2 = std::abs(eta_lep2);

  int i_binX_lep1 = effMap.GetBinNumber("X", eta_lep1);
  int i_binY_lep1 = effMap.GetBinNumber("Y", pt_lep1);
  int i_binX_lep2 = effMap.GetBinNumber("X", eta_lep2);
  int i_binY_lep2 = effMap.GetBinNumber("Y", pt_lep2);

  double eff_lep1_dataType1 = effMap.map()->GetBinContent(i_binX_lep1, i_binY_lep1);
  double eff_lep2_dataType1 = effMap.map()->GetBinContent(i_binX_lep2, i_binY_lep2);

  double eff_lep1_dataType2 = effMap.map_counter_cv()->GetBinContent(i_binX_lep1, i_binY_lep1);
  double eff_lep2_dataType2 = effMap.map_counter_cv()->GetBinContent(i_binX_lep2, i_binY_lep2);

  double ratio = 1.0; // -- dataType1 / dataType2 (= effSF if dataType1 == "data")
  if( effMap.effType() == "STRIG" ) {
    if( pt_lep1 < _sMuTrigPtCut ) {
      eff_lep1_dataType1 = 0.0;
      eff_lep1_dataType2 = 0.0;
    }
    if( pt_lep2 < _sMuTrigPtCut ) {
      eff_lep2_dataType1 = 0.0;
      eff_lep2_dataType2 = 0.0;
    }

    double eff_dilep_dataType1 = 1.0 - (1.0 - eff_lep1_dataType1)*(1.0 - eff_lep2_dataType1);
    double eff_dilep_dataType2 = 1.0 - (1.0 - eff_lep1_dataType2)*(1.0 - eff_lep2_dataType2);
    ratio = eff_dilep_dataType1 / eff_dilep_dataType2;
  }
  else { // -- ID or ISO

    double ratio_lep1 = (eff_lep1_dataType1/eff_lep1_dataType2);
    double ratio_lep2 = (eff_lep2_dataType1/eff_lep2_dataType2);
    ratio = ratio_lep1 * ratio_lep2;
  }

  return effMap.dataType() == "data" ? ratio : (1.0/ratio);
}

void dyjets_analyzer_syst::init_effMap() {
  TString fileName = _fileName_effMap;
  util::logging::info << "Input file for the systematic efficiency maps: " + fileName << std::endl;

  TFile* f_input = !gSystem->AccessPathName(fileName) ? TFile::Open(fileName) : nullptr;
  if( f_input == nullptr )
    throw std::invalid_argument("File " + fileName + " under " + gSystem->GetWorkingDirectory() + " doesn't exist");
  TH1::AddDirectory(false); // to keep histograms even after closing the input file

  TString tag_era;
  if( get_era() == 0 ) tag_era = "16pre";
  if( get_era() == 1 ) tag_era = "16post";
  if( get_era() == 2 ) tag_era = "17";
  if( get_era() == 3 ) tag_era = "18";

  TString tag_lepType;
  if( _channel == "mm" ) tag_lepType = "mu";
  if( _channel == "ee" ) tag_lepType = "el";

  // -- loop over histograms in the .root file
  TIter list_key(f_input->GetListOfKeys());
  TKey *key;
  while( (key = (TKey*)list_key()) ) {
    // TClass *cl = gROOT->GetClass(key->GetClassName());
    // if (!cl->InheritsFrom("TH2")) continue;
    TString className = key->GetClassName();
    if( !className.Contains("TH2") ) continue;

    TString histName = ( (TH1D*)(key->ReadObj()) )->GetName();
    if( !histName.Contains("absUnc") ) {
      EffMap effMap(fileName, histName);

      if( effMap.era() != tag_era ) continue;
      if( effMap.lepType() != tag_lepType ) continue;

      _vec_effMap.push_back( effMap );

      if( effMap.systType() == "cv" ) {
        TString type = TString::Format("%s-%s", effMap.effType().Data(), effMap.dataType().Data() );
        _map_type_effMapCV.insert( std::make_pair(type, effMap) );
      } // -- if nominal
    }
  } // -- end of while
}

