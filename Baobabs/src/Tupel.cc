/* -*- c-basic-offset: 2; -*-

   Code by: Bugra Bilin, Kittikul Kovitanggoon, Tomislav Seva, Efe Yazgan,
   Philippe Gras...

*/

#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <TLorentzVector.h>
#include <stdlib.h>
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "FWCore/Utilities/interface/TimeOfDay.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Version/interface/GetReleaseVersion.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "EgammaAnalysis/ElectronTools/interface/EGammaCutBasedEleId.h"
#include "EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/JetReco/interface/PileupJetIdentifier.h"
#include "FWCore/Framework/interface/GenericHandle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
//#include "CMGTools/External/interface/PileupJetIdentifier.h"
#include "EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/BTauReco/interface/JetTag.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "CondFormats/DataRecord/interface/HBHENegativeEFilterRcd.h"

#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

typedef std::vector<reco::GenParticle> GenParticleCollection;
using reco::GenParticleCollection;
using edm::Handle;
using edm::View;
using edm::InputTag;


#include "TreeHelper.h"

const double pi = 4*atan(1.);

#define QUOTE2(a) #a
#define QUOTE(a) QUOTE2(a)
const static char* checksum = QUOTE(MYFILE_CHECKSUM);

/**
 * Define a bit of a bit field.
 * 1. Document the bit in the ROOT tree
 * 2. Set the variable with the bit mask (integer with the relevant bit set).
 * The second version DEF_BIT2 omits this setting. It can be useful when we
 * want to avoid defining the variable.
 * 3. Set a map to match bit name to bit mask
 * A field named after label prefixed with a k and with an undercore
 * appended to it must be defined in the class members. This macro will set it.
 * @param bitField: name (verbatim string without quotes) of the event tree branch the bit belongs to
 * @param bitNum: bit position, LSB is 0, MSB is 31
 * @param label: used to build the class field names (k<label>_ for the mask
 * and <label>Map_ for the name->mask map). The label is stored as
 * description in the ROOT file. Use a verbatim string without quotes.
 *
 *  For long description including spaces please use the DEF_BIT_L version
 */
#define DEF_BIT(bitField, bitNum, label)		\
  k ## label ## _ = 1 <<bitNum;				\
  bitField ## Map ## _[#label] = 1LL <<bitNum;		\
  treeHelper_->defineBit(#bitField, bitNum, #label);

#define DEF_BIT2(bitField, bitNum, label)		\
  bitField ## Map ## _[#label] = 1LL <<bitNum;		\
  treeHelper_->defineBit(#bitField, bitNum, #label);

/**
 * See DEF_BIT. Version for long description which can not be used
 * as variable name. The argument desc must be a c string including
 * the quotes or a c-string (char*) type variable.
 */
#define DEF_BIT_L(bitField, bitNum, label, desc)	\
  k ## label ## _ = 1 <<bitNum;				\
  bitField ## Map ## _[#label] = 1 <<bitNum;		\
  treeHelper_->defineBit(#bitField, bitNum, desc);

#define DEF_BIT2_L(bitField, bitNum, label, desc)	\
  bitField ## Map ## _[#label] = 1 <<bitNum;		\
  treeHelper_->defineBit(#bitField, bitNum, desc);


//#name -> "name", name ## _ -> name_
#define ADD_BRANCH_D(name, desc) treeHelper_->addBranch(#name, name ## _, desc)
#define ADD_BRANCH(name) treeHelper_->addBranch(#name, name ## _)
#define ADD_MOMENTUM_BRANCH_D(name, desc) treeHelper_->addMomentumBranch(#name, name ## Pt_, name ## Eta_, name ## Phi_, name ## E_, desc)
#define ADD_MOMENTUM_BRANCH(name) treeHelper_->addMomentumBranch(#name, name ## Pt_, name ## Eta_, name ## Phi_, name ## E_)


class TTree;
class Tupel : public edm::EDAnalyzer {

public:
  /// default constructor
  explicit Tupel(const edm::ParameterSet&);
  /// default destructor
  ~Tupel();

private:
  /// everything that needs to be done before the event loop
  virtual void beginJob() ;
  /// everything that needs to be done during the event loop
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  /// everything that needs to be done after the event loop

  virtual void endJob();

  virtual void endRun(edm::Run const& iRun, edm::EventSetup const&);

  void allocateTrigMap(int ntrigMax);

  void defineBitFields();

  //help function to compute pseudo rapidity of a TLorentz without
  //ROOT exception in case of infinite pseudo rapidity
  double eta(TLorentzVector v){
    if(fabs(v.CosTheta()) < 1) return -0.5 * log((1.0-v.CosTheta())/(1.0+v.CosTheta()));
    else if(v.Pz() > 0) return 10e10;
    else return -10e10;
  }

  //help function to compute DR between two TLorentz vectors without
  //ROOT exception in case of infinite pseudo rapidity
  double deltar(TLorentzVector v1, TLorentzVector v2){
    return sqrt(pow(v1.DeltaPhi(v2), 2) + pow(eta(v1)-eta(v2), 2));
  }

  
  //Write Job information tree in the output. The tree
  //contains one event, with general information
  void writeHeader();

  //Retrieves event objets
  void readEvent(const edm::Event& iEvent);

  //Processes MET
  void processMET(const edm::Event& iEvent);

  //Processes vertices
  void processVtx();

  //Processes PU
  void processPu(const edm::Event& iEvent);

  //Processes GenParticles
  void processGenParticles(const edm::Event& iEvent);

  void processGenJets(const edm::Event& iEvent);

  //Modified by Clement Leloup
  void processGenFatJets(const edm::Event& iEvent);

  void processPdfInfo(const edm::Event& iEvent);

  void processTrigger(const edm::Event& iEvent);
    
  void processMETFilter(const edm::Event& iEvent);

  void processMuons();

  void processElectrons();

  // Modified by Clement Leloup
  void processTaus();

  void processJets();

  //Modified by Clement Leloup
  void processFatJets();

  void processPhotons(const edm::Event& iEvent, const edm::EventSetup& iSetup);

  void processPfCands();

  /** Check if the argument is one of the
   * HLT trigger considered to be stored and if it is set
   * the corresponding bit.
   */
  void fillTrig(const std::string& trigname);

  void writeTriggerStat();

  /** Checks trigger name, ignoring the version number.
   * @param triggerBasename trigger base name without the version number
   * @param triggerName with possibly the _vNNN version suffix
   * @return true iff triggerName is identical to triggerBasename once the
   * version suffix discarded
   */
  bool compTrigger(const char* triggerBasename, const char* triggerName) const;
  
  // input tags
  std::string elecMatch_;
  std::string muonMatch_;
  std::string muonMatch2_;

  edm::EDGetTokenT<std::vector<pat::Electron> > elecToken_;

  edm::EDGetTokenT<std::vector<pat::Muon> > muonToken_;

  // Modified by Clement Leloup
  edm::EDGetTokenT<std::vector<pat::Tau> > tauToken_;  

  edm::EDGetTokenT<reco::ConversionCollection> conversionsToken_;

  edm::EDGetTokenT<edm::View<pat::Jet> > jetToken_;

  //Modified by Clement Leloup
  std::string fatJetSw_;

  //Modified by Clement Leloup
  edm::EDGetTokenT<edm::View<pat::Jet> > fatJetToken_;

  std::string photonSw_;
  edm::EDGetTokenT<std::vector<pat::Photon> > photonToken_;

  std::string candidateSw_;
  edm::EDGetTokenT<std::vector<pat::PackedCandidate> > candidateToken_; 

  std::string recoSw_;

  edm::EDGetTokenT<std::vector<reco::Vertex> > vertexToken_;

  edm::EDGetTokenT<double> mSrcRhoToken_;

  edm::EDGetTokenT<reco::BeamSpot> beamSpotToken_;

  std::vector<edm::EDGetTokenT<edm::View<pat::MET> >  > metSrcsToken;

  edm::EDGetTokenT<edm::TriggerResults> HLTTagToken_;

  edm::EDGetTokenT<edm::TriggerResults> metFilTagToken_;
  edm::EDGetTokenT<edm::TriggerResults> metFilTagRECOToken_;
	
  //edm::EDGetTokenT<edm::View<pat::MET> > newMetToken_;
  //edm::EDGetTokenT<edm::View<pat::MET> > new2MetToken_;

  edm::EDGetTokenT<std::vector<reco::GenParticle> > genParticleToken_;

  edm::EDGetTokenT<std::vector<reco::GenJet> > gjetToken_;

  //Modified by Clement Leloup
  edm::EDGetTokenT<std::vector<reco::GenJet> > gfatJetToken_;

  edm::EDGetTokenT<std::vector<PileupSummaryInfo> > puToken_;

  edm::EDGetTokenT<GenEventInfoProduct> generatorToken_;

  edm::EDGetTokenT<LHEEventProduct> lheEventToken_;
  edm::EDGetTokenT<LHERunInfoProduct> lheRunToken_;

  edm::EDGetTokenT<edm::ValueMap<StoredPileupJetIdentifier> > puJetIdsToken_;
  const edm::ValueMap<StoredPileupJetIdentifier>*  puJetIds_;
  std::string puMvaName_;

  bool triggerStat_;


  //for EcalClusterLazyTools
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitEBToken_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitEEToken_;
  edm::EDGetTokenT<EcalRecHitCollection> ecalHitESToken_;


  /** Selection of list of trigger to copy
   * in the ntuple
   */
  std::string triggerMenu_;

  /** Total number of events analyzed so far
   */
  Long64_t analyzedEventCnt_;

  bool photonIdsListed_;
  bool elecIdsListed_;
  bool hltListed_;

  //edm::EDGetTokenT<edm::ValueMap<float> > full5x5SigmaIEtaIEtaMapToken_;

  // ----------member data ---------------------------
  TTree *myTree;
  std::auto_ptr<TreeHelper> treeHelper_;

  //Event
  std::auto_ptr<int>      EvtIsRealData_;
  std::auto_ptr<unsigned> EvtNum_;
  std::auto_ptr<unsigned> EvtRunNum_;
  std::auto_ptr<int> 	  EvtLumiNum_;
  std::auto_ptr<int> 	  EvtBxNum_;
  std::auto_ptr<int> 	  EvtVtxCnt_;
  std::auto_ptr<int> 	  EvtPuCnt_;
  std::auto_ptr<int> 	  EvtPuCntTruth_;
  std::auto_ptr<std::vector<double> > EvtWeights_;
  std::auto_ptr<float>    EvtFastJetRho_;
    
  // MET Filter
  std::auto_ptr<ULong64_t>         TrigMET_;

  //Trigger
  std::auto_ptr<unsigned>          TrigHlt_;
  std::map<std::string, unsigned>  TrigHltMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltPhot_;
  std::map<std::string, ULong64_t> TrigHltPhotMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltDiPhot_;
  std::map<std::string, ULong64_t> TrigHltDiPhotMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltMu_;
  std::map<std::string, ULong64_t> TrigHltMuMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltDiMu_;
  std::map<std::string, ULong64_t> TrigHltDiMuMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltEl_;
  std::map<std::string, ULong64_t> TrigHltElMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltDiEl_;
  std::map<std::string, ULong64_t> TrigHltDiElMap_; //bit assignment
  std::auto_ptr<ULong64_t>         TrigHltElMu_;
  std::map<std::string, ULong64_t> TrigHltElMuMap_; //bit assignment
  struct  TrigHltMapRcd {
    TrigHltMapRcd(): pMap(0), pTrig(0) {}
    TrigHltMapRcd(std::map<std::string, ULong64_t>* pMap_, ULong64_t* pTrig_): pMap(pMap_), pTrig(pTrig_) {
      assert(pTrig_);
    }
    //    TrigHltMapRcd(const TrigHltMapRcd& a){ this->pMap = a.pMap; this->pTrig = a.pTrig; }
    std::map<std::string, ULong64_t>* pMap;
    ULong64_t* pTrig;
  };
  std::vector<TrigHltMapRcd> trigHltMapList_; //list of trigger maps.

  //Missing energy
  std::auto_ptr<std::vector<float> > METPt_;
  std::auto_ptr<std::vector<float> > METPx_;
  std::auto_ptr<std::vector<float> > METPy_;
  std::auto_ptr<std::vector<float> > METPz_;
  std::auto_ptr<std::vector<float> > METE_;
  std::auto_ptr<std::vector<float> > GMETPt_;
  std::auto_ptr<std::vector<float> > GMETPx_;
  std::auto_ptr<std::vector<float> > GMETPy_;
  std::auto_ptr<std::vector<float> > GMETPz_;
  std::auto_ptr<std::vector<float> > GMETE_;
  std::auto_ptr<std::vector<float> > METsigx2_;
  std::auto_ptr<std::vector<float> > METsigxy_;
  std::auto_ptr<std::vector<float> > METsigy2_;
  std::auto_ptr<std::vector<float> > METsig_;

  //Generator level leptons, dressed
  std::auto_ptr<std::vector<float> > 	GLepDr01Pt_;
  std::auto_ptr<std::vector<float> > 	GLepDr01Eta_;
  std::auto_ptr<std::vector<float> > 	GLepDr01Phi_;
  std::auto_ptr<std::vector<float> > 	GLepDr01E_;
  std::auto_ptr<std::vector<int> >      GLepDr01Id_;
  std::auto_ptr<std::vector<int> >      GLepDr01St_;
  std::auto_ptr<std::vector<int> >      GLepDr01MomId_;
  std::auto_ptr<std::vector<bool> >     GLepDr01Prompt_;
  std::auto_ptr<std::vector<bool> >     GLepDr01TauProd_;

  //Generator level leptons, not-dressed
  std::auto_ptr<std::vector<float> > 	GLepBarePt_;
  std::auto_ptr<std::vector<float> > 	GLepBareEta_;
  std::auto_ptr<std::vector<float> > 	GLepBarePhi_;
  std::auto_ptr<std::vector<float> > 	GLepBareE_;
  std::auto_ptr<std::vector<int> >      GLepBareId_;
  std::auto_ptr<std::vector<int> > 	GLepBareSt_;
  std::auto_ptr<std::vector<int> > 	GLepBareMomId_;
  std::auto_ptr<std::vector<bool> >     GLepBarePrompt_;
  std::auto_ptr<std::vector<bool> >     GLepBareTauProd_;
  
  //Generator level leptons, status 3
  std::auto_ptr<std::vector<float> > GLepSt3Pt_;
  std::auto_ptr<std::vector<float> > GLepSt3Eta_;
  std::auto_ptr<std::vector<float> > GLepSt3Phi_;
  std::auto_ptr<std::vector<float> > GLepSt3E_;
  std::auto_ptr<std::vector<float> > GLepSt3M_;
  std::auto_ptr<std::vector<int> >   GLepSt3Id_;
  std::auto_ptr<std::vector<int> >   GLepSt3St_;
  std::auto_ptr<std::vector<int> >   GLepSt3Mother0Id_;
  std::auto_ptr<std::vector<int> >   GLepSt3MotherCnt_;

  //Generator level photons
  std::auto_ptr<std::vector<float> > GPhotPt_;
  std::auto_ptr<std::vector<float> > GPhotEta_;
  std::auto_ptr<std::vector<float> > GPhotPhi_;
  std::auto_ptr<std::vector<float> > GPhotE_;
  std::auto_ptr<std::vector<int> >   GPhotMotherId_;
  std::auto_ptr<std::vector<int> >   GPhotSt_;
  std::auto_ptr<std::vector<float> > GPhotIsoEDR03_;
  std::auto_ptr<std::vector<float> > GPhotIsoEDR04_;
  std::auto_ptr<std::vector<float> > GPhotIsoEDR05_;
  std::auto_ptr<std::vector<float> > GPhotIsoSumPtDR03_;
  std::auto_ptr<std::vector<float> > GPhotIsoSumPtDR04_;
  std::auto_ptr<std::vector<float> > GPhotIsoSumPtDR05_;

  //Photons in the vicinity of the leptons
  std::auto_ptr<std::vector<float> > GLepClosePhotPt_;
  std::auto_ptr<std::vector<float> > GLepClosePhotEta_;
  std::auto_ptr<std::vector<float> > GLepClosePhotPhi_;
  std::auto_ptr<std::vector<float> > GLepClosePhotE_;
  std::auto_ptr<std::vector<float> > GLepClosePhotM_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotId_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotMother0Id_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotMotherCnt_;
  std::auto_ptr<std::vector<int> >   GLepClosePhotSt_;

  //Gen Jets
  std::auto_ptr<std::vector<float> > GJetAk04Pt_;
  std::auto_ptr<std::vector<float> > GJetAk04Eta_;
  std::auto_ptr<std::vector<float> > GJetAk04Phi_;
  std::auto_ptr<std::vector<float> > GJetAk04E_;
  std::auto_ptr<std::vector<float> > GJetAk04Id_;
  std::auto_ptr<std::vector<float> > GJetAk04PuId_;
  std::auto_ptr<std::vector<float> > GJetAk04ChFrac_;
  std::auto_ptr<std::vector<int> >   GJetAk04ConstCnt_;
  std::auto_ptr<std::vector<int> >   GJetAk04ConstId_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstPt_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstEta_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstPhi_;
  std::auto_ptr<std::vector<float> > GJetAk04ConstE_;
  std::auto_ptr<std::vector<float> > GJetAk04MatchedPartonID_;
  std::auto_ptr<std::vector<float> > GJetAk04MatchedPartonDR_;

  //Modified by Clement Leloup
  //Gen Fat Jets
  std::auto_ptr<std::vector<float> > GJetAk08Pt_;
  std::auto_ptr<std::vector<float> > GJetAk08Eta_;
  std::auto_ptr<std::vector<float> > GJetAk08Phi_;
  std::auto_ptr<std::vector<float> > GJetAk08E_;
  std::auto_ptr<std::vector<float> > GJetAk08Id_;
  std::auto_ptr<std::vector<float> > GJetAk08PuId_;
  std::auto_ptr<std::vector<float> > GJetAk08ChFrac_;
  std::auto_ptr<std::vector<int> >   GJetAk08ConstCnt_;
  std::auto_ptr<std::vector<int> >   GJetAk08ConstId_;
  std::auto_ptr<std::vector<float> > GJetAk08ConstPt_;
  std::auto_ptr<std::vector<float> > GJetAk08ConstEta_;
  std::auto_ptr<std::vector<float> > GJetAk08ConstPhi_;
  std::auto_ptr<std::vector<float> > GJetAk08ConstE_;
  std::auto_ptr<std::vector<float> > GJetAk08MatchedPartonID_;
  std::auto_ptr<std::vector<float> > GJetAk08MatchedPartonDR_;

  //Exta generator information
  std::auto_ptr<std::vector<int> >   GPdfId1_;
  std::auto_ptr<std::vector<int> >   GPdfId2_;
  std::auto_ptr<std::vector<float> > GPdfx1_;
  std::auto_ptr<std::vector<float> > GPdfx2_;
  std::auto_ptr<std::vector<float> > GPdfScale_;
  std::auto_ptr<float>               GBinningValue_;
  std::auto_ptr<int>                 GNup_;


  ///Muons
  std::auto_ptr<std::vector<float> > 	MuPt_;
  std::auto_ptr<std::vector<float> > 	MuEta_;
  std::auto_ptr<std::vector<float> > 	MuPhi_;
  std::auto_ptr<std::vector<float> > 	MuE_;
  std::auto_ptr<std::vector<unsigned> > MuId_;
  std::map<std::string, unsigned>       MuIdMap_; //bit assignment
  std::auto_ptr<std::vector<unsigned> > MuIdTight_;
  std::map<std::string, unsigned>    	MuIdTightMap_; //bit assignment
  std::auto_ptr<std::vector<float> > 	MuCh_;
  std::auto_ptr<std::vector<float> > 	MuVtxZ_;
  std::auto_ptr<std::vector<float> > 	MuDxy_;
  std::auto_ptr<std::vector<float> > 	MuIsoRho_;
  std::auto_ptr<std::vector<float> > 	MuPfIso_;
  std::auto_ptr<std::vector<float> > 	MuType_;
  std::map<std::string, unsigned>    	MuTypeMap_; //bit assignment
  std::auto_ptr<std::vector<float> > 	MuIsoTkIsoAbs_;
  std::auto_ptr<std::vector<float> > 	MuIsoTkIsoRel_;
  std::auto_ptr<std::vector<float> > 	MuIsoCalAbs_;
  std::auto_ptr<std::vector<float> > 	MuIsoCombRel_;
  std::auto_ptr<std::vector<float> > 	MuTkNormChi2_;
  std::auto_ptr<std::vector<int> > 	MuTkHitCnt_;
  std::auto_ptr<std::vector<int> > 	MuMatchedStationCnt_;
  std::auto_ptr<std::vector<float> > 	MuDz_;
  std::auto_ptr<std::vector<int> > 	MuPixelHitCnt_;
  std::auto_ptr<std::vector<int> > 	MuTkLayerCnt_;
  std::auto_ptr<std::vector<float> > 	MuPfIsoChHad_;
  std::auto_ptr<std::vector<float> > 	MuPfIsoNeutralHad_;
  std::auto_ptr<std::vector<float> > 	MuPfIsoRawRel_;
  std::auto_ptr<std::vector<unsigned> > MuHltMatch_;

  //Electrons
  std::auto_ptr<std::vector<float> > 	ElPt_;
  std::auto_ptr<std::vector<float> > 	ElEta_;
  std::auto_ptr<std::vector<float> > 	ElEtaSc_;
  std::auto_ptr<std::vector<float> > 	ElPhi_;
  std::auto_ptr<std::vector<float> > 	ElE_;
  std::auto_ptr<std::vector<float> >    ElScRawE_;
  std::auto_ptr<std::vector<float> >    ElCorrE_;
  std::auto_ptr<std::vector<float> > 	ElCh_;
  std::auto_ptr<std::vector<float> >    ElEcalIso_;
  std::auto_ptr<std::vector<float> >    ElEcalPfIso_;
  std::auto_ptr<std::vector<unsigned> > ElId_;
  std::map<std::string, unsigned>    	ElIdMap_; //bit assignment
  std::auto_ptr<std::vector<float> > 	ElMvaTrig_;
  std::auto_ptr<std::vector<float> > 	ElMvaNonTrig_;
  std::auto_ptr<std::vector<float> > 	ElMvaPresel_;
  std::auto_ptr<std::vector<float> > 	ElDEtaTkScAtVtx_;
  std::auto_ptr<std::vector<float> > 	ElDPhiTkScAtVtx_;
  std::auto_ptr<std::vector<float> > 	ElHoE_;
  std::auto_ptr<std::vector<float> > 	ElSigmaIetaIeta_;
  std::auto_ptr<std::vector<float> > 	ElSigmaIetaIetaFull5x5_;
  std::auto_ptr<std::vector<float> > 	ElEinvMinusPinv_;
  std::auto_ptr<std::vector<float> > 	ElD0_;
  std::auto_ptr<std::vector<float> > 	ElDz_;
  std::auto_ptr<std::vector<int> >   	ElExpectedMissingInnerHitCnt_;
  std::auto_ptr<std::vector<int> >   	ElPassConvVeto_;
  std::auto_ptr<std::vector<unsigned> > ElHltMatch_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoChHad_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoNeutralHad_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoIso_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoPuChHad_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoRaw_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoDbeta_;
  std::auto_ptr<std::vector<float> > 	ElPfIsoRho_;
  std::auto_ptr<std::vector<float> > 	ElAEff_;
  std::auto_ptr<std::vector<float> >    ElDr03TkSumPt_;
  std::auto_ptr<std::vector<float> >    ElDr03EcalRecHitSumEt_;
  std::auto_ptr<std::vector<float> >    ElDr03HcalTowerSumEt_;

  //Modified by Clement Leloup
  // Taus
  std::auto_ptr<std::vector<float> > 	TauPt_;
  std::auto_ptr<std::vector<float> > 	TauEta_;
  std::auto_ptr<std::vector<float> > 	TauPhi_;
  std::auto_ptr<std::vector<float> > 	TauE_;
  std::auto_ptr<std::vector<float> > 	TauCh_;
  std::auto_ptr<std::vector<unsigned> > TauDecayModeFinding_;
  std::auto_ptr<std::vector<float> >    TauCombinedIsolationDeltaBetaCorrRaw3Hits_;
  std::auto_ptr<std::vector<unsigned> > TauDiscMuonLoose_;
  std::auto_ptr<std::vector<unsigned> > TauDiscMuonTight_;
  std::auto_ptr<std::vector<unsigned> > TauDiscElVLoose_;
  std::auto_ptr<std::vector<unsigned> > TauDiscElLoose_;
  std::auto_ptr<std::vector<unsigned> > TauDiscElVTight_;
  std::auto_ptr<std::vector<unsigned> > TauDiscElTight_;

  //PF particle candidates
  std::auto_ptr<std::vector<float> > PfCandPt_;
  std::auto_ptr<std::vector<float> > PfCandEta_;
  std::auto_ptr<std::vector<float> > PfCandPhi_;
  std::auto_ptr<std::vector<float> > PfCandE_;  
  std::auto_ptr<std::vector<float> > PfCandVx_;
  std::auto_ptr<std::vector<float> > PfCandVy_;
  std::auto_ptr<std::vector<float> > PfCandVz_;
  std::auto_ptr<std::vector<float> > PfCandTkInPt_;
  std::auto_ptr<std::vector<float> > PfCandTkInEta_;
  std::auto_ptr<std::vector<float> > PfCandTkInPhi_;
  std::auto_ptr<std::vector<float> > PfCandTkValidHitCnt_;
  std::auto_ptr<std::vector<float> > PfCandTkValidFrac_;
  std::auto_ptr<std::vector<float> > PfCandTkChi2_;
  std::auto_ptr<std::vector<float> > PfCandDz_;
  std::auto_ptr<std::vector<float> > PfCandDxy_;
  std::auto_ptr<std::vector<float> > PfCandDz0_;
  std::auto_ptr<std::vector<float> > PfCandDzPV_;  
  std::auto_ptr<std::vector<float> > PfCandDxyPV_;

  //Photons
  //photon momenta
  std::auto_ptr<std::vector<float> > PhotPt_;
  std::auto_ptr<std::vector<float> > PhotEta_;
  std::auto_ptr<std::vector<float> > PhotPhi_;
  std::auto_ptr<std::vector<float> > PhotScRawE_;
  std::auto_ptr<std::vector<float> > PhotScEta_;
  std::auto_ptr<std::vector<float> > PhotScPhi_;


  //photon isolations
  std::auto_ptr<std::vector<float> > PhotIsoEcal_;
  std::auto_ptr<std::vector<float> > PhotIsoHcal_;
  std::auto_ptr<std::vector<float> > PhotIsoTk_;
  std::auto_ptr<std::vector<float> > PhotPfIsoChHad_;
  std::auto_ptr<std::vector<float> > PhotPfIsoNeutralHad_;
  std::auto_ptr<std::vector<float> > PhotPfIsoPhot_;
  std::auto_ptr<std::vector<float> > PhotPfIsoPuChHad_;
  std::auto_ptr<std::vector<float> > PhotPfIsoEcalClus_;
  std::auto_ptr<std::vector<float> > PhotPfIsoHcalClus_;

  //photon cluster shapes
  std::auto_ptr<std::vector<float> > PhotE3x3_;
  std::auto_ptr<std::vector<float> > PhotE1x5_;
  std::auto_ptr<std::vector<float> > PhotE1x3_;
  std::auto_ptr<std::vector<float> > PhotE2x2_;
  std::auto_ptr<std::vector<float> > PhotE2x5_;
  std::auto_ptr<std::vector<float> > PhotE5x5_;
  std::auto_ptr<std::vector<float> > PhotSigmaIetaIeta_;
  std::auto_ptr<std::vector<float> > PhotSigmaIetaIphi_;
  std::auto_ptr<std::vector<float> > PhotSigmaIphiIphi_;
  std::auto_ptr<std::vector<float> > PhotEtaWidth_;
  std::auto_ptr<std::vector<float> > PhotPhiWidth_;
  std::auto_ptr<std::vector<float> > PhotR9_;
  std::auto_ptr<std::vector<float> > PhotS4_;
  std::auto_ptr<std::vector<float> > PhotE1x5Full5x5_;    
  std::auto_ptr<std::vector<float> > PhotE2x5Full5x5_;
  std::auto_ptr<std::vector<float> > PhotE3x3Full5x5_;
  std::auto_ptr<std::vector<float> > PhotE5x5Full5x5_;
  std::auto_ptr<std::vector<float> > PhotSigmaIetaIetaFull5x5_;
  std::auto_ptr<std::vector<float> > PhotR9Full5x5_; 

  //photon preshower
  std::auto_ptr<std::vector<float> > PhotEsE_;
  std::auto_ptr<std::vector<float> > PhotEsSigmaIxIx_;
  std::auto_ptr<std::vector<float> > PhotEsSigmaIyIy_;
  std::auto_ptr<std::vector<float> > PhotEsSigmaIrIr_;

  //photon id (bit field)
  std::auto_ptr<std::vector<unsigned> > PhotId_;
  std::map<std::string, unsigned>    	PhotIdMap_; //bit assignment
  std::auto_ptr<std::vector<float> >    PhotHoE_;
  std::auto_ptr<std::vector<bool> >     PhotHasPixelSeed_;
  std::auto_ptr<std::vector<int> >      PhotPassElVeto_;

  //photon timing
  std::auto_ptr<std::vector<float> > PhotTime_;

  //PF Jets
  std::auto_ptr<std::vector<float> > JetAk04Pt_;
  std::auto_ptr<std::vector<float> > JetAk04Eta_;
  std::auto_ptr<std::vector<float> > JetAk04Phi_;
  std::auto_ptr<std::vector<float> > JetAk04E_;
  std::auto_ptr<std::vector<float> > JetAk04Id_;
  std::auto_ptr<std::vector<bool> >  JetAk04PuId_;
  std::auto_ptr<std::vector<float> > JetAk04PuMva_;
  std::auto_ptr<std::vector<float> > JetAk04RawPt_;
  std::auto_ptr<std::vector<float> > JetAk04RawE_;
  std::auto_ptr<std::vector<float> > JetAk04HfHadE_;
  std::auto_ptr<std::vector<float> > JetAk04HfEmE_;
  std::auto_ptr<std::vector<float> > JetAk04BetaClassic_;
  std::auto_ptr<std::vector<float> > JetAk04Beta_;
  std::auto_ptr<std::vector<float> > JetAk04BetaStar_;
  std::auto_ptr<std::vector<float> > JetAk04BetaStarClassic_;
  std::auto_ptr<std::vector<float> > JetAk04Rms_;
  std::auto_ptr<std::vector<float> > JetAk04ChHadFrac_;
  std::auto_ptr<std::vector<float> > JetAk04NeutralHadAndHfFrac_;
  std::auto_ptr<std::vector<float> > JetAk04ChEmFrac_;
  std::auto_ptr<std::vector<float> > JetAk04NeutralEmFrac_;
  std::auto_ptr<std::vector<float> > JetAk04ChMult_;
  std::auto_ptr<std::vector<float> > JetAk04ConstCnt_;
  std::auto_ptr<std::vector<float> > JetAk04BTagCsv_;
  std::auto_ptr<std::vector<float> > JetAk04BTagCsvV1_;
  std::auto_ptr<std::vector<float> > JetAk04BTagCsvSLV1_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscCisvV2_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscJp_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscBjp_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscTche_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscTchp_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscSsvhe_;
  std::auto_ptr<std::vector<float> > JetAk04BDiscSsvhp_;
  std::auto_ptr<std::vector<float> > JetAk04PartFlav_;
  std::auto_ptr<std::vector<float> > JetAk04HadFlav_;
  std::auto_ptr<std::vector<float> > JetAk04JecUncUp_;
  std::auto_ptr<std::vector<float> > JetAk04JecUncDwn_;
  std::auto_ptr<std::vector<int> >   JetAk04ConstId_;
  std::auto_ptr<std::vector<float> > JetAk04ConstPt_;
  std::auto_ptr<std::vector<float> > JetAk04ConstEta_;
  std::auto_ptr<std::vector<float> > JetAk04ConstPhi_;
  std::auto_ptr<std::vector<float> > JetAk04ConstE_;
  std::auto_ptr<std::vector<int> >   JetAk04GenJet_;

  //Modified by Clement Leloup
  //PF ak08 Jets
  std::auto_ptr<std::vector<float> > JetAk08Pt_;
  std::auto_ptr<std::vector<float> > JetAk08Eta_;
  std::auto_ptr<std::vector<float> > JetAk08Phi_;
  std::auto_ptr<std::vector<float> > JetAk08E_;
  std::auto_ptr<std::vector<float> > JetAk08Id_;
  std::auto_ptr<std::vector<float> > JetAk08RawPt_;
  std::auto_ptr<std::vector<float> > JetAk08RawE_;
  std::auto_ptr<std::vector<float> > JetAk08HfHadE_;
  std::auto_ptr<std::vector<float> > JetAk08HfEmE_;
  std::auto_ptr<std::vector<float> > JetAk08ChHadFrac_;
  std::auto_ptr<std::vector<float> > JetAk08NeutralHadAndHfFrac_;
  std::auto_ptr<std::vector<float> > JetAk08ChEmFrac_;
  std::auto_ptr<std::vector<float> > JetAk08NeutralEmFrac_;
  std::auto_ptr<std::vector<float> > JetAk08ChMult_;
  std::auto_ptr<std::vector<float> > JetAk08ConstCnt_;
  std::auto_ptr<std::vector<float> > JetAk08BTagCsv_;
  std::auto_ptr<std::vector<float> > JetAk08BTagCsvV1_;
  std::auto_ptr<std::vector<float> > JetAk08BTagCsvV2_;
  std::auto_ptr<std::vector<float> > JetAk08BTagCsvSLV1_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscCisvV2_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscJp_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscBjp_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscTche_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscTchp_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscSsvhe_;
  std::auto_ptr<std::vector<float> > JetAk08BDiscSsvhp_;
  std::auto_ptr<std::vector<float> > JetAk08PartFlav_;
  std::auto_ptr<std::vector<float> > JetAk08HadFlav_;
  std::auto_ptr<std::vector<float> > JetAk08JecUncUp_;
  std::auto_ptr<std::vector<float> > JetAk08JecUncDwn_;
  std::auto_ptr<std::vector<int> >   JetAk08ConstId_;
  std::auto_ptr<std::vector<float> > JetAk08ConstPt_;
  std::auto_ptr<std::vector<float> > JetAk08ConstEta_;
  std::auto_ptr<std::vector<float> > JetAk08ConstPhi_;
  std::auto_ptr<std::vector<float> > JetAk08ConstE_;
  std::auto_ptr<std::vector<int> >   JetAk08GenJet_;
  std::auto_ptr<std::vector<float> > JetAk08PrunedMass_;
  std::auto_ptr<std::vector<float> > JetAk08FilteredMass_;
  std::auto_ptr<std::vector<float> > JetAk08SoftDropMass_;
  std::auto_ptr<std::vector<float> > JetAk08TrimmedMass_;
  std::auto_ptr<std::vector<float> > JetAk08Tau1_;
  std::auto_ptr<std::vector<float> > JetAk08Tau2_;
  std::auto_ptr<std::vector<float> > JetAk08Tau3_;
  

  //bits
  unsigned kMuIdLoose_;
  unsigned kMuIdCustom_;
  unsigned kGlobMu_;
  unsigned kTkMu_;
  unsigned kPfMu_;

  unsigned kElec17_Elec8_;
  unsigned kMu17_Mu8_;
  unsigned kMu17_TkMu8_;

  ///Event objects
  edm::Handle<GenParticleCollection> genParticles_h;
  const GenParticleCollection* genParticles;
  edm::Handle<std::vector<pat::Muon> > muons;
  const std::vector<pat::Muon> * muon;
  edm::Handle<std::vector<pat::Electron> > electrons;
  const std::vector<pat::Electron>  *electron;

  edm::Handle<reco::ConversionCollection> conversions_h;
  edm::Handle<std::vector<pat::Tau> > taus;

  //Modified by Clement Leloup
  const std::vector<pat::Tau>  *tau;  

  edm::Handle<edm::View<pat::Jet> > jets;

  //Modified by Clement Leloup
  edm::Handle<edm::View<pat::Jet> > hFatJets;

  const edm::View<pat::Jet> * jettt;

  //Modified by Clement Leloup
  const edm::View<pat::Jet> * fatjettt;

  const std::vector<pat::Photon>  *photons;

  const std::vector<pat::PackedCandidate> *candidates;
  edm::Handle<std::vector<reco::Vertex> >  pvHandle;
  edm ::Handle<reco::VertexCollection> vtx_h;
  const std::vector<reco::Vertex> * vtxx;
  double rhoIso;
  edm::Handle<reco::BeamSpot> beamSpotHandle;
  reco::BeamSpot beamSpot;
  edm::Handle<double> rho;
  edm::Handle<reco::GenJetCollection> genjetColl_;

  //Modified by Clement Leloup
  edm::Handle<reco::GenJetCollection> genfatJetColl_;
  ///

  std::vector<std::vector<int> > trigAccept_;
  std::vector<std::string> trigNames_;

  enum { candidatesOff = 0, candidatesOn, candidatesWithTrack } candidateMode_;
  enum { photonsOff = 0, photonsOn } photonMode_;
  enum { fatJetOff = 0, fatJetOn } fatJetMode_;
  
  bool trigStatValid_;

  //  int singleMuOnly_;

  //keep track if the input file contained LHE weights:
  bool weightsFromLhe_;

  //keep track if GenRunInfoProduct weights which were found:
  enum {UNKNOWN, YES, NO, MIXTURE} weightFromGenEventInfo_ = UNKNOWN;

  struct TrigSorter{
    TrigSorter(Tupel* t): tupel_(t){}
    bool operator()(int i, int j) const{
      return tupel_->trigAccept_[1+i][0] > tupel_->trigAccept_[1+j][0];
    }
    Tupel* tupel_;
  };

};

//using namespace std;
//using namespace reco;

Tupel::Tupel(const edm::ParameterSet& iConfig):

  elecMatch_( iConfig.getParameter< std::string >( "elecMatch" ) ),
  muonMatch_( iConfig.getParameter< std::string >( "muonMatch" ) ),
  muonMatch2_( iConfig.getParameter< std::string >( "muonMatch2" ) ),

  elecToken_(consumes<std::vector<pat::Electron> >(iConfig.getUntrackedParameter<edm::InputTag>("electronSrc"))),

  muonToken_(consumes<std::vector<pat::Muon> >(iConfig.getUntrackedParameter<edm::InputTag>("muonSrc"))),

  //Modified by Clement Leloup
  tauToken_(consumes<std::vector<pat::Tau> >(iConfig.getUntrackedParameter<edm::InputTag>("tauSrc"))),  

  conversionsToken_(consumes<reco::ConversionCollection>(edm::InputTag("reducedEgamma","reducedConversions"))),
  jetToken_(consumes<edm::View<pat::Jet> >(iConfig.getUntrackedParameter<edm::InputTag>("jetSrc"))),

  //Modified by Clement Leloup
  fatJetSw_(iConfig.getUntrackedParameter<std::string>("fatJetSw")),

  photonSw_(iConfig.getUntrackedParameter<std::string>("photonSw")),
  candidateSw_(iConfig.getUntrackedParameter<std::string>("candidateSw")),
  recoSw_(iConfig.getUntrackedParameter<std::string>("recoSw", "on")),
  puJetIds_(0),
  puMvaName_(iConfig.getUntrackedParameter<std::string>("puMvaName")),
  triggerStat_(iConfig.getUntrackedParameter<bool>("triggerStat", false)),
  triggerMenu_(iConfig.getUntrackedParameter<std::string>("triggerMenu", "2015")),
  analyzedEventCnt_(0),
  photonIdsListed_(false),
  elecIdsListed_(false),
  hltListed_(false),
  trigStatValid_(true),
  weightsFromLhe_(false)
{

  genParticleToken_ = consumes<std::vector<reco::GenParticle> >(iConfig.getUntrackedParameter<edm::InputTag>("genSrc"));
  gjetToken_ = consumes<std::vector<reco::GenJet> >(iConfig.getUntrackedParameter<edm::InputTag>("gjetSrc"));
  generatorToken_ = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
  lheEventToken_ = consumes<LHEEventProduct>(iConfig.getUntrackedParameter<edm::InputTag>("lheSrc"));
  lheRunToken_ = consumes<LHERunInfoProduct, edm::InRun>(iConfig.getUntrackedParameter<edm::InputTag>("lheSrc"));

  if(recoSw_==std::string("on")){
    vertexToken_ = consumes<std::vector<reco::Vertex> >(iConfig.getUntrackedParameter<edm::InputTag>("pvSrc"));
    mSrcRhoToken_ = consumes<double>(iConfig.getUntrackedParameter<edm::InputTag>("mSrcRho" ));
    beamSpotToken_ = consumes<reco::BeamSpot>(edm::InputTag("offlineBeamSpot"));
    HLTTagToken_ = consumes<edm::TriggerResults>(edm::InputTag("TriggerResults", "", "HLT"));
    metFilTagToken_ = consumes<edm::TriggerResults>(edm::InputTag("TriggerResults", "", "PAT"));
    metFilTagRECOToken_ = consumes<edm::TriggerResults>(edm::InputTag("TriggerResults", "", "RECO"));
    puToken_ = consumes<std::vector<PileupSummaryInfo> >(iConfig.getUntrackedParameter<edm::InputTag>("puSrc"));
    puJetIdsToken_ = consumes<edm::ValueMap<StoredPileupJetIdentifier> >(iConfig.getUntrackedParameter<edm::InputTag>("puJetIdSrc"));
    ecalHitEBToken_ =  consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>( "reducedBarrelRecHitCollection" ) ) ;
    ecalHitEEToken_ =  consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>( "reducedEndcapRecHitCollection" ) ) ;
    ecalHitESToken_ =  consumes<EcalRecHitCollection>( iConfig.getParameter<edm::InputTag>( "reducedPreshowerRecHitCollection" ) );
    std::vector<edm::InputTag> metConfig = iConfig.getParameter<std::vector<edm::InputTag> >("metSrcs");
    for (auto& conf : metConfig) {
      metSrcsToken.push_back(consumes<edm::View<pat::MET> >(conf));   
    }
    if(photonSw_ == std::string("on")){
      photonMode_ = photonsOn;
      photonToken_ = consumes<std::vector<pat::Photon> >(iConfig.getUntrackedParameter<edm::InputTag>("photonSrc"));
    } else if(photonSw_ == std::string("off")){
      photonMode_ = photonsOff;
    } else{
      throw cms::Exception(TString::Format("Invalid value (%s) for photonSw parameter. Valid values are on and off.", photonSw_.c_str()));
    }
    if(candidateSw_==std::string("on") || candidateSw_==std::string("withTrack")){
      candidateToken_ = consumes<std::vector<pat::PackedCandidate> >(iConfig.getUntrackedParameter<edm::InputTag>("candidateSrc"));
      if(candidateSw_ == std::string("on")) candidateMode_ = candidatesOn;
    } else if(candidateSw_ == std::string("off")){
      candidateMode_ = candidatesOff;
    } else{
      throw cms::Exception(TString::Format("Invalid value (%s) for candidateSw parameter. Valid values are: all, withTrack, and off.", candidateSw_.c_str()));
    }
  } else if(recoSw_!=std::string("off")){
    throw cms::Exception(TString::Format("Invalid value (%s) for recoSw parameter. Valid values are: on and off.", fatJetSw_.c_str()));
  }
  
  //Modified by Clement Leloup
  if(fatJetSw_==std::string("on")){
    if(recoSw_==std::string("on")) fatJetToken_ = consumes<edm::View<pat::Jet> >(iConfig.getUntrackedParameter<edm::InputTag>("fatJetSrc"));
    gfatJetToken_ = consumes<std::vector<reco::GenJet> >(iConfig.getUntrackedParameter<edm::InputTag>("gfatJetSrc"));
    fatJetMode_ = fatJetOn;
  } else if(fatJetSw_ == std::string("off")){
    fatJetMode_ = fatJetOff;
  } else{
    throw cms::Exception(TString::Format("Invalid value (%s) for useFatJet parameter. Valid values are: on and off.", fatJetSw_.c_str()));
  }
}

Tupel::~Tupel()
{
}

void Tupel::defineBitFields(){

  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltPhotMap_, TrigHltPhot_.get()));
  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltDiPhotMap_, TrigHltDiPhot_.get()));
  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltMuMap_,   TrigHltMu_.get()));
  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltDiMuMap_, TrigHltDiMu_.get()));
  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltElMap_,   TrigHltEl_.get()));
  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltDiElMap_, TrigHltDiEl_.get()));
  trigHltMapList_.push_back(TrigHltMapRcd(&TrigHltElMuMap_, TrigHltElMu_.get()));

  if(triggerMenu_ == "2015"){
    #include "trigger2015.h"
  } else if(triggerMenu_ == "2016"){
    #include "trigger2016.h"
  } else{
    throw cms::Exception("Value of triggerMenu parameter is not valid.");
  }

  DEF_BIT(TrigHlt, 0, Elec17_Elec8);
  DEF_BIT(TrigHlt, 1, Mu17_Mu8);
  DEF_BIT(TrigHlt, 2, Mu17_TkMu8);

  DEF_BIT2(TrigHltPhot,  0, HLT_DoublePhoton60);
  DEF_BIT2(TrigHltPhot,  1, HLT_DoublePhoton85);
  DEF_BIT2(TrigHltPhot,  2, HLT_HISinglePhoton10ForEndOfFill);
  DEF_BIT2(TrigHltPhot,  3, HLT_HISinglePhoton15ForEndOfFill);
  DEF_BIT2(TrigHltPhot,  4, HLT_HISinglePhoton20ForEndOfFill);
  DEF_BIT2(TrigHltPhot,  5, HLT_HISinglePhoton40);
  DEF_BIT2(TrigHltPhot,  6, HLT_HISinglePhoton60);
  DEF_BIT2(TrigHltPhot,  7, HLT_Mu12_Photon25_CaloIdL_L1ISO);
  DEF_BIT2(TrigHltPhot,  8, HLT_Mu12_Photon25_CaloIdL_L1OR);
  DEF_BIT2(TrigHltPhot,  9, HLT_Mu12_Photon25_CaloIdL);
  DEF_BIT2(TrigHltPhot, 10, HLT_Mu17_Photon22_CaloIdL_L1ISO);
  DEF_BIT2(TrigHltPhot, 11, HLT_Mu17_Photon30_CaloIdL_L1ISO);
  DEF_BIT2(TrigHltPhot, 12, HLT_Mu17_Photon35_CaloIdL_L1ISO);
  DEF_BIT2(TrigHltPhot, 13, HLT_Mu23NoFiltersNoVtx_Photon23_CaloIdL);
  DEF_BIT2(TrigHltPhot, 14, HLT_Mu28NoFiltersNoVtxDisplaced_Photon28_CaloIdL);
  DEF_BIT2(TrigHltPhot, 15, HLT_Mu33NoFiltersNoVtxDisplaced_Photon33_CaloIdL);
  DEF_BIT2(TrigHltPhot, 16, HLT_Mu38NoFiltersNoVtx_Photon38_CaloIdL);
  DEF_BIT2(TrigHltPhot, 17, HLT_Mu42NoFiltersNoVtx_Photon42_CaloIdL);
  DEF_BIT2(TrigHltPhot, 18, HLT_Photon120_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 19, HLT_Photon120_R9Id90_HE10_Iso40_EBOnly);
  DEF_BIT2(TrigHltPhot, 20, HLT_Photon120_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 21, HLT_Photon120);
  DEF_BIT2(TrigHltPhot, 22, HLT_Photon135_PFMET100_JetIdCleaned);
  DEF_BIT2(TrigHltPhot, 23, HLT_Photon165_HE10);
  DEF_BIT2(TrigHltPhot, 24, HLT_Photon165_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 25, HLT_Photon175);
  DEF_BIT2(TrigHltPhot, 26, HLT_Photon22_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 27, HLT_Photon22_R9Id90_HE10_Iso40_EBOnly);
  DEF_BIT2(TrigHltPhot, 28, HLT_Photon22_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 29, HLT_Photon22);
  DEF_BIT2(TrigHltPhot, 30, HLT_Photon250_NoHE);
  DEF_BIT2(TrigHltPhot, 31, HLT_Photon26_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon16_AND_HE10_R9Id65_Eta2_Mass60);
  DEF_BIT2(TrigHltPhot, 32, HLT_Photon300_NoHE);
  DEF_BIT2(TrigHltPhot, 33, HLT_Photon30_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 34, HLT_Photon30);
  DEF_BIT2(TrigHltPhot, 35, HLT_Photon36_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon22_AND_HE10_R9Id65_Eta2_Mass15);
  DEF_BIT2(TrigHltPhot, 36, HLT_Photon36_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 37, HLT_Photon36_R9Id90_HE10_Iso40_EBOnly);
  DEF_BIT2(TrigHltPhot, 38, HLT_Photon36_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 39, HLT_Photon36);
  DEF_BIT2(TrigHltPhot, 40, HLT_Photon42_R9Id85_OR_CaloId24b40e_Iso50T80L_Photon25_AND_HE10_R9Id65_Eta2_Mass15);
  DEF_BIT2(TrigHltPhot, 41, HLT_Photon500);
  DEF_BIT2(TrigHltPhot, 42, HLT_Photon50_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 43, HLT_Photon50_R9Id90_HE10_Iso40_EBOnly);
  DEF_BIT2(TrigHltPhot, 44, HLT_Photon50_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 45, HLT_Photon50);
  DEF_BIT2(TrigHltPhot, 46, HLT_Photon600);
  DEF_BIT2(TrigHltPhot, 47, HLT_Photon75_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 48, HLT_Photon75_R9Id90_HE10_Iso40_EBOnly);
  DEF_BIT2(TrigHltPhot, 49, HLT_Photon75_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 50, HLT_Photon75);
  DEF_BIT2(TrigHltPhot, 51, HLT_Photon90_CaloIdL_PFHT500);
  DEF_BIT2(TrigHltPhot, 52, HLT_Photon90_CaloIdL_PFHT600);
  DEF_BIT2(TrigHltPhot, 53, HLT_Photon90_R9Id90_HE10_Iso40_EBOnly_PFMET40);
  DEF_BIT2(TrigHltPhot, 54, HLT_Photon90_R9Id90_HE10_Iso40_EBOnly);
  DEF_BIT2(TrigHltPhot, 55, HLT_Photon90_R9Id90_HE10_IsoM);
  DEF_BIT2(TrigHltPhot, 56, HLT_Photon90);

  DEF_BIT2(TrigHltDiPhot, 1, HLT_DiSC30_18_EIso_AND_HE_Mass70);   //aggiunti per Boff
  DEF_BIT2(TrigHltDiPhot, 2, HLT_DoublePhoton40);
  DEF_BIT2(TrigHltDiPhot, 3, HLT_DoublePhoton50);
  DEF_BIT2(TrigHltDiPhot, 4, HLT_DoublePhoton85);
  DEF_BIT2(TrigHltDiPhot, 5, HLT_Diphoton30EB_18EB_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55); //aggiunti per Bon
  DEF_BIT2(TrigHltDiPhot, 6, HLT_Diphoton30PV_18PV_R9Id_AND_IsoCaloId_AND_HE_R9Id_DoublePixelVeto_Mass55);
  DEF_BIT2(TrigHltDiPhot, 7, HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_DoublePixelSeedMatch_Mass70);
  DEF_BIT2(TrigHltDiPhot, 8, HLT_Diphoton30_18_R9Id_OR_IsoCaloId_AND_HE_R9Id_Mass95);
  DEF_BIT2(TrigHltDiPhot, 9, HLT_Diphoton30_18_Solid_R9Id_AND_IsoCaloId_AND_HE_R9Id_Mass55);
  DEF_BIT2(TrigHltDiPhot, 10, HLT_DoublePhoton60);

  DEF_BIT2(TrigHltDiMu, 1 ,HLT_DoubleMu33NoFiltersNoVtx);
  DEF_BIT2(TrigHltDiMu, 2 ,HLT_DoubleMu38NoFiltersNoVtx);
  DEF_BIT2(TrigHltDiMu, 3 ,HLT_DoubleMu23NoFiltersNoVtxDisplaced);
  DEF_BIT2(TrigHltDiMu, 4 ,HLT_DoubleMu28NoFiltersNoVtxDisplaced);
  DEF_BIT2(TrigHltDiMu, 5 ,HLT_DoubleIsoMu17_eta2p1);
  DEF_BIT2(TrigHltDiMu, 6 ,HLT_L2DoubleMu23_NoVertex);
  DEF_BIT2(TrigHltDiMu, 7 ,HLT_L2DoubleMu28_NoVertex_2Cha_Angle2p5_Mass10);
  DEF_BIT2(TrigHltDiMu, 8 ,HLT_L2DoubleMu38_NoVertex_2Cha_Angle2p5_Mass10);
  DEF_BIT2(TrigHltDiMu, 9 ,HLT_Mu17_Mu8_DZ);
  DEF_BIT2(TrigHltDiMu, 10,HLT_Mu17_Mu8_SameSign_DZ);
  DEF_BIT2(TrigHltDiMu, 11,HLT_Mu20_Mu10);
  DEF_BIT2(TrigHltDiMu, 12,HLT_Mu20_Mu10_DZ);
  DEF_BIT2(TrigHltDiMu, 13,HLT_Mu20_Mu10_SameSign_DZ);
  DEF_BIT2(TrigHltDiMu, 14,HLT_Mu27_TkMu8);
  DEF_BIT2(TrigHltDiMu, 15,HLT_Mu30_TkMu11);
  DEF_BIT2(TrigHltDiMu, 16,HLT_Mu40_TkMu11);
  DEF_BIT2(TrigHltDiMu, 17,HLT_Mu17_TkMu8_DZ);
  DEF_BIT2(TrigHltDiMu, 18,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL);
  DEF_BIT2(TrigHltDiMu, 19,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ);
  DEF_BIT2(TrigHltDiMu, 20,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL);
  DEF_BIT2(TrigHltDiMu, 21,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ);
  DEF_BIT2(TrigHltDiMu, 22,HLT_DoubleMu18NoFiltersNoVtx);
  DEF_BIT2(TrigHltDiMu, 23,HLT_TrkMu15_DoubleTrkMu5NoFiltersNoVtx);
  DEF_BIT2(TrigHltDiMu, 24,HLT_TrkMu17_DoubleTrkMu8NoFiltersNoVtx);

  DEF_BIT2(TrigHltMu, 1 , HLT_Mu16_eta2p1_CaloMET30);
  DEF_BIT2(TrigHltMu, 2 , HLT_IsoMu16_eta2p1_CaloMET30);
  DEF_BIT2(TrigHltMu, 3 , HLT_IsoMu16_eta2p1_CaloMET30_LooseIsoPFTau50_Trk30_eta2p1);
  DEF_BIT2(TrigHltMu, 4 , HLT_IsoMu17_eta2p1);
  DEF_BIT2(TrigHltMu, 5 , HLT_IsoMu17_eta2p1_LooseIsoPFTau20);
  DEF_BIT2(TrigHltMu, 6 , HLT_IsoMu17_eta2p1_LooseIsoPFTau20_SingleL1);
  DEF_BIT2(TrigHltMu, 7 , HLT_IsoMu17_eta2p1_MediumIsoPFTau40_Trk1_eta2p1_Reg);
  DEF_BIT2(TrigHltMu, 8 , HLT_IsoMu24_eta2p1_LooseIsoPFTau20);
  DEF_BIT2(TrigHltMu, 9 , HLT_IsoMu20_eta2p1_CentralPFJet30_BTagCSV07);
  DEF_BIT2(TrigHltMu, 10, HLT_IsoMu20_eta2p1_TriCentralPFJet30);
  DEF_BIT2(TrigHltMu, 11, HLT_IsoMu20_eta2p1_TriCentralPFJet50_40_30);
  DEF_BIT2(TrigHltMu, 12, HLT_IsoMu20);
  DEF_BIT2(TrigHltMu, 13, HLT_IsoMu20_eta2p1);
  DEF_BIT2(TrigHltMu, 14, HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07);
  DEF_BIT2(TrigHltMu, 15, HLT_IsoMu24_eta2p1_TriCentralPFJet30);
  DEF_BIT2(TrigHltMu, 16, HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30);
  DEF_BIT2(TrigHltMu, 17, HLT_IsoMu24_eta2p1);
  DEF_BIT2(TrigHltMu, 18, HLT_IsoMu27);
  DEF_BIT2(TrigHltMu, 19, HLT_IsoTkMu20);
  DEF_BIT2(TrigHltMu, 20, HLT_IsoTkMu20_eta2p1);
  DEF_BIT2(TrigHltMu, 21, HLT_IsoTkMu24_eta2p1);
  DEF_BIT2(TrigHltMu, 22, HLT_IsoTkMu27);
  DEF_BIT2(TrigHltMu, 23, HLT_L2Mu10);
  DEF_BIT2(TrigHltMu, 24, HLT_L2Mu10_NoVertex_NoBPTX3BX_NoHalo);
  DEF_BIT2(TrigHltMu, 25, HLT_L2Mu10_NoVertex_NoBPTX);
  DEF_BIT2(TrigHltMu, 26, HLT_L2Mu35_NoVertex_3Sta_NoBPTX3BX_NoHalo);
  DEF_BIT2(TrigHltMu, 27, HLT_L2Mu40_NoVertex_3Sta_NoBPTX3BX_NoHalo);
  DEF_BIT2(TrigHltMu, 28, HLT_Mu20);
  DEF_BIT2(TrigHltMu, 29, HLT_TkMu20);
  DEF_BIT2(TrigHltMu, 30, HLT_Mu24_eta2p1);
  DEF_BIT2(TrigHltMu, 31, HLT_TkMu24_eta2p1);
  DEF_BIT2(TrigHltMu, 32, HLT_Mu27);
  DEF_BIT2(TrigHltMu, 33, HLT_TkMu27);
  DEF_BIT2(TrigHltMu, 34, HLT_Mu50);
  DEF_BIT2(TrigHltMu, 35, HLT_Mu55);
  DEF_BIT2(TrigHltMu, 36, HLT_Mu45_eta2p1);
  DEF_BIT2(TrigHltMu, 37, HLT_Mu50_eta2p1);
  DEF_BIT2(TrigHltMu, 38, HLT_Mu8_TrkIsoVVL);
  DEF_BIT2(TrigHltMu, 39, HLT_Mu17_TrkIsoVVL);
  DEF_BIT2(TrigHltMu, 40, HLT_Mu24_TrkIsoVVL);
  DEF_BIT2(TrigHltMu, 41, HLT_Mu34_TrkIsoVVL);
  DEF_BIT2(TrigHltMu, 42, HLT_Mu8);
  DEF_BIT2(TrigHltMu, 43, HLT_Mu17);
  DEF_BIT2(TrigHltMu, 44, HLT_Mu24);
  DEF_BIT2(TrigHltMu, 45, HLT_Mu34);
  DEF_BIT2(TrigHltMu, 46, HLT_IsoMu22);
  DEF_BIT2(TrigHltMu, 47, HLT_IsoMu18);
  DEF_BIT2(TrigHltMu, 48, HLT_IsoTkMu18);

  DEF_BIT2(TrigHltEl,  1, HLT_Ele25WP60_Ele8_Mass55);
  DEF_BIT2(TrigHltEl,  2, HLT_Ele25WP60_SC4_Mass55);
  DEF_BIT2(TrigHltEl,  3, HLT_Ele22_eta2p1_WPLoose_Gsf);
  DEF_BIT2(TrigHltEl,  4, HLT_Ele22_eta2p1_WPTight_Gsf);
  DEF_BIT2(TrigHltEl,  5, HLT_Ele22_eta2p1_WPLoose_Gsf_LooseIsoPFTau20);
  DEF_BIT2(TrigHltEl,  6, HLT_Ele23_WPLoose_Gsf);
  DEF_BIT2(TrigHltEl,  7, HLT_Ele27_WPLoose_Gsf_WHbbBoost);
  DEF_BIT2(TrigHltEl,  8, HLT_Ele27_eta2p1_WPLoose_Gsf_LooseIsoPFTau20);
  DEF_BIT2(TrigHltEl,  9, HLT_Ele27_eta2p1_WPLoose_Gsf_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg);
  DEF_BIT2(TrigHltEl, 10, HLT_Ele27_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07);
  DEF_BIT2(TrigHltEl, 11, HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet30);
  DEF_BIT2(TrigHltEl, 12, HLT_Ele27_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30);
  DEF_BIT2(TrigHltEl, 13, HLT_Ele27_eta2p1_WPLoose_Gsf);
  DEF_BIT2(TrigHltEl, 14, HLT_Ele27_eta2p1_WPTight_Gsf);
  DEF_BIT2(TrigHltEl, 15, HLT_Ele32_eta2p1_WPLoose_Gsf_LooseIsoPFTau20);
  DEF_BIT2(TrigHltEl, 16, HLT_Ele32_eta2p1_WPLoose_Gsf_DoubleMediumIsoPFTau40_Trk1_eta2p1_Reg);
  DEF_BIT2(TrigHltEl, 17, HLT_Ele32_eta2p1_WPLoose_Gsf_CentralPFJet30_BTagCSV07);
  DEF_BIT2(TrigHltEl, 18, HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet30);
  DEF_BIT2(TrigHltEl, 19, HLT_Ele32_eta2p1_WPLoose_Gsf_TriCentralPFJet50_40_30);
  DEF_BIT2(TrigHltEl, 20, HLT_Ele32_eta2p1_WPLoose_Gsf);
  DEF_BIT2(TrigHltEl, 21, HLT_Ele32_eta2p1_WPTight_Gsf);
  DEF_BIT2(TrigHltEl, 22, HLT_Ele105_CaloIdVT_GsfTrkIdT);
  DEF_BIT2(TrigHltEl, 23, HLT_Ele115_CaloIdVT_GsfTrkIdT);
  DEF_BIT2(TrigHltEl, 24, HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30);
  DEF_BIT2(TrigHltEl, 25, HLT_Ele18_CaloIdL_TrackIdL_IsoVL_PFJet30);
  DEF_BIT2(TrigHltEl, 26, HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30);
  DEF_BIT2(TrigHltEl, 27, HLT_Ele33_CaloIdL_TrackIdL_IsoVL_PFJet30);
  DEF_BIT2(TrigHltEl, 28, HLT_Ele17_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltEl, 29, HLT_Ele23_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltEl, 30, HLT_Ele12_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltEl, 31, HLT_Ele27_eta2p1_WPLoose_Gsf_HT200);
  DEF_BIT2(TrigHltEl, 32, HLT_Ele10_CaloIdM_TrackIdM_CentralPFJet30_BTagCSV0p54PF);
  DEF_BIT2(TrigHltEl, 33, HLT_Ele15_IsoVVVL_BTagCSV0p72_PFHT400);
  DEF_BIT2(TrigHltEl, 34, HLT_Ele15_IsoVVVL_PFHT350_PFMET70);
  DEF_BIT2(TrigHltEl, 35, HLT_Ele15_IsoVVVL_PFHT600);
  DEF_BIT2(TrigHltEl, 36, HLT_Ele8_CaloIdM_TrackIdM_PFJet30);
  DEF_BIT2(TrigHltEl, 37, HLT_Ele12_CaloIdM_TrackIdM_PFJet30);
  DEF_BIT2(TrigHltEl, 38, HLT_Ele18_CaloIdM_TrackIdM_PFJet30);
  DEF_BIT2(TrigHltEl, 39, HLT_Ele23_CaloIdM_TrackIdM_PFJet30);
  DEF_BIT2(TrigHltEl, 40, HLT_Ele33_CaloIdM_TrackIdM_PFJet30);
  DEF_BIT2(TrigHltEl, 41, HLT_Ele15_PFHT300);

  DEF_BIT2(TrigHltDiEl, 1, HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
  DEF_BIT2(TrigHltDiEl, 2, HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ);
  DEF_BIT2(TrigHltDiEl, 3, HLT_Ele16_Ele12_Ele8_CaloIdL_TrackIdL);
  DEF_BIT2(TrigHltDiEl, 4, HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltDiEl, 5, HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltDiEl, 6, HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT300);
  DEF_BIT2(TrigHltDiEl, 7, HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf);
  DEF_BIT2(TrigHltDiEl, 8, HLT_DoubleEle33_CaloIdL_GsfTrkIdVL_MW);
  DEF_BIT2(TrigHltDiEl, 9, HLT_DoubleEle33_CaloIdL_GsfTrkIdVL);
  DEF_BIT2(TrigHltDiEl, 10, HLT_DoubleEle25_CaloIdL_GsfTrkIdVL);  //aggiunto per Boff
  DEF_BIT2(TrigHltDiEl, 11, HLT_DoubleEle8_CaloIdM_TrackIdM_Mass8_PFHT250); //aggiunti per Bon
  DEF_BIT2(TrigHltDiEl, 12, HLT_Ele30WP60_Ele8_Mass55);
  DEF_BIT2(TrigHltDiEl, 13, HLT_Ele30WP60_SC4_Mass55);


  DEF_BIT2(TrigHltElMu, 1, HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltElMu, 2, HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltElMu, 3, HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltElMu, 4, HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL);
  DEF_BIT2(TrigHltElMu, 5, HLT_Mu30_Ele30_CaloIdL_GsfTrkIdVL);

  DEF_BIT(MuId, 0, MuIdLoose);
  DEF_BIT_L(MuId, 3, MuIdCustom, "Mu Id: isGlobalMuon\n"
	    "&& isPFMuon\n"
	    "&& normChi2 < 10\n"
	    "&& muonHits > 0 && nMatches > \n"
	    "&& dB < 0.2 && dZ < 0.5\n"
	    "&& pixelHits > 0 && trkLayers > 5");
  DEF_BIT(MuType, 0, GlobMu);
  DEF_BIT(MuType, 1, TkMu);
  DEF_BIT(MuType, 2, PfMu);

  DEF_BIT2(ElId, 0, cutBasedElectronID-CSA14-50ns-V1-standalone-veto); 
  DEF_BIT2(ElId, 1, cutBasedElectronID-CSA14-50ns-V1-standalone-loose);
  DEF_BIT2(ElId, 2, cutBasedElectronID-CSA14-50ns-V1-standalone-medium);
  DEF_BIT2(ElId, 3, cutBasedElectronID-CSA14-50ns-V1-standalone-tight);

  DEF_BIT2(ElId, 4, cutBasedElectronID-CSA14-PU20bx25-V0-standalone-loose);
  DEF_BIT2(ElId, 5, cutBasedElectronID-CSA14-PU20bx25-V0-standalone-medium);
  DEF_BIT2(ElId, 6, cutBasedElectronID-CSA14-PU20bx25-V0-standalone-tight);
  DEF_BIT2(ElId, 7, cutBasedElectronID-CSA14-PU20bx25-V0-standalone-veto);

  DEF_BIT2(ElId, 8,  cutBasedElectronID-Spring15-25ns-V1-standalone-loose);
  DEF_BIT2(ElId, 9,  cutBasedElectronID-Spring15-25ns-V1-standalone-medium);
  DEF_BIT2(ElId, 10, cutBasedElectronID-Spring15-25ns-V1-standalone-tight);
  DEF_BIT2(ElId, 11, cutBasedElectronID-Spring15-25ns-V1-standalone-veto);

  DEF_BIT2(ElId, 12, cutBasedElectronID-Spring15-50ns-V1-standalone-loose); 
  DEF_BIT2(ElId, 13, cutBasedElectronID-Spring15-50ns-V1-standalone-medium);
  DEF_BIT2(ElId, 14, cutBasedElectronID-Spring15-50ns-V1-standalone-tight);
  DEF_BIT2(ElId, 15, cutBasedElectronID-Spring15-50ns-V1-standalone-veto);

  DEF_BIT2(ElId, 16, eidLoose);
  DEF_BIT2(ElId, 17, eidTight);
  DEF_BIT2(ElId, 18, eidRobustLoose);
  DEF_BIT2(ElId, 19, eidRobustTight);

  DEF_BIT2(PhotId, 0, PhotonCutBasedIDLoose);
  DEF_BIT2(PhotId, 2, PhotonCutBasedIDTight);
}

void Tupel::readEvent(const edm::Event& iEvent){
  *EvtNum_        = iEvent.id().event();
  *EvtRunNum_     = iEvent.id().run();
  *EvtLumiNum_    = iEvent.luminosityBlock();
  *EvtBxNum_      = iEvent.bunchCrossing();
  *EvtIsRealData_ = iEvent.isRealData();

  const pat::helper::TriggerMatchHelper matchHelper;


  // get gen particle collection
  iEvent.getByToken(genParticleToken_, genParticles_h);
  genParticles  = genParticles_h.failedToGet () ? 0 : &*genParticles_h;

  if(recoSw_==std::string("on")){
    
    // get muon collection
    iEvent.getByToken(muonToken_, muons);
    muon = muons.failedToGet () ? 0 : &*muons;
    
    // get electron collection
    iEvent.getByToken(elecToken_,electrons);
    electron = electrons.failedToGet () ? 0 :  &*electrons;
    
    //Modified by Clement Leloup
    iEvent.getByToken(tauToken_, taus);
    tau = taus.failedToGet () ? 0 : &*taus;
    
    //get photon conversion collection
    iEvent.getByToken(conversionsToken_, conversions_h);

    // get jet collection
    iEvent.getByToken(jetToken_, jets);  
    jettt = jets.failedToGet () ? 0 : &*jets ;
    
    //Modified by Clement Leloup
    // get fat jet collection
    if(fatJetMode_){
      iEvent.getByToken(fatJetToken_, hFatJets);  
      fatjettt = hFatJets.failedToGet () ? 0 : &*hFatJets ;
    } else{
      fatjettt = 0;
    }

    edm::Handle<edm::ValueMap<StoredPileupJetIdentifier> > puJetIdHandle;
    iEvent.getByToken(puJetIdsToken_, puJetIdHandle);
    puJetIds_ = puJetIdHandle.failedToGet() ? 0 : &* puJetIdHandle;
    
    // get photon collection
    if(photonMode_){
      edm::Handle<std::vector<pat::Photon> > hPhotons;  
      iEvent.getByToken(photonToken_, hPhotons);
      photons = hPhotons.failedToGet () ? 0 :  &*hPhotons;
    } else{
      photons = 0;
    }
    
    // get PF candidate collection
    if(candidateMode_){
      edm::Handle<std::vector<pat::PackedCandidate> > hCandidates;  
      iEvent.getByToken(candidateToken_, hCandidates);  
      candidates = hCandidates.failedToGet() ? 0 : &*hCandidates;
    } else{
      candidates = 0;
    }
    //get primary vertex collection
    iEvent.getByToken(vertexToken_, pvHandle);
    vtxx = pvHandle.failedToGet () ? 0 : &*pvHandle ;
    
    //get rho information
    iEvent.getByToken(mSrcRhoToken_, rho);
    
    rhoIso=99;
    if(!rho.failedToGet()) rhoIso = *rho;
    
    //get beam spot information
    iEvent.getByToken(beamSpotToken_, beamSpotHandle);
    beamSpot = beamSpotHandle.failedToGet() ? reco::BeamSpot() : *beamSpotHandle;
    
  }
  
  //get Gen jets
  iEvent.getByToken(gjetToken_, genjetColl_);
  
  //Modified by Clement Leloup
  //get Gen fat jets
  if(fatJetMode_){
    iEvent.getByToken(gfatJetToken_, genfatJetColl_);
  }
}

void Tupel::processMET(const edm::Event& iEvent){

  for(unsigned int imet=0;imet<metSrcsToken.size();imet++){
    Handle<edm::View<pat::MET> > metH;  
    iEvent.getByToken(metSrcsToken[imet], metH);
//      if (imet == 1) {
//          iEvent.getByToken(newMetToken_, metH);
//      }
//      else if (imet == 2){
//          iEvent.getByToken(new2MetToken_, metH);
//      }
//      else{
//          iEvent.getByToken(metSrcsToken[imet], metH);
//      }

    if(!metH.isValid()) continue;
    
	//std::cout<<"MET"<<imet<<"  " /*<<*metSrcsToken[imet]<< "  " <<metH->ptrAt(0)->pt() */ <<std::endl;
	//std::cout
	/*<< " GMETE_ " << metH->ptrAt(0)->genMET()->energy()*/
	/*<< " GMETPt_ " << metH->ptrAt(0)->genMET()->pt() << " GMETPx_ " << metH->ptrAt(0)->genMET()->px()*/
	//<< " met.pt() " << metH->ptrAt(0)->pt() /*<< " met.px() " << metH->ptrAt(0)->px()*/
	//<< " met.uncorPt() " << metH->ptrAt(0)->uncorPt()
	//<< " corr Pt " << metH->ptrAt(0)->pt() - metH->ptrAt(0)->uncorPt()
	/*<< " met.uncorPhi() " << metH->ptrAt(0)->uncorPhi()*/
	/*<< " met.uncorPx() " << metH->ptrAt(0)->uncorPx()*/
	//<< std::endl;

    METPt_->push_back(metH->ptrAt(0)->pt());
    METPx_->push_back(metH->ptrAt(0)->px());
    METPy_->push_back(metH->ptrAt(0)->py());
    METPz_->push_back(metH->ptrAt(0)->pz());
    METE_->push_back(metH->ptrAt(0)->energy());
    METsigx2_->push_back(metH->ptrAt(0)->getSignificanceMatrix()(0,0));
    METsigxy_->push_back(metH->ptrAt(0)->getSignificanceMatrix()(0,1));
    METsigy2_->push_back(metH->ptrAt(0)->getSignificanceMatrix()(1,1));
    METsig_->push_back(metH->ptrAt(0)->significance());
    if(!*EvtIsRealData_){
      GMETPt_->push_back(metH->ptrAt(0)->genMET()->pt());
      GMETPx_->push_back(metH->ptrAt(0)->genMET()->px());
      GMETPy_->push_back(metH->ptrAt(0)->genMET()->py());
      GMETPz_->push_back(metH->ptrAt(0)->genMET()->pz());
      GMETE_->push_back(metH->ptrAt(0)->genMET()->energy());
    }
  }
}

void Tupel::processVtx(){
  if(vtxx){
    for (std::vector<reco::Vertex>::const_iterator vtx = pvHandle->begin(); vtx != pvHandle->end(); ++vtx){
      if (vtx->isValid() && !vtx->isFake()) ++(*EvtVtxCnt_);
    }
  }
}

void Tupel::processPu(const edm::Event& iEvent){
  Handle<std::vector< PileupSummaryInfo > >  PupInfo;
  iEvent.getByToken(puToken_, PupInfo);

  if(!PupInfo.failedToGet()){
    std::vector<PileupSummaryInfo>::const_iterator PVI;
    float npT=-1.;
    float npIT=-1.;

    for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {

      int BX = PVI->getBunchCrossing();

      if(BX == 0) {
        npT = PVI->getTrueNumInteractions();
        npIT = PVI->getPU_NumInteractions();
      }
    }
    //////TO CHECK
    *EvtPuCntTruth_ = npT;
    *EvtPuCnt_      = npIT;
  }
  else {
    *EvtPuCntTruth_ = -2.;
    *EvtPuCnt_ = -2.;
  }
}

void Tupel::processGenParticles(const edm::Event& iEvent){
  const std::vector<reco::GenParticle> & gen = *genParticles_h;
  for (size_t i=0; i<genParticles->size(); ++i){
    TLorentzVector genR1DressLep1(0,0,0,0);
    int st = gen[i].status();
    int id = gen[i].pdgId();


    // ---- consider only gen  photons -----------------
    if (TMath::Abs(id) == 22) {
      TLorentzVector genPho(0, 0, 0, 0);
      genPho.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
      
      GPhotPt_->push_back(gen[i].pt());
      GPhotEta_->push_back(gen[i].eta());
      GPhotPhi_->push_back(gen[i].phi());
      GPhotE_->push_back(gen[i].energy());
      int motherId = gen[i].numberOfMothers() ? gen[i].mother()->pdgId() : 0 ;
      GPhotMotherId_->push_back(motherId);
      GPhotSt_->push_back(gen[i].status());

      //--- search for stable particles around photon in DR=0.3,0.4 and 0.5 cone around the photon
      //--- for particle isolation, don't check for invisible particles
      float isoEDR03 = 0;
      float isoEDR04 = 0;
      float isoEDR05 = 0;
      float isoSumPtDR03 = 0;
      float isoSumPtDR04 = 0;
      float isoSumPtDR05 = 0;
      for (size_t j=0; j<genParticles->size(); ++j) {
	//remove the genPho photon from the list of particles
	if (j != i) {
	  //don't use neutrinos
	  if (gen[j].status() == 1 && abs(gen[j].pdgId()) !=12 && abs(gen[j].pdgId()) !=14 && abs(gen[j].pdgId()) !=16){
	    TLorentzVector partP4GEN(0, 0, 0, 0);
	    partP4GEN.SetPtEtaPhiE(gen[j].pt(),gen[j].eta(),gen[j].phi(),gen[j].energy());
	    float DR = deltar(partP4GEN,genPho);
	    if (DR<0.5) {
	      isoEDR05 += gen[j].energy();
	      isoSumPtDR05 += gen[j].pt();
	    }
	    if (DR<0.4){
	      isoEDR04 += gen[j].energy();
	      isoSumPtDR04 += gen[j].pt();
	    }
	    if (DR<0.3) {
	      isoEDR03 += gen[j].energy();
	      isoSumPtDR03 += gen[j].pt();
	    }
	  }
	}
      } //gen photon isolation
      GPhotIsoEDR03_->push_back(isoEDR03);
      GPhotIsoEDR04_->push_back(isoEDR04);
      GPhotIsoEDR05_->push_back(isoEDR05);
      GPhotIsoSumPtDR03_->push_back(isoSumPtDR03);
      GPhotIsoSumPtDR04_->push_back(isoSumPtDR04);
      GPhotIsoSumPtDR05_->push_back(isoSumPtDR05);
    } // if |id| == 22

    
    if (11 <= abs(id) && abs(id) <= 18 //lepton or neutrino
	&& (abs(st)==23 || abs(st)==22 || abs(st)==21 || abs(st)==61 || abs(st)==3 || abs(st)==2)){
      TLorentzVector genLep3(0,0,0,0);
      if(abs(st)!=21)genLep3.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
      if(abs(st)==21)genLep3.SetPxPyPzE(0.001,0.001,gen[i].pz(),gen[i].energy());
      GLepSt3Pt_->push_back(genLep3.Pt());
      GLepSt3Eta_->push_back(eta(genLep3));
      GLepSt3Phi_->push_back(genLep3.Phi());
      GLepSt3E_->push_back(genLep3.Energy());
      GLepSt3Mother0Id_->push_back(gen[i].numberOfMothers() ? gen[i].mother()->pdgId() : -1);
      GLepSt3Id_->push_back(id);
      GLepSt3St_->push_back(st);
    }
    
    GLepSt3MotherCnt_->push_back(gen[i].numberOfMothers());

    int momId = gen[i].numberOfMothers() ? gen[i].mother()->pdgId() : -1 ;
      
    if (st==1 && (abs(id)==11 || abs(id)==13 || abs(id)==15
		  || abs(id)==12|| abs(id)==14|| abs(id)==16) /*&& gen[i].pt() > 0.1 && fabs(gen[i].eta())<3.0*/){
	
      TLorentzVector genLep1(0,0,0,0);
      genLep1.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
      TLorentzVector genR1Pho1(0,0,0,0);

      edm::Handle<std::vector<reco::GenParticle> > genpart2;//DONT know why we Need to handle another collection
      iEvent.getByToken(genParticleToken_, genpart2);
      const std::vector<reco::GenParticle> & gen2 = *genpart2;
      //LOOP over photons//
      if (st==1 && (abs(id)==13||abs(id)==11)){
	for(unsigned int j=0; j<genpart2->size(); ++j){
	  if(gen2[j].numberOfMothers()){
	    if( gen2[j].status()!=1|| gen2[j].pdgId()!=22 || gen2[j].energy()<0.000001 /*|| fabs(MomId2)!=fabs(id)*/) continue;
	    TLorentzVector thisPho1(0,0,0,0);
	    thisPho1.SetPtEtaPhiE(gen2[j].pt(),gen2[j].eta(),gen2[j].phi(),gen2[j].energy());
	    double dR = deltar(genLep1,thisPho1);
	    if(dR<0.1){
	      genR1Pho1+=thisPho1;
	    }

	    if(dR<0.2){
	      GLepClosePhotPt_->push_back(thisPho1.Pt());
	      GLepClosePhotEta_->push_back(eta(thisPho1));
	      GLepClosePhotPhi_->push_back(thisPho1.Phi());
	      GLepClosePhotE_->push_back(thisPho1.Energy());
	      GLepClosePhotId_->push_back(gen2[j].pdgId());
	      GLepClosePhotMother0Id_->push_back(fabs(gen2[j].mother()->pdgId()));
	      GLepClosePhotMotherCnt_->push_back(gen2[j].numberOfMothers());
	      GLepClosePhotSt_->push_back(gen2[j].status());
	    }
	  }
	}
      }//photon loop

      bool isPrompt = gen[i].isPromptFinalState();
      bool tauProd  = gen[i].isDirectPromptTauDecayProductFinalState();
	
      genR1DressLep1 = genLep1+genR1Pho1;
      GLepDr01Pt_->push_back(genR1DressLep1.Pt());
      GLepDr01Eta_->push_back(eta(genR1DressLep1));
      GLepDr01Phi_->push_back(genR1DressLep1.Phi());
      GLepDr01E_->push_back(genR1DressLep1.Energy());
      GLepDr01Id_->push_back(id);
      GLepDr01MomId_->push_back(momId);
      GLepDr01St_->push_back(st);
      GLepDr01Prompt_->push_back(isPrompt);
      GLepDr01TauProd_->push_back(tauProd);

      GLepBarePt_->push_back(genLep1.Pt());
      GLepBareEta_->push_back(eta(genLep1));
      GLepBarePhi_->push_back(genLep1.Phi());
      GLepBareE_->push_back(genLep1.Energy());
      GLepBareId_->push_back(id);
      GLepBareMomId_->push_back(momId);
      GLepBareSt_->push_back(st);
      GLepBarePrompt_->push_back(isPrompt);
      GLepBareTauProd_->push_back(tauProd);
    } //status lepton or neutrino
  } //gen particle loop
}

void Tupel::processGenJets(const edm::Event& iEvent){
  //matrix element info
  edm::Handle<LHEEventProduct> lheH;
  iEvent.getByToken(lheEventToken_, lheH);
  if(lheH.isValid()) *GNup_ = lheH->hepeup().NUP;
    //*GNup_ = lheH->hepeup().NUP;
    //cout << " lheH.isValid() " << lheH.isValid() << " GNup_ " << *GNup_ << endl;
    
  if(!genjetColl_.failedToGet()){
    const reco::GenJetCollection & genjet = *genjetColl_;
    for(unsigned int k=0; k < genjet.size(); ++k){
      GJetAk04Pt_->push_back(genjet[k].pt());
      GJetAk04Eta_->push_back(genjet[k].eta());
      GJetAk04Phi_->push_back(genjet[k].phi());
      GJetAk04E_->push_back(genjet[k].energy());
      GJetAk04ConstCnt_->push_back(genjet[k].numberOfDaughters());
      /*if(genjet[k].numberOfDaughters()>0){
	for(unsigned int idx =0; idx<genjet[k].numberOfDaughters();idx++){
	//cout<<genjet[k].numberOfDaughters()<<" GEN AHMEEEEET "<<idx<<"  "<<genjet[k].daughter(idx)->pdgId()<<"  "<<endl;
	GJetAk04ConstId->push_back(genjet[k].daughter(idx)->pdgId());
	GJetAk04ConstPt->push_back(genjet[k].daughter(idx)->pt());
	GJetAk04ConstEta->push_back(genjet[k].daughter(idx)->eta());
	GJetAk04ConstPhi->push_back(genjet[k].daughter(idx)->phi());
	GJetAk04ConstE->push_back(genjet[k].daughter(idx)->energy();)
	}
	}*/
      
      TLorentzVector genjetlv;
      genjetlv.SetPtEtaPhiE( genjet[k].pt(), genjet[k].eta(), genjet[k].phi(), genjet[k].energy());
      float mindr = 100.;
      int mindr_id =0;
          
      const std::vector<reco::GenParticle> & gen = *genParticles_h;
      for (size_t i=0; i<genParticles->size(); ++i){

	int id = gen[i].pdgId();
	if( !( id == 21 || abs(id) < 6 )  ) continue; // only consider quarks or gluons

	if( gen[i].pt() < 0.0000001 ) continue; // prevent from using 0 pT  particles (DeltaR fails)

	TLorentzVector genobj(0,0,0,0);
	genobj.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
	float dr = deltar(genjetlv, genobj);

	if( dr < mindr ) {
	  mindr = dr;
	  mindr_id = gen[i].pdgId();
	}
      }

      GJetAk04MatchedPartonID_->push_back(mindr_id);
      GJetAk04MatchedPartonDR_->push_back(mindr);

    }
  }
}

//Modified by Clement Leloup
void Tupel::processGenFatJets(const edm::Event& iEvent){

  //matrix element info
  edm::Handle<LHEEventProduct> lheH;
  iEvent.getByToken(lheEventToken_, lheH);
  if(lheH.isValid()) *GNup_ = lheH->hepeup().NUP;
    //*GNup_ = lheH->hepeup().NUP;
    //cout << " lheH.isValid() " << lheH.isValid() << " GNup_ " << *GNup_ << endl;
    
  if(!genfatJetColl_.failedToGet()){
    const reco::GenJetCollection & genfatjet = *genfatJetColl_;
    for(unsigned int k=0; k < genfatjet.size(); ++k){
      GJetAk08Pt_->push_back(genfatjet[k].pt());
      GJetAk08Eta_->push_back(genfatjet[k].eta());
      GJetAk08Phi_->push_back(genfatjet[k].phi());
      GJetAk08E_->push_back(genfatjet[k].energy());
      GJetAk08ConstCnt_->push_back(genfatjet[k].numberOfDaughters());
      /*if(genjet[k].numberOfDaughters()>0){
	for(unsigned int idx =0; idx<genjet[k].numberOfDaughters();idx++){
	//cout<<genjet[k].numberOfDaughters()<<" GEN AHMEEEEET "<<idx<<"  "<<genjet[k].daughter(idx)->pdgId()<<"  "<<endl;
	GJetAk04ConstId->push_back(genjet[k].daughter(idx)->pdgId());
	GJetAk04ConstPt->push_back(genjet[k].daughter(idx)->pt());
	GJetAk04ConstEta->push_back(genjet[k].daughter(idx)->eta());
	GJetAk04ConstPhi->push_back(genjet[k].daughter(idx)->phi());
	GJetAk04ConstE->push_back(genjet[k].daughter(idx)->energy();)
	}
	}*/
      
      TLorentzVector genfatjetlv;
      genfatjetlv.SetPtEtaPhiE( genfatjet[k].pt(), genfatjet[k].eta(), genfatjet[k].phi(), genfatjet[k].energy());
      float mindr = 100.;
      int mindr_id =0;
          
      const std::vector<reco::GenParticle> & gen = *genParticles_h;
      for (size_t i=0; i<genParticles->size(); ++i){

	int id = gen[i].pdgId();
	if( !( id == 21 || abs(id) < 6 )  ) continue; // only consider quarks or gluons

	//	if( gen[i].pt() < 0.0000001 ) continue; // prevent from using 0 pT  particles (DeltaR fails)

	TLorentzVector genobj(0,0,0,0);
	genobj.SetPtEtaPhiE(gen[i].pt(),gen[i].eta(),gen[i].phi(),gen[i].energy());
	float dr = deltar(genfatjetlv, genobj );

	if( dr < mindr ) {
	  mindr = dr;
	  mindr_id = gen[i].pdgId();
	}
      }

      GJetAk08MatchedPartonID_->push_back(mindr_id);
      GJetAk08MatchedPartonDR_->push_back(mindr);

    }
  }
}

void Tupel::processPdfInfo(const edm::Event& iEvent){
  edm::Handle<GenEventInfoProduct> genEventInfoProd;
  if (iEvent.getByToken(generatorToken_, genEventInfoProd)) {
    if (genEventInfoProd->hasBinningValues()){
      *GBinningValue_ = genEventInfoProd->binningValues()[0];
    }
    EvtWeights_->push_back(genEventInfoProd->weight());
  }

  if(EvtWeights_->size() == 0){
    EvtWeights_->push_back(1.);
    if(weightFromGenEventInfo_== NO) weightFromGenEventInfo_ = MIXTURE;
    else if (weightFromGenEventInfo_==UNKNOWN) weightFromGenEventInfo_ = YES;
  } else{
    if(weightFromGenEventInfo_== YES) weightFromGenEventInfo_ = MIXTURE;
    else if (weightFromGenEventInfo_==UNKNOWN) weightFromGenEventInfo_ = NO;
  }

  edm::Handle<LHEEventProduct>   lheEvent;
  if (iEvent.getByToken(lheEventToken_, lheEvent)) {
    for(unsigned iw = 0; iw < lheEvent->weights().size(); ++iw){
      EvtWeights_->push_back(lheEvent->weights()[iw].wgt);
    }
  }
  if(EvtWeights_->size() > 0) weightsFromLhe_ = true;

  /// now get the PDF information
  edm::Handle<GenEventInfoProduct> pdfInfoHandle;
  if(iEvent.getByToken(generatorToken_, pdfInfoHandle)) {

    if (pdfInfoHandle->pdf()) {
      GPdfId1_->push_back(pdfInfoHandle->pdf()->id.first);
      GPdfId2_->push_back(pdfInfoHandle->pdf()->id.second);
      GPdfx1_->push_back(pdfInfoHandle->pdf()->x.first);
      GPdfx2_->push_back(pdfInfoHandle->pdf()->x.second);
      //pdfInfo_->push_back(pdfInfoHandle->pdf()->xPDF.first);
      //dfInfo_->push_back(pdfInfoHandle->pdf()->xPDF.second);
      GPdfScale_->push_back(pdfInfoHandle->pdf()->scalePDF);
    }
  }
}

//https://twiki.cern.ch/twiki/bin/view/CMS/MissingETOptionalFiltersRun2
void Tupel::processMETFilter(const edm::Event& iEvent){
    edm::Handle< edm::TriggerResults > metFilHandle;
    iEvent.getByToken(metFilTagToken_, metFilHandle);
	edm::Handle< edm::TriggerResults >     metFilHandleRECO;
	iEvent.getByToken(metFilTagRECOToken_, metFilHandleRECO);

    if ( metFilHandle.isValid() && !metFilHandle.failedToGet() ) {
        edm::RefProd<edm::TriggerNames> metFilNames( &(iEvent.triggerNames( *metFilHandle )) );
        int nMetFil = (int)metFilNames->size();
		//-- for MC
		// 0	Flag_HBHENoiseFilter                    # TO BE USED
		// 1	Flag_HBHENoiseIsoFilter                 # TO BE USED
		// 2	Flag_CSCTightHaloFilter
		// 3	Flag_CSCTightHaloTrkMuUnvetoFilter
		// 4	Flag_CSCTightHalo2015Filter             # TO BE USED
		// 5	Flag_HcalStripHaloFilter
		// 6	Flag_hcalLaserEventFilter
		// 7	Flag_EcalDeadCellTriggerPrimitiveFilter # TO BE USED
		// 8	Flag_EcalDeadCellBoundaryEnergyFilter
		// 9	Flag_goodVertices                       # TO BE USED
		// 10	Flag_eeBadScFilter                      # TO BE USED
		// 11	Flag_ecalLaserCorrFilter
		// 12	Flag_trkPOGFilters
		// 13	Flag_chargedHadronTrackResolutionFilter
		// 14	Flag_muonBadTrackFilter
		// 15	Flag_trkPOG_manystripclus53X
		// 16	Flag_trkPOG_toomanystripclus53X
		// 17	Flag_trkPOG_logErrorTooManyClusters
		// 18	Flag_METFilters
		
        for (int i = 0; i < nMetFil; i++) {
			//std::cout << i << " " << metFilNames->triggerName(i) << "  " <<  metFilHandle->accept(i) << " " << metFilHandle.product()->accept(i) <<  "  ";
            if (metFilHandle->accept(i)) *TrigMET_ |= 1LL <<i;
			//std::cout << *TrigMET_ << std::endl;
        }
    }
	else if ( metFilHandleRECO.isValid() && !metFilHandleRECO.failedToGet() ){
		edm::RefProd<edm::TriggerNames> metFilNames( &(iEvent.triggerNames( *metFilHandleRECO )) );
		//int nMetFil = (int)metFilNames->size();
		//-- for DATA
		// 10	Flag_HBHENoiseFilter                    # TO BE USED
		// 11	Flag_HBHENoiseIsoFilter                 # TO BE USED
		// 12	Flag_CSCTightHaloFilter
		// 13	Flag_CSCTightHaloTrkMuUnvetoFilter
		// 14	Flag_CSCTightHalo2015Filter             # TO BE USED
		// 15	Flag_HcalStripHaloFilter
		// 16	Flag_hcalLaserEventFilter
		// 17	Flag_EcalDeadCellTriggerPrimitiveFilter # TO BE USED
		// 18	Flag_EcalDeadCellBoundaryEnergyFilter
		// 19	Flag_goodVertices                       # TO BE USED
		// 20	Flag_eeBadScFilter                      # TO BE USED
		// 21	Flag_ecalLaserCorrFilter
		// 22	Flag_trkPOGFilters
		// 23	Flag_chargedHadronTrackResolutionFilter
		// 24	Flag_muonBadTrackFilter
		// 25	Flag_trkPOG_manystripclus53X
		// 26	Flag_trkPOG_toomanystripclus53X
		// 27	Flag_trkPOG_logErrorTooManyClusters
		// 28	Flag_METFilters
		
		for (int i = 10; i < 29; i++) {
			//std::cout << i << " " << metFilNames->triggerName(i) << "  " <<  metFilHandleRECO->accept(i) << " " << metFilHandleRECO.product()->accept(i) <<  "  ";
			if (metFilHandleRECO->accept(i)) *TrigMET_ |= 1LL << (i-10);
			//std::cout << *TrigMET_ << std::endl;
		}
	}
}



void Tupel::processTrigger(const edm::Event& iEvent){
  bool trigNameFilled = trigNames_.size();
  int ntrigs;
  edm::Handle< edm::TriggerResults > HLTResHandle;
  std::vector<int> trigIndexList;
  if(triggerStat_) trigIndexList.reserve(30);
  iEvent.getByToken(HLTTagToken_, HLTResHandle);  

  std::ofstream f;
  if(analyzedEventCnt_==1){
    f.open("trigger_list.txt");
    f << "List of triggers extracted from event " << iEvent.id().event()
      << " of run " << iEvent.id().run() << "\n\n";
  }
  if ( HLTResHandle.isValid() && !HLTResHandle.failedToGet() ) {
    edm::RefProd<edm::TriggerNames> trigNames( &(iEvent.triggerNames( *HLTResHandle )) );
    ntrigs = (int)trigNames->size();
    if(triggerStat_) allocateTrigMap(ntrigs);
    for (int i = 0; i < ntrigs; i++) {
      if(analyzedEventCnt_==1) f << trigNames->triggerName(i) << "\n";
      if(triggerStat_){
	if(!trigNameFilled) trigNames_[i] = trigNames->triggerName(i);
	else if(trigNames_[i] != trigNames->triggerName(i)) trigStatValid_ = false;
      }
      //insert trigger name in the acceptance map if not yet in:
      if (HLTResHandle->accept(i)){
	if(triggerStat_) trigIndexList.push_back(i);
	if(trigNames->triggerName(i).find("HLT_Mu17_Mu8") != std::string::npos) *TrigHlt_ |= kMu17_Mu8_;
	if(trigNames->triggerName(i).find("HLT_Mu17_TkMu8") != std::string::npos) *TrigHlt_ |= kMu17_TkMu8_;
	if(trigNames->triggerName(i).find("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL")!=std::string::npos) *TrigHlt_ |= kElec17_Elec8_;
	std::string thisTrigger = trigNames->triggerName(i);
	fillTrig(std::string(trigNames->triggerName(i)));
      }
    }

    if(triggerStat_){
      if(trigIndexList.size()==0) trigAccept_[0][0] += 1;
      for(std::vector<int>::iterator it1 = trigIndexList.begin();
	  it1 != trigIndexList.end();
	  ++it1){
	trigAccept_[1 + *it1][0] += 1;
	if(trigIndexList.size()==1) trigAccept_[1+*it1][1] += 1;
	for(std::vector<int>::iterator it2 = trigIndexList.begin();
	    it2 != trigIndexList.end();
	    ++it2){
	  trigAccept_[1 + *it1][2 + *it2] += 1;
	}
      }
    }
  }
}

void Tupel::fillTrig(const std::string& trigname){
  for(std::vector<TrigHltMapRcd>::iterator itTrigHltMap = trigHltMapList_.begin();
      itTrigHltMap != trigHltMapList_.end(); ++itTrigHltMap){
    const std::map<std::string, ULong64_t>& trigHltMap = *(itTrigHltMap->pMap);
    ULong64_t* pTrig = itTrigHltMap->pTrig;
    for(std::map<std::string, ULong64_t>::const_iterator it = trigHltMap.begin();
	it != trigHltMap.end(); ++it){
      if(compTrigger(it->first.c_str(), trigname.c_str()))  *pTrig |= it->second;
      //      std::cout << it->first.c_str() << ", " <<  trigname.c_str() << " -> "
      //		<< (compTrigger(it->first.c_str(), trigname.c_str()) ? "identical" : "different") << "\n";
    }
  }
}

void Tupel::processMuons(){
  double MuFill = 0;

  for (unsigned int j = 0; j < muons->size(); ++j){
    const std::vector<pat::Muon> & mu = *muons;

    if(mu[j].isGlobalMuon()){
      //const pat::TriggerObjectRef trigRef( matchHelper.triggerMatchObject( muons,j,muonMatch_, iEvent, *triggerEvent ) );
      //if ( trigRef.isAvailable() && trigRef.isNonnull() ) {
      //  Mu17_Mu8_Matched=1;
      //	}
      //	const pat::TriggerObjectRef trigRef2( matchHelper.triggerMatchObject( muons,j,muonMatch2_, iEvent, *triggerEvent ) );
      //	if ( trigRef2.isAvailable() && trigRef2.isNonnull() ) {
      //	  Mu17_TkMu8_Matched=1;
      //	}
      //TODO: filled MuHltMatch
      //patMuonIdMedium_->push_back(mu[j].isMediumMuon()); Requires CMSSW >= 4_7_2
      if(vtxx){
	unsigned bit = 0;
	unsigned muonTightIds = 0;
	for (std::vector<reco::Vertex>::const_iterator vtx = pvHandle->begin(); vtx != pvHandle->end(); ++vtx){
	  if(vtx->isValid() && !vtx->isFake() && mu[j].isTightMuon(*vtx)){
	    muonTightIds |= (1 <<bit);
	  }
	  ++bit;
	  if(bit > 31) break;
	}
	MuIdTight_->push_back(muonTightIds);
      }


      //MuFill=1;
      MuPt_->push_back(mu[j].pt());
      MuEta_->push_back(mu[j].eta());
      MuPhi_->push_back(mu[j].phi());
      MuE_->push_back(mu[j].energy());
      MuCh_->push_back(mu[j].charge());
      MuVtxZ_->push_back(mu[j].vz());
      MuDxy_->push_back(mu[j].dB());

      double trkLayers = -99999;
      double pixelHits = -99999;
      double muonHits  = -99999;
      double nMatches  = -99999;
      double normChi2  = +99999;
      double dZ = -99999;
      bool isTrackMuon =mu[j].isTrackerMuon();
      bool isGlobalMuon =mu[j].isGlobalMuon();
      if(isTrackMuon && isGlobalMuon){
	trkLayers     = mu[j].innerTrack()->hitPattern().trackerLayersWithMeasurement();
	pixelHits     = mu[j].innerTrack()->hitPattern().numberOfValidPixelHits();
	muonHits      = mu[j].globalTrack()->hitPattern().numberOfValidMuonHits();
	nMatches      = mu[j].numberOfMatchedStations();
	normChi2      = mu[j].globalTrack()->normalizedChi2();


	 if( !pvHandle->empty() && !pvHandle->front().isFake() ) {
	    const reco::Vertex &vtx = pvHandle->front();
	   dZ= mu[j].muonBestTrack()->dz(vtx.position());
  	}


      }
      MuTkNormChi2_->push_back(normChi2);
      MuTkHitCnt_->push_back(muonHits);
      MuMatchedStationCnt_->push_back(nMatches);
      MuDz_->push_back(dZ);
      MuPixelHitCnt_->push_back(pixelHits);
      MuTkLayerCnt_->push_back(trkLayers);

      bool customMuId =	( mu[j].isGlobalMuon()
			  && mu[j].isPFMuon()
			  && normChi2<10
			  && muonHits>0 && nMatches>1
			  && mu[j].dB()<0.2 && dZ<0.5
			  && pixelHits>0 && trkLayers>5 );
      unsigned muId = 0;
      if(mu[j].isLooseMuon()) muId |= kMuIdLoose_;
      //if(mu[j].isMediumMuon()) muId |= kMuIdMedium_;
      if(customMuId) muId |= kMuIdCustom_;
      MuId_->push_back(muId);

      //      unsigned muHltMath = 0;
      float muEta = mu[j].eta(); // essentially track direction at Vtx (recommended prescription)
      float Aecal=0.041; // initiallize with EE value
      float Ahcal=0.032; // initiallize with HE value
      if (fabs(muEta)<1.48) {
	Aecal = 0.074;   // substitute EB value
	Ahcal = 0.023;   // substitute EE value
      }
      double theRho = *rho;
      float muonIsoRho = mu[j].isolationR03().sumPt + std::max(0.,(mu[j].isolationR03().emEt -Aecal*(theRho))) + std::max(0.,(mu[j].isolationR03().hadEt-Ahcal*(theRho)));
      double dbeta = muonIsoRho/mu[j].pt();
      MuIsoRho_->push_back(dbeta);

      // pf Isolation variables
      double chargedHadronIso = mu[j].pfIsolationR04().sumChargedHadronPt;
      double chargedHadronIsoPU = mu[j].pfIsolationR04().sumPUPt;
      double neutralHadronIso  = mu[j].pfIsolationR04().sumNeutralHadronEt;
      double photonIso  = mu[j].pfIsolationR04().sumPhotonEt;
      double a=0.5;
      // OPTION 1: DeltaBeta corrections for iosolation
      float RelativeIsolationDBetaCorr = (chargedHadronIso + std::max(photonIso+neutralHadronIso - 0.5*chargedHadronIsoPU,0.))/std::max(a, mu[j].pt());
      MuPfIso_->push_back(RelativeIsolationDBetaCorr);
      int muType = 0;
      if(mu[j].isGlobalMuon()) muType |= kGlobMu_;
      if(mu[j].isTrackerMuon()) muType |= kTkMu_;
      if(mu[j].isPFMuon()) muType |= kPfMu_;
      MuType_->push_back(muType);
      MuIsoTkIsoAbs_->push_back(mu[j].isolationR03().sumPt);
      MuIsoTkIsoRel_->push_back(mu[j].isolationR03().sumPt/mu[j].pt());
      MuIsoCalAbs_->push_back(mu[j].isolationR03().emEt + mu[j].isolationR03().hadEt);
      MuIsoCombRel_->push_back((mu[j].isolationR03().sumPt+mu[j].isolationR03().hadEt)/mu[j].pt());
      MuPfIsoChHad_->push_back(mu[j].pfIsolationR03().sumChargedHadronPt);
      MuPfIsoNeutralHad_->push_back(mu[j].pfIsolationR03().sumNeutralHadronEt);
      MuPfIsoRawRel_->push_back((mu[j].pfIsolationR03().sumChargedHadronPt+mu[j].pfIsolationR03().sumNeutralHadronEt)/mu[j].pt());
      MuFill++;
    }
  }
}

void Tupel::processElectrons(){
  int ElecFill=0;
  std::auto_ptr<std::vector<pat::Electron> > electronColl( new std::vector<pat::Electron> (*electrons) );
  for (unsigned int j=0; j < electronColl->size();++j){
    pat::Electron & el = (*electronColl)[j];

    double dEtaIn_;
    double dPhiIn_;
    double hOverE_;
    double sigmaIetaIeta_;
    double full5x5_sigmaIetaIeta_;
    //double relIsoWithDBeta_;
    double ooEmooP_;
    double d0_ = -99.;
    double dz_ = -99.;
    int   expectedMissingInnerHits_;
    int   passConversionVeto_;

    std::vector<std::pair<std::string,float> > idlist = el.electronIDs();
    if(!elecIdsListed_) {
      std::ofstream f("electron_id_list.txt");
      f << "Autogenerated file. Electron id list.\n\n"
	"Id name: bit # (mask)\n";
      for (unsigned k  = 0 ; k < idlist.size(); ++k){
	std::map<std::string, unsigned>::const_iterator it = ElIdMap_.find(idlist[k].first);
	f << idlist[k].first << ": ";
	if(it == ElIdMap_.end()) f << "not supported by Tupel.\n";
	else f << log(it->second)/log(2) << " (0x"  << std::hex << it->second  << std::dec << ")\n";
      }

      f.close();
      elecIdsListed_ = true;
    }
    unsigned elecid = 0;
    
    for(unsigned i = 0; i < idlist.size(); ++i){
      //const int idAndConvRejectMask = 0x3;
      //if(int(idlist[i].second) & idAndConvRejectMask){
      if(int(idlist[i].second)){
	std::map<std::string, unsigned>::const_iterator it = ElIdMap_.find(idlist[i].first);
	if(it != ElIdMap_.end()){
	  elecid  |= it->second;
	  //std::cout << "Event " << analyzedEventCnt_ << ". " << idlist[i].first << ": " << idlist[i].second
	  //	    << " / "  << it->first << ": " << it->second << " -> " << elecid << "\n";
	}
      }
    }
      
    ElId_->push_back(elecid);

    dEtaIn_ = el.deltaEtaSuperClusterTrackAtVtx();
    dPhiIn_ = el.deltaPhiSuperClusterTrackAtVtx();
    hOverE_ = el.hcalOverEcal();
    sigmaIetaIeta_ = el.sigmaIetaIeta();
    full5x5_sigmaIetaIeta_ =  el.full5x5_sigmaIetaIeta();
    if( el.ecalEnergy() == 0 ){
      // printf("Electron energy is zero!\n");
      ooEmooP_ = 1e30;
    }else if( !std::isfinite(el.ecalEnergy())){
      // printf("Electron energy is not finite!\n");
      ooEmooP_ = 1e30;
    }else{
      ooEmooP_ = fabs(1.0/el.ecalEnergy() - el.eSuperClusterOverP()/el.ecalEnergy() );
    }

    ///G.N.
    //if(vtx_h->size() > 0){
    //  d0_ = (-1) * el.gsfTrack()->dxy((*vtx_h)[0].position() );
    //  dz_ = el.gsfTrack()->dz( (*vtx_h)[0].position() );
    //}

    if(pvHandle->size() > 0){
      d0_ = (-1) * el.gsfTrack()->dxy((*pvHandle)[0].position() );
      dz_ = el.gsfTrack()->dz( (*pvHandle)[0].position() );
    }


    expectedMissingInnerHits_ = el.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
    passConversionVeto_ = false;
    if( beamSpotHandle.isValid() && conversions_h.isValid()) {
      passConversionVeto_ = !ConversionTools::hasMatchedConversion(el,conversions_h,
								   beamSpotHandle->position());
    } else{
      printf("\n\nERROR!!! conversions not found!!!\n");
    }

    //std::cout << "dEtaIn_ = "  << dEtaIn_
    //	      << "\ndPhiIn_ =  " <<dPhiIn_
    //	      << "\nhOverE_ = " << hOverE_
    //	      << "\nsigmaIetaIeta_ = " << sigmaIetaIeta_
    //	      << "\nfull5x5_sigmaIetaIeta_  = " << full5x5_sigmaIetaIeta_
    //	      << "\nooEmooP_ = "<< ooEmooP_
    //	      << "\nd0_ = "<< d0_
    //	      << "\ndz_ = " << dz_
    //	      << "\nexpectedMissingInnerHits_ = " << expectedMissingInnerHits_
    //	      << "\npassConversionVeto_ = " << passConversionVeto_
    //	      << "\n";

    ElDEtaTkScAtVtx_->push_back(dEtaIn_);
    ElDPhiTkScAtVtx_->push_back(dPhiIn_);
    ElHoE_->push_back(hOverE_);
    ElSigmaIetaIeta_->push_back(sigmaIetaIeta_);
    ElSigmaIetaIetaFull5x5_->push_back(full5x5_sigmaIetaIeta_);
    ElEinvMinusPinv_->push_back(ooEmooP_);
    ElD0_->push_back(d0_);
    ElDz_->push_back(dz_);
    ElExpectedMissingInnerHitCnt_->push_back(expectedMissingInnerHits_);
    ElPassConvVeto_->push_back(passConversionVeto_);

    int hltMatch = 0;

    ElHltMatch_->push_back(hltMatch);//no matching yet...BB
    const std::string mvaTrigV0 = "mvaTrigV0";
    const std::string mvaNonTrigV0 = "mvaNonTrigV0";

    ElPt_->push_back(el.pt());
    ElEta_->push_back(el.eta());

    ElEtaSc_->push_back(el.superCluster()->eta());
    ElPhi_->push_back(el.phi());
    ElE_->push_back(el.energy());
    ElCh_->push_back(el.charge());

    ElScRawE_->push_back(el.superCluster()->rawEnergy());
    ElCorrE_->push_back(el.correctedEcalEnergy());
    ElEcalIso_->push_back(el.ecalIso());
    ElEcalPfIso_->push_back(el.ecalPFClusterIso());

    double aeff = ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03, el.superCluster()->eta(), ElectronEffectiveArea::kEleEAData2012);
    ElAEff_->push_back(aeff);

    const double chIso03_ = el.chargedHadronIso();
    const double nhIso03_ = el.neutralHadronIso();
    const double phIso03_ = el.photonIso();
    const double puChIso03_= el.puChargedHadronIso();
    ElPfIsoChHad_->push_back(chIso03_);
    ElPfIsoNeutralHad_->push_back(nhIso03_);
    ElPfIsoIso_->push_back(phIso03_);
    ElPfIsoPuChHad_->push_back(puChIso03_);
    ElPfIsoRaw_->push_back(( chIso03_ + nhIso03_ + phIso03_ ) / el.pt());
    ElPfIsoDbeta_->push_back(( chIso03_ + std::max(0.0, nhIso03_ + phIso03_ - 0.5*puChIso03_) )/ el.pt());

    *EvtFastJetRho_ =  rhoIso;
    double rhoPrime = std::max(0., rhoIso);

    ElPfIsoRho_->push_back(( chIso03_ + std::max(0.0, nhIso03_ + phIso03_ - rhoPrime*(aeff)) )/ el.pt());


    ElDr03TkSumPt_->push_back(el.dr03TkSumPt());
    ElDr03EcalRecHitSumEt_->push_back(el.dr03EcalRecHitSumEt());
    ElDr03HcalTowerSumEt_->push_back(el.dr03HcalTowerSumEt());


    bool myTrigPresel = false;
    if(fabs(el.superCluster()->eta()) < 1.479){
      if(el.sigmaIetaIeta() < 0.014 &&
	 el.hadronicOverEm() < 0.15 &&
	 el.dr03TkSumPt()/el.pt() < 0.2 &&
	 el.dr03EcalRecHitSumEt()/el.pt() < 0.2 &&
	 el.dr03HcalTowerSumEt()/el.pt() < 0.2 /*&&
						 el.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits() == 0*/)
	myTrigPresel = true;
    }
    else {
      if(el.sigmaIetaIeta() < 0.035 &&
	 el.hadronicOverEm() < 0.10 &&
	 el.dr03TkSumPt()/el.pt() < 0.2 &&
	 el.dr03EcalRecHitSumEt()/el.pt() < 0.2 &&
	 el.dr03HcalTowerSumEt()/el.pt() < 0.2 /*&&
						 el.gsfTrack()->trackerExpectedHitsInner().numberOfLostHits() == 0*/)
	myTrigPresel = true;
    }
    ElMvaPresel_->push_back(myTrigPresel);
    ElecFill++;
  }
}

//Modified by Clement Leloup
void Tupel::processTaus(){

  std::auto_ptr<std::vector<pat::Tau> > tauColl( new std::vector<pat::Tau> (*taus) );
  for (unsigned int j=0; j < tauColl->size();++j){
    pat::Tau & ta = (*tauColl)[j];

    TauPt_->push_back(ta.pt());
    TauEta_->push_back(ta.eta());
    TauPhi_->push_back(ta.phi());
    TauE_->push_back(ta.energy());
    TauCh_->push_back(ta.charge());
    TauDecayModeFinding_->push_back(ta.tauID("decayModeFinding"));
    TauCombinedIsolationDeltaBetaCorrRaw3Hits_->push_back(ta.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits"));
    TauDiscMuonLoose_->push_back(ta.tauID("againstMuonLoose3"));
    TauDiscMuonTight_->push_back(ta.tauID("againstMuonTight3"));
    TauDiscElVLoose_->push_back(ta.tauID("againstElectronVLooseMVA6"));
    TauDiscElLoose_->push_back(ta.tauID("againstElectronLooseMVA6"));
    TauDiscElVTight_->push_back(ta.tauID("againstElectronVTightMVA6"));
    TauDiscElTight_->push_back(ta.tauID("againstElectronTightMVA6"));

  }
}

void Tupel::processJets(){
  //double PFjetFill=0;
  double chf = 0;
  double nhf = 0;
  double cemf = 0;
  double nemf = 0;
  double cmult = 0;
  double nconst = 0;
    
  for ( unsigned int i=0; i<jets->size(); ++i ) {
    const pat::Jet & jet = jets->at(i);

    if(i==0 && analyzedEventCnt_== 1){
      std::cout << "Jet user float list:\n";
      for(unsigned j = 0; j < jet.userFloatNames().size(); ++j){
	std::cout << jet.userFloatNames()[j] << "\n";
      }
      std::cout << std::endl;
    }

    if(jet.hasUserFloat(puMvaName_)) JetAk04PuMva_->push_back(jet.userFloat(puMvaName_));
    else JetAk04PuMva_->push_back(-99.);
    //else cerr << "No PU MVA " << analyzedEventCnt_ << ". " << i << "\n";

    if(puJetIds_){
      const StoredPileupJetIdentifier& thisJetIds = (*puJetIds_)[jets->refAt(i)->originalObjectRef()];
      JetAk04Beta_->push_back(           thisJetIds.beta());
      JetAk04BetaClassic_->push_back(    thisJetIds.betaStar());
      JetAk04BetaStar_->push_back(       thisJetIds.betaClassic());
      JetAk04BetaStarClassic_->push_back(thisJetIds.betaStarClassic());
      JetAk04Rms_->push_back(thisJetIds.dR2Mean());      
    }

    chf = jet.chargedHadronEnergyFraction();
    nhf = (jet.neutralHadronEnergy()+jet.HFHadronEnergy())/jet.correctedJet(0).energy();
    cemf = jet.chargedEmEnergyFraction();
    nemf = jet.neutralEmEnergyFraction();
    cmult = jet.chargedMultiplicity();
    nconst = jet.numberOfDaughters();

    // cout<<"jet.bDiscriminator(combinedSecondaryVertexBJetTags)=  "<<jet.bDiscriminator("combinedSecondaryVertexBJetTags")<<endl;
    //  cout<<"jet.bDiscriminator(combinedSecondaryVertexV1BJetTags)=  "<<jet.bDiscriminator("combinedSecondaryVertexV1BJetTags")<<endl;
    //  cout<<"jet.bDiscriminator(combinedSecondaryVertexSoftPFLeptonV1BJetTags)=  "<<jet.bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags")<<endl;
    JetAk04BTagCsv_->push_back(jet.bDiscriminator("combinedSecondaryVertexBJetTags"));
    JetAk04BTagCsvV1_->push_back(jet.bDiscriminator("combinedSecondaryVertexV1BJetTags"));
    JetAk04BTagCsvSLV1_->push_back(jet.bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags"));
    JetAk04BDiscCisvV2_->push_back(jet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
    JetAk04BDiscJp_->push_back(jet.bDiscriminator("pfJetProbabilityBJetTags"));
    JetAk04BDiscBjp_->push_back(jet.bDiscriminator("pfJetBProbabilityBJetTags"));
    JetAk04BDiscTche_->push_back(jet.bDiscriminator("pfTrackCountingHighEffBJetTags"));
    JetAk04BDiscTchp_->push_back(jet.bDiscriminator("pfTrackCountingHighPurBJetTags"));
    JetAk04BDiscSsvhe_->push_back(jet.bDiscriminator("pfSimpleSecondaryVertexHighEffBJetTags"));
    JetAk04BDiscSsvhp_->push_back(jet.bDiscriminator("pfSimpleSecondaryVertexHighPurBJetTags"));
    JetAk04PartFlav_->push_back(jet.partonFlavour());
    JetAk04HadFlav_->push_back(jet.hadronFlavour());

    //cout << " JetAk04HadFlav_ " << jet.hadronFlavour() << " " << jet.partonFlavour() << endl;
    //cout << " jet.pt() " << jet.pt() << " corr " << jet.pt()/jet.correctedJet(0).pt() << " jet.correctedJet(0).pt() " << jet.correctedJet(0).pt() << " " << eta(jet) << endl;
      
    JetAk04E_->push_back(jet.energy());
    JetAk04Pt_->push_back(jet.pt());
    JetAk04Eta_->push_back(jet.eta());
    JetAk04Phi_->push_back(jet.phi());
    JetAk04RawPt_->push_back(jet.correctedJet(0).pt());
    JetAk04RawE_->push_back(jet.correctedJet(0).energy());
    JetAk04HfHadE_->push_back(jet.HFHadronEnergy());
    JetAk04HfEmE_->push_back(jet.HFEMEnergy());
    JetAk04ChHadFrac_->push_back(chf);
    JetAk04NeutralHadAndHfFrac_->push_back(nhf);
    JetAk04ChEmFrac_->push_back(cemf);
    JetAk04NeutralEmFrac_->push_back(nemf);
    JetAk04ChMult_->push_back(cmult);
    JetAk04ConstCnt_->push_back(nconst);

    for(unsigned int idx =0; idx<jet.numberOfDaughters();idx++){
      //cout<<jet.numberOfDaughters()<<" RECO AHMEEEEET "<<idx<<"  "<<jet.daughter(idx)->pdgId()<<"  "<<endl;
      JetAk04ConstId_->push_back(jet.daughter(idx)->pdgId());
      JetAk04ConstPt_->push_back(jet.daughter(idx)->pt());
      JetAk04ConstEta_->push_back(jet.daughter(idx)->eta());
      JetAk04ConstPhi_->push_back(jet.daughter(idx)->phi());
      JetAk04ConstE_->push_back(jet.daughter(idx)->energy());


    }
    //TODO: insert correct value
    double unc = 1.;
    JetAk04JecUncUp_->push_back(unc);
    JetAk04JecUncDwn_->push_back(unc);
    double tempJetID=0;
    if( abs(jet.eta())<2.4){
      if(chf>0 && nhf<0.99 && cmult>0.0 && cemf<0.99 && nemf<0.99 && nconst>1) tempJetID=1;
      if((chf>0)&&(nhf<0.95)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.95)&&(nconst>1)) tempJetID=2;
      if((chf>0)&&(nhf<0.9)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.9)&&(nconst>1)) tempJetID=3;
    }
    if( abs(jet.eta())>=2.4){
      if ((nhf<0.99)&&(nemf<0.99)&&(nconst>1))tempJetID=1;
      if ((nhf<0.95)&&(nemf<0.95)&&(nconst>1))tempJetID=2;
      if ((nhf<0.9)&&(nemf<0.9)&&(nconst>1))tempJetID=3;
    }
    JetAk04Id_->push_back(tempJetID);//ala
    //PFjetFill++;

    if(!*EvtIsRealData_){
      int genJetIdx = -1; //code for not matched jets
      if (jet.genJet()){
	//	genJetIdx = jet.genJetFwdRef().key();
	for(genJetIdx = 0; (unsigned) genJetIdx < genjetColl_->size();++genJetIdx){
	  if(&(*genjetColl_)[genJetIdx] == jet.genJet()) break;
	}
	genJetIdx = jet.genJetFwdRef().backRef().key();
	assert(jet.genJet() == &(*genjetColl_)[genJetIdx]);
	//	if((unsigned)genJetIdx == genjetColl_->size()){
	//	  genJetIdx = -2;
	//	  std::cerr << "Matched gen jet not found!\n";
	//	} else{
	//	  double dphi = fabs(jet.phi()-(*genjetColl_)[genJetIdx].phi());
	//	  if(dphi > pi ) dphi = 2*pi - dphi;
	//	  std::cout << "Jet matching check: R = "
	//		    << sqrt(std::pow(eta(jet)-(*genjetColl_)[genJetIdx].eta(),2)
	//			    + std::pow(dphi,2))
	//		    << "\tPt ratio = "
	//		    << jet.pt() / (*genjetColl_)[genJetIdx].pt()
	//		    << "\t" << jet.pt() / jet.genJet()->pt()
	//		    << "\t key: " << genJetIdx
	//		    << "\t coll size: " <<  genjetColl_->size()
	//		    << "\t" << hex << jet.genJet()
	//		    << "\t" << &((*genjetColl_)[genJetIdx]) << dec
	//		    << "\tIndices: " << genJetIdx << ", " << jet.genJetFwdRef().backRef().key() << ", " << jet.genJetFwdRef().ref().key() << ", " << jet.genJetFwdRef().key()
	//		    << "\n";
	//}
      }
      JetAk04GenJet_->push_back(genJetIdx);
    }
  }
}

//Modified by Clement Leloup
void Tupel::processFatJets(){

  //double PFjetFill=0;
  double chf = 0;
  double nhf = 0;
  double cemf = 0;
  double nemf = 0;
  double cmult = 0;
  double nconst = 0;

  for ( unsigned int i=0; i<hFatJets->size(); ++i ) {
    const pat::Jet & fatjet = hFatJets->at(i);

    if(i==0 && analyzedEventCnt_== 1){
      std::cout << "Jet user float list:\n";
      for(unsigned j = 0; j < fatjet.userFloatNames().size(); ++j){
	std::cout << fatjet.userFloatNames()[j] << "\n";
      }
      std::cout << std::endl;
    }

    chf = fatjet.chargedHadronEnergyFraction();
    nhf = (fatjet.neutralHadronEnergy()+fatjet.HFHadronEnergy())/fatjet.correctedJet(0).energy();
    cemf = fatjet.chargedEmEnergyFraction();
    nemf = fatjet.neutralEmEnergyFraction();
    cmult = fatjet.chargedMultiplicity();
    nconst = fatjet.numberOfDaughters();

    JetAk08BTagCsv_->push_back(fatjet.bDiscriminator("combinedSecondaryVertexBJetTags"));
    JetAk08BTagCsvV1_->push_back(fatjet.bDiscriminator("combinedSecondaryVertexV1BJetTags"));

    JetAk08BTagCsvSLV1_->push_back(fatjet.bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags"));
    JetAk08BDiscCisvV2_->push_back(fatjet.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));

    JetAk08BDiscJp_->push_back(fatjet.bDiscriminator("pfJetProbabilityBJetTags"));
    JetAk08BDiscBjp_->push_back(fatjet.bDiscriminator("pfJetBProbabilityBJetTags"));
    JetAk08BDiscTche_->push_back(fatjet.bDiscriminator("pfTrackCountingHighEffBJetTags"));
    
    JetAk08BDiscTchp_->push_back(fatjet.bDiscriminator("pfTrackCountingHighPurBJetTags"));
    JetAk08BDiscSsvhe_->push_back(fatjet.bDiscriminator("pfSimpleSecondaryVertexHighEffBJetTags"));
    JetAk08BDiscSsvhp_->push_back(fatjet.bDiscriminator("pfSimpleSecondaryVertexHighPurBJetTags"));
    JetAk08PartFlav_->push_back(fatjet.partonFlavour());
    JetAk08HadFlav_->push_back(fatjet.hadronFlavour());

    //cout << " JetAk04HadFlav_ " << jet.hadronFlavour() << " " << jet.partonFlavour() << endl;
    //cout << " jet.pt() " << jet.pt() << " corr " << jet.pt()/jet.correctedJet(0).pt() << " jet.correctedJet(0).pt() " << jet.correctedJet(0).pt() << " " << eta(jet) << endl;
      
    JetAk08E_->push_back(fatjet.energy());
    JetAk08Pt_->push_back(fatjet.pt());
    JetAk08Eta_->push_back(fatjet.eta());
    JetAk08Phi_->push_back(fatjet.phi());
    JetAk08RawPt_->push_back(fatjet.correctedJet(0).pt());
    JetAk08RawE_->push_back(fatjet.correctedJet(0).energy());
    JetAk08HfHadE_->push_back(fatjet.HFHadronEnergy());
    JetAk08HfEmE_->push_back(fatjet.HFEMEnergy());
    JetAk08ChHadFrac_->push_back(chf);
    JetAk08NeutralHadAndHfFrac_->push_back(nhf);
    JetAk08ChEmFrac_->push_back(cemf);
    JetAk08NeutralEmFrac_->push_back(nemf);
    JetAk08ChMult_->push_back(cmult);
    JetAk08ConstCnt_->push_back(nconst);

    for(unsigned int idx =0; idx<fatjet.numberOfDaughters();idx++){
      //cout<<jet.numberOfDaughters()<<" RECO AHMEEEEET "<<idx<<"  "<<jet.daughter(idx)->pdgId()<<"  "<<endl;
      JetAk08ConstId_->push_back(fatjet.daughter(idx)->pdgId());
      JetAk08ConstPt_->push_back(fatjet.daughter(idx)->pt());
      JetAk08ConstEta_->push_back(fatjet.daughter(idx)->eta());
      JetAk08ConstPhi_->push_back(fatjet.daughter(idx)->phi());
      JetAk08ConstE_->push_back(fatjet.daughter(idx)->energy());
    }

    //TODO: insert correct value
    double unc = 1.;
    JetAk08JecUncUp_->push_back(unc);
    JetAk08JecUncDwn_->push_back(unc);
    double tempJetID=0;
    if( abs(fatjet.eta())<2.4){

      if(chf>0 && nhf<0.99 && cmult>0.0 && cemf<0.99 && nemf<0.99 && nconst>1) tempJetID=1;
      if((chf>0)&&(nhf<0.95)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.95)&&(nconst>1)) tempJetID=2;
      if((chf>0)&&(nhf<0.9)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.9)&&(nconst>1)) tempJetID=3;
    }
    if( abs(fatjet.eta())>=2.4){
      if ((nhf<0.99)&&(nemf<0.99)&&(nconst>1))tempJetID=1;
      if ((nhf<0.95)&&(nemf<0.95)&&(nconst>1))tempJetID=2;
      if ((nhf<0.9)&&(nemf<0.9)&&(nconst>1))tempJetID=3;
    }

    JetAk08Id_->push_back(tempJetID);//ala
    //PFjetFill++;

    if(!*EvtIsRealData_){
      int genFatJetIdx = -1; //code for not matched jets
      if (fatjet.genJet()){
	//	genJetIdx = jet.genJetFwdRef().key();
	for(genFatJetIdx = 0; (unsigned) genFatJetIdx < genfatJetColl_->size();++genFatJetIdx){
	  if(&(*genfatJetColl_)[genFatJetIdx] == fatjet.genJet()) break;
	}
	genFatJetIdx = fatjet.genJetFwdRef().backRef().key();
	assert(fatjet.genJet() == &(*genfatJetColl_)[genFatJetIdx]);
	//	if((unsigned)genJetIdx == genjetColl_->size()){
	//	  genJetIdx = -2;
	//	  std::cerr << "Matched gen jet not found!\n";
	//	} else{
	//	  double dphi = fabs(jet.phi()-(*genjetColl_)[genJetIdx].phi());
	//	  if(dphi > pi ) dphi = 2*pi - dphi;
	//	  std::cout << "Jet matching check: R = "
	//		    << sqrt(std::pow(jet.eta()-(*genjetColl_)[genJetIdx].eta(),2)
	//			    + std::pow(dphi,2))
	//		    << "\tPt ratio = "
	//		    << jet.pt() / (*genjetColl_)[genJetIdx].pt()
	//		    << "\t" << jet.pt() / jet.genJet()->pt()
	//		    << "\t key: " << genJetIdx
	//		    << "\t coll size: " <<  genjetColl_->size()
	//		    << "\t" << hex << jet.genJet()
	//		    << "\t" << &((*genjetColl_)[genJetIdx]) << dec
	//		    << "\tIndices: " << genJetIdx << ", " << jet.genJetFwdRef().backRef().key() << ", " << jet.genJetFwdRef().ref().key() << ", " << jet.genJetFwdRef().key()
	//		    << "\n";
	//}
      }
      JetAk08GenJet_->push_back(genFatJetIdx);
    }

    JetAk08PrunedMass_->push_back(fatjet.userFloat("ak8PFJetsCHSPrunedMass"));
    //JetAk08FilteredMass_->push_back(fatjet.userFloat("ak8PFJetsCHSFilteredMass"));
    JetAk08SoftDropMass_->push_back(fatjet.userFloat("ak8PFJetsCHSSoftDropMass"));
    //JetAk08TrimmedMass_->push_back(fatjet.userFloat("ak8PFJetsCHSTrimmedMass"));
    JetAk08Tau1_->push_back(fatjet.userFloat("NjettinessAK8:tau1"));
    JetAk08Tau2_->push_back(fatjet.userFloat("NjettinessAK8:tau2"));
    JetAk08Tau3_->push_back(fatjet.userFloat("NjettinessAK8:tau3"));

  }
}


void Tupel::processPfCands()
{
  for (unsigned j = 0; j < candidates->size(); ++j){
    const pat::PackedCandidate& cand = (*candidates)[j];
    const reco::Track* track = cand.bestTrack();

    if(candidateMode_ == candidatesWithTrack && !track) continue;
    
    double tDxy_ = -99.;
    double tDz_ = -99.;
    
    if(pvHandle->size() > 0){
      tDxy_ = (-1) * cand.dxy( (*pvHandle)[0].position() );
      tDz_ = cand.dz( (*pvHandle)[0].position() );
    }

    PfCandPt_->push_back(cand.pt());
    PfCandEta_->push_back(cand.eta());
    PfCandPhi_->push_back(cand.phi());
    PfCandE_->push_back(cand.energy());
    PfCandVx_->push_back(cand.vx());
    PfCandVy_->push_back(cand.vy());
    PfCandVz_->push_back(cand.vz());
    PfCandDz_->push_back(tDz_);
    PfCandDxy_->push_back(tDxy_);
    PfCandDzPV_->push_back(cand.dzAssociatedPV());
    PfCandDxyPV_->push_back(cand.dxy());
    if(track){
      PfCandTkInPt_->push_back(track->pt());
      PfCandTkInEta_->push_back(track->eta());
      PfCandTkInPhi_->push_back(track->phi());
      PfCandTkValidHitCnt_->push_back(track->numberOfValidHits());
      PfCandTkValidFrac_->push_back(track->validFraction());
      PfCandTkChi2_->push_back(track->normalizedChi2());
    }
  }
}


void Tupel::processPhotons(const edm::Event& iEvent, const edm::EventSetup& iSetup){

  EcalClusterLazyTools lazyTool(iEvent, iSetup, ecalHitEBToken_, ecalHitEEToken_, ecalHitESToken_ );
  std::auto_ptr<std::vector<pat::Electron> > electronColl( new std::vector<pat::Electron> (*electrons) );

  for (unsigned j = 0; j < photons->size(); ++j){

    const pat::Photon& photon = (*photons)[j];

    const reco::CaloClusterPtr  seed_clu = photon.superCluster()->seed();

    int passElectronVeto_;

    //photon momentum
    PhotPt_->push_back(photon.pt());
    PhotEta_->push_back(photon.eta());
    PhotPhi_->push_back(photon.phi());
    PhotScRawE_->push_back(photon.superCluster()->rawEnergy());
    PhotScEta_->push_back(photon.superCluster()->eta());
    PhotScPhi_->push_back(photon.superCluster()->phi());

    //photon isolation
    PhotIsoEcal_->push_back(photon.ecalIso());
    PhotIsoHcal_->push_back(photon.hcalIso());
    PhotIsoTk_->push_back(photon.trackIso());
    PhotPfIsoChHad_->push_back(photon.chargedHadronIso());
    PhotPfIsoNeutralHad_->push_back(photon.neutralHadronIso());
    PhotPfIsoPhot_->push_back(photon.photonIso());
    PhotPfIsoPuChHad_->push_back(photon.puChargedHadronIso());
    PhotPfIsoEcalClus_->push_back(photon.ecalPFClusterIso());
    PhotPfIsoHcalClus_->push_back(photon.hcalPFClusterIso());

    //photon cluster shape
    PhotE3x3_->push_back(photon.e3x3());
    PhotE1x5_->push_back(photon.e1x5());
    PhotE1x3_->push_back( lazyTool.e1x3( *seed_clu ) );
    PhotE2x2_->push_back( lazyTool.e2x2( *seed_clu ) );
    PhotE2x5_->push_back(photon.e2x5());
    PhotE5x5_->push_back(photon.e5x5());
    PhotSigmaIetaIeta_->push_back(photon.sigmaIetaIeta());
    //PhotSigmaIetaIphi_->push_back(...);
    //PhotSigmaIphiIphi_->push_back(...);

    PhotEtaWidth_->push_back(photon.superCluster()->etaWidth());
    PhotPhiWidth_->push_back(photon.superCluster()->phiWidth());
    PhotR9_->push_back(photon.r9());
    PhotS4_->push_back( lazyTool.e2x2( *seed_clu ) / lazyTool.e5x5( *seed_clu ) );


    PhotE1x5Full5x5_->push_back(photon.full5x5_e1x5());    
    PhotE2x5Full5x5_->push_back(photon.full5x5_e2x5());
    PhotE3x3Full5x5_->push_back(photon.full5x5_e3x3());
    PhotE5x5Full5x5_->push_back(photon.full5x5_e5x5());
    PhotSigmaIetaIetaFull5x5_->push_back(photon.full5x5_sigmaIetaIeta());
    PhotR9Full5x5_->push_back(photon.full5x5_r9());   

    //photon ids:
    std::vector<std::pair<std::string,Bool_t> > idlist = photon.photonIDs();
    if(!photonIdsListed_) {
      std::ofstream f("photon_id_list.txt");
      f << "Autogenerated file. Photon id list\n\n"
	"Id name: bit # (mask)\n";
      for (unsigned k  = 0 ; k < idlist.size(); ++k){
	std::map<std::string, unsigned>::const_iterator it = PhotIdMap_.find(idlist[k].first);
	f << idlist[k].first << ": ";
	if(it == PhotIdMap_.end()) f << "not supported by Tupel\n";
	else f << log(it->second)/log(2) << " (0x"  << std::hex << it->second  << std::dec << ")\n";
      }
      f.close();
      photonIdsListed_ = true;
    }

    int photonId = 0;
    //    if(photon.photonID(std::string("PhotonCutBasedIDLoose"))) photonId |= 1;
    //    if(photon.photonID(std::string("PhotonCutBasedIDTight"))) photonId |= 4;

    for(unsigned i = 0; i < idlist.size(); ++i){
      if(int(idlist[i].second)){
	std::map<std::string, unsigned>::const_iterator it = PhotIdMap_.find(idlist[i].first);
	if(it != PhotIdMap_.end()){
	  photonId  |= it->second;
	  //std::cout << "Event " << analyzedEventCnt_ << ". " << idlist[i].first << ": " << idlist[i].second
	  //          << " / "  << it->first << ": " << it->second << " -> " << photonId << "\n";
	}
      }
    }

    PhotId_->push_back(photonId);



    PhotHoE_->push_back(photon.hadronicOverEm());
    PhotHasPixelSeed_->push_back(photon.hasPixelSeed());
   
    //Modified by Clement Leloup
    passElectronVeto_ = true;
    if( beamSpotHandle.isValid() && conversions_h.isValid()) {
      if (!photon.superCluster().isNull()){
	for (unsigned int j=0; j < electronColl->size();++j){
	  pat::Electron & el = (*electronColl)[j];

	  if(el.superCluster()!=photon.superCluster()) continue;
	  //if(el.gsfTrack()->trackerExpectedHitsInner().numberOfHits()>0) continue;
	  if(el.gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS)>0) continue;
	  if(ConversionTools::hasMatchedConversion(el, conversions_h, beamSpotHandle->position())) continue;

	  passElectronVeto_ = false;
	}
      }
    }else{
      printf("\n\nERROR!!! conversions not found!!!\n");
    }

    PhotPassElVeto_->push_back((int)passElectronVeto_); 

  }
}

void Tupel::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  ++analyzedEventCnt_;

  //std::cout << " ------ New Event ----- "  << std::endl;
  readEvent(iEvent);

  // PAT trigger EvtNum
  //edm::Handle<pat::TriggerEvent>  triggerEvent;
  //iEvent.getByLabel( triggerEvent_, triggerEvent );

  if(!*EvtIsRealData_){
    processGenJets(iEvent);
      if (genParticles) processGenParticles(iEvent);
      processPdfInfo(iEvent);
      //Modified by Clement Leloup
      if(fatJetMode_ == fatJetOn){
	processGenFatJets(iEvent);
      }
  }

  if(recoSw_==std::string("on")){
    processMET(iEvent);
    
    processVtx();
    
    if (!*EvtIsRealData_){
      processPu(iEvent);       
    }
    
    processTrigger(iEvent);
    
    processMETFilter(iEvent);
    
    if(muon) processMuons();
    
    //electrons B.B.
    if(electron) processElectrons();
    
    //Modified by Clement Leloup
    if(tau) processTaus();
    
    //jets
    if(jettt) processJets();
    
    //Modified by Clement Leloup
    //fat jets
    if(fatjettt) processFatJets();

    //photons. Ph. G.
    //if(photons) processPhotons(); 
    if(photons) processPhotons(iEvent, iSetup);
    
    //tracks G.N.
    if(candidates) processPfCands();
  }

  //Stores the EvtNum in the output tree
  treeHelper_->fill();

}

void
Tupel::writeHeader(){
  TTree* t = new TTree("Header", "Header");
  TString checksum_(checksum);
  t->Branch("Tupel_cc_githash", &checksum_);

  TString cmssw_release(edm::getReleaseVersion().c_str());
  t->Branch("CMSSW_RELEASE", &cmssw_release);

  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  hostname[sizeof(hostname)-1] = 0;
  TString hostname_(hostname);
  t->Branch("Hostname", &hostname_);

  edm::TimeOfDay ts;
  std::stringstream s;
  s << std::setprecision(0) << ts;
  TString ts_(s.str().c_str());
  t->Branch("CreationTime", &ts_);

  t->Fill();
}

void
Tupel::beginJob()
{
  //  jecUnc = new JetCorrectionUncertainty("Fall12_V7_DATA_Uncertainty_AK5PFchs.txt");
  // register to the TFileService
  edm::Service<TFileService> fs;
  TFileDirectory TestDir = fs->mkdir("test");

  writeHeader();

  myTree = new TTree("EventTree"," EventTree");
  treeHelper_ = std::auto_ptr<TreeHelper>
    (new TreeHelper(myTree, new TTree("Description", "Description"),
		    new TTree("BitFields", "BitFields")));

  //Event
  ADD_BRANCH_D(EvtIsRealData, "True if real data, false if MC");
  ADD_BRANCH_D(EvtNum, "Event number");
  ADD_BRANCH_D(EvtRunNum, "Run number");
  ADD_BRANCH_D(EvtLumiNum, "Luminosity block number");
  ADD_BRANCH_D(EvtBxNum, "Bunch crossing number");
  ADD_BRANCH_D(EvtVtxCnt, "Number of reconstructed primary vertices");
  ADD_BRANCH_D(EvtPuCnt, "Number of measured pile-up events");
  ADD_BRANCH_D(EvtPuCntTruth, "True number of pile-up events");
  ADD_BRANCH(EvtWeights); //description filled in endRun()
  ADD_BRANCH_D(EvtFastJetRho, "Fastjet pile-up variable \\rho");
    
  ADD_BRANCH(TrigMET);

  //Trigger
  ADD_BRANCH_D(TrigHlt, "HLT triggger bits. See BitField.TrigHlt for bit description.");
  ADD_BRANCH_D(TrigHltPhot, "HLT Photon triggger bits. See BitField.TrigHltPhot for bit description.");
  ADD_BRANCH_D(TrigHltDiPhot, "HLT Diphoton triggger bits. See BitField.TrigHltDiPhot for bit description.");  
  ADD_BRANCH_D(TrigHltMu, "HLT Muon triggger bits. See BitField.TrigHltMu for bit description.");
  ADD_BRANCH_D(TrigHltDiMu, "HLT Dimuon triggger bits. See BitField.TrigHltDiMu for bit description.");
  ADD_BRANCH_D(TrigHltEl, "HLT Electron triggger bits. See BitField.TrigHltEl for bit description.");
  ADD_BRANCH_D(TrigHltDiEl, "HLT Dielectron triggger bits. See BitField.TrigHltDiEl for bit description.");
  ADD_BRANCH_D(TrigHltElMu, "HLT Muon + Electron triggger bits. See BitField.TrigHltElMu for bit description.");

  //Missing Energy
  treeHelper_->addDescription("MET", "PF MET");
  ADD_BRANCH(METPt);
  ADD_BRANCH(METPx);
  ADD_BRANCH(METPy);
  ADD_BRANCH(METPz);
  ADD_BRANCH(METE);
  ADD_BRANCH(METsigx2);
  ADD_BRANCH(METsigxy);
  ADD_BRANCH(METsigy2);
  ADD_BRANCH(METsig);

  //Generator MET
  treeHelper_->addDescription("MET", "Generator level MET");
  ADD_BRANCH(GMETPt);
  ADD_BRANCH(GMETPx);
  ADD_BRANCH(GMETPy);
  ADD_BRANCH(GMETPz);
  ADD_BRANCH(GMETE);

  //Generator level leptons.
  treeHelper_->addDescription("GLepDr01", "Generator-level leptons. Muons and electrons and their antiparticles are dressed using a cone of radius R = 0.1");
  ADD_BRANCH(GLepDr01Pt);
  ADD_BRANCH(GLepDr01Eta);
  ADD_BRANCH(GLepDr01Phi);
  ADD_BRANCH(GLepDr01E);
  ADD_BRANCH(GLepDr01Id);
  ADD_BRANCH(GLepDr01St);
  ADD_BRANCH(GLepDr01MomId);
  ADD_BRANCH(GLepDr01Prompt);
  ADD_BRANCH(GLepDr01TauProd);  
  treeHelper_->addDescription("GLepBare", "Generator-level leptons, status 1 without dressing.");
  ADD_BRANCH(GLepBarePt);
  ADD_BRANCH(GLepBareEta);
  ADD_BRANCH(GLepBarePhi);
  ADD_BRANCH(GLepBareE);
  ADD_BRANCH(GLepBareId);
  ADD_BRANCH(GLepBareSt);
  ADD_BRANCH(GLepBareMomId);
  ADD_BRANCH(GLepBarePrompt);
  ADD_BRANCH(GLepBareTauProd);  
  treeHelper_->addDescription("GLepDr01", "Status 3 generator-level leptons.");
  ADD_BRANCH(GLepSt3Pt);
  ADD_BRANCH(GLepSt3Eta);
  ADD_BRANCH(GLepSt3Phi);
  ADD_BRANCH(GLepSt3E);
  ADD_BRANCH(GLepSt3Id);
  ADD_BRANCH(GLepSt3St);
  ADD_BRANCH_D(GLepSt3Mother0Id, "Lepton mother PDG Id. Filled only for first mother. Number of mothers can be checked in GLepoSt3MotherCnt.");
  ADD_BRANCH(GLepSt3MotherCnt);
  //Generator level photons.
  treeHelper_->addDescription("GPhot", "Generator-level photons.");
  ADD_BRANCH(GPhotPt);
  ADD_BRANCH(GPhotEta);
  ADD_BRANCH(GPhotPhi);
  ADD_BRANCH(GPhotE);
  ADD_BRANCH_D(GPhotMotherId, "Photon mother PDG Id. Filled only for first mother.");
  ADD_BRANCH(GPhotSt);
  ADD_BRANCH(GPhotIsoEDR03);
  ADD_BRANCH(GPhotIsoEDR04);
  ADD_BRANCH(GPhotIsoEDR05);
  ADD_BRANCH(GPhotIsoSumPtDR03);
  ADD_BRANCH(GPhotIsoSumPtDR04);
  ADD_BRANCH(GPhotIsoSumPtDR05);

  //Photons in the vicinity of the leptons
  treeHelper_->addDescription("GLepClosePhot", "Photons aroud leptons. Selection cone size: R = 0.2");
  ADD_BRANCH(GLepClosePhotPt);
  ADD_BRANCH(GLepClosePhotEta);
  ADD_BRANCH(GLepClosePhotPhi);
  ADD_BRANCH(GLepClosePhotE);
  ADD_BRANCH(GLepClosePhotId);
  ADD_BRANCH_D(GLepClosePhotMother0Id, "Photon mother PDG Id. Filled only for first mother. Number of mothers can be checked in GLepoSt3MotherCnt.");
  ADD_BRANCH(GLepClosePhotMotherCnt);
  ADD_BRANCH(GLepClosePhotSt);

  //Gen Jets
  treeHelper_->addDescription("GJetAk04", "Generator-level reconstructed with the anti-kt algorithm with distance parameter R = 0.4");
  ADD_BRANCH(GJetAk04Pt);
  ADD_BRANCH(GJetAk04Eta);
  ADD_BRANCH(GJetAk04Phi);
  ADD_BRANCH(GJetAk04E);
  ADD_BRANCH(GJetAk04ChFrac);
  ADD_BRANCH(GJetAk04ConstCnt);
  ADD_BRANCH(GJetAk04ConstId);
  ADD_BRANCH(GJetAk04ConstPt);
  ADD_BRANCH(GJetAk04ConstEta);
  ADD_BRANCH(GJetAk04ConstPhi);
  ADD_BRANCH(GJetAk04ConstE);
  ADD_BRANCH(GJetAk04MatchedPartonID);
  ADD_BRANCH(GJetAk04MatchedPartonDR);

  //Modified by Clement Leloup
  //Gen Fat Jets
  if(fatJetMode_ == fatJetOn){
    treeHelper_->addDescription("GJetAk08", "Generator-level reconstructed with the anti-kt algorithm with distance parameter R = 0.8");
    ADD_BRANCH(GJetAk08Pt);
    ADD_BRANCH(GJetAk08Eta);
    ADD_BRANCH(GJetAk08Phi);
    ADD_BRANCH(GJetAk08E);
    ADD_BRANCH(GJetAk08ChFrac);
    ADD_BRANCH(GJetAk08ConstCnt);
    ADD_BRANCH(GJetAk08ConstId);
    ADD_BRANCH(GJetAk08ConstPt);
    ADD_BRANCH(GJetAk08ConstEta);
    ADD_BRANCH(GJetAk08ConstPhi);
    ADD_BRANCH(GJetAk08ConstE);
    ADD_BRANCH(GJetAk08MatchedPartonID);
    ADD_BRANCH(GJetAk08MatchedPartonDR);
  }

  //Exta generator information
  ADD_BRANCH_D(GPdfId1, "PDF Id for beam 1");
  ADD_BRANCH_D(GPdfId2, "PDF Id for beam 2");
  ADD_BRANCH_D(GPdfx1, "PDF x for beam 1");
  ADD_BRANCH_D(GPdfx2, "PDF x for beam 1");
  ADD_BRANCH_D(GPdfScale, "PDF energy scale");
  ADD_BRANCH_D(GBinningValue, "Value of the observable used to split the MC sample generation (e.g. pt_hat for a pt_hat binned MC sample).");
  ADD_BRANCH_D(GNup, "Number of particles/partons included in the matrix element.");

  //Muons
  treeHelper_->addDescription("Mu", "PF reconstruced muons.");
  ADD_BRANCH(MuPt);
  ADD_BRANCH(MuEta);
  ADD_BRANCH(MuPhi);
  ADD_BRANCH(MuE);
  ADD_BRANCH(MuId);
  ADD_BRANCH_D(MuIdTight, "Muon tight id. Bit field, one bit per primary vertex hypothesis. Bit position corresponds to index in EvtVtx");
  ADD_BRANCH(MuCh);
  ADD_BRANCH(MuVtxZ);
  ADD_BRANCH(MuDxy);
  ADD_BRANCH(MuIsoRho);
  ADD_BRANCH(MuPfIso);
  ADD_BRANCH(MuType);
  ADD_BRANCH(MuIsoTkIsoAbs);
  ADD_BRANCH(MuIsoTkIsoRel);
  ADD_BRANCH(MuIsoCalAbs);
  ADD_BRANCH(MuIsoCombRel);
  ADD_BRANCH(MuTkNormChi2);
  ADD_BRANCH(MuTkHitCnt);
  ADD_BRANCH(MuMatchedStationCnt);
  ADD_BRANCH(MuDz);
  ADD_BRANCH(MuPixelHitCnt);
  ADD_BRANCH(MuTkLayerCnt);
  ADD_BRANCH(MuPfIsoChHad);
  ADD_BRANCH(MuPfIsoNeutralHad);
  ADD_BRANCH(MuPfIsoRawRel);
  ADD_BRANCH(MuHltMatch);

  //Electrons
  treeHelper_->addDescription("El", "PF reconstructed electrons");
  ADD_BRANCH(ElPt);
  ADD_BRANCH(ElEta);
  ADD_BRANCH(ElEtaSc);
  ADD_BRANCH(ElPhi);
  ADD_BRANCH(ElE);
  ADD_BRANCH(ElId);
  ADD_BRANCH(ElCh);
  //G.N.
  ADD_BRANCH_D(ElScRawE, "Electron Supercluster uncorrected energy");
  ADD_BRANCH_D(ElCorrE, "Electron ncorrected energy");
  ADD_BRANCH_D(ElEcalIso, "ECAL-only isolation");
  ADD_BRANCH_D(ElEcalPfIso, "ECAL-only PFcluster isolation");
  //
  ADD_BRANCH(ElMvaTrig);
  ADD_BRANCH(ElMvaNonTrig);
  ADD_BRANCH(ElMvaPresel);
  ADD_BRANCH(ElDEtaTkScAtVtx);
  ADD_BRANCH(ElDPhiTkScAtVtx);
  ADD_BRANCH(ElHoE);
  ADD_BRANCH(ElSigmaIetaIeta);
  ADD_BRANCH(ElSigmaIetaIetaFull5x5);
  ADD_BRANCH(ElEinvMinusPinv);
  ADD_BRANCH(ElD0);
  ADD_BRANCH(ElDz);
  ADD_BRANCH(ElExpectedMissingInnerHitCnt);
  ADD_BRANCH(ElPassConvVeto);
  ADD_BRANCH(ElHltMatch);
  ADD_BRANCH(ElPfIsoChHad);
  ADD_BRANCH(ElPfIsoNeutralHad);
  ADD_BRANCH(ElPfIsoIso);
  ADD_BRANCH(ElPfIsoPuChHad);
  ADD_BRANCH(ElPfIsoRaw);
  ADD_BRANCH(ElPfIsoDbeta);
  ADD_BRANCH(ElPfIsoRho);
  ADD_BRANCH_D(ElAEff, "Electron effecive area");
  ADD_BRANCH(ElDr03TkSumPt);
  ADD_BRANCH(ElDr03EcalRecHitSumEt);
  ADD_BRANCH(ElDr03HcalTowerSumEt);

  //Modified by Clement Leloup
  treeHelper_->addDescription("Tau", "PF reconstructed taus");
  ADD_BRANCH(TauPt);
  ADD_BRANCH(TauEta);
  ADD_BRANCH(TauPhi);
  ADD_BRANCH(TauE);
  ADD_BRANCH(TauCh);
  ADD_BRANCH(TauDecayModeFinding);
  ADD_BRANCH(TauCombinedIsolationDeltaBetaCorrRaw3Hits);
  ADD_BRANCH(TauDiscMuonLoose);
  ADD_BRANCH(TauDiscMuonTight);
  ADD_BRANCH(TauDiscElVLoose);
  ADD_BRANCH(TauDiscElLoose);
  ADD_BRANCH(TauDiscElVTight);
  ADD_BRANCH(TauDiscElTight);

  //PF candidates
  if(candidateMode_ != candidatesOff){
    treeHelper_->addDescription("PfCands", "PF particle candidates");
    ADD_BRANCH(PfCandPt);
    ADD_BRANCH(PfCandEta);
    ADD_BRANCH(PfCandPhi);
    ADD_BRANCH(PfCandE);
    ADD_BRANCH(PfCandTkInPt);
    ADD_BRANCH(PfCandTkInEta);
    ADD_BRANCH(PfCandTkInPhi);
    ADD_BRANCH(PfCandVx);
    ADD_BRANCH(PfCandVy);
    ADD_BRANCH(PfCandVz);
    ADD_BRANCH(PfCandTkValidHitCnt);
    ADD_BRANCH(PfCandTkValidFrac);
    ADD_BRANCH(PfCandTkChi2);
    ADD_BRANCH(PfCandDz);
    ADD_BRANCH(PfCandDxy);
    ADD_BRANCH(PfCandDz0);
    ADD_BRANCH(PfCandDzPV);
    ADD_BRANCH(PfCandDxyPV);
  }

  if(photonMode_ != photonsOff){
    //photon momenta
    treeHelper_->addDescription("Phot", "Particle flow photons");
    ADD_BRANCH(PhotPt);
    ADD_BRANCH(PhotEta);
    ADD_BRANCH(PhotPhi);
    ADD_BRANCH_D(PhotScRawE, "Photon Supercluster uncorrected energy");
    ADD_BRANCH_D(PhotScEta, "Photon Supercluster eta");
    ADD_BRANCH_D(PhotScPhi, "Photon Supercluster phi");

    //photon isolations
    ADD_BRANCH(PhotIsoEcal);
    ADD_BRANCH(PhotIsoHcal);
    ADD_BRANCH(PhotIsoTk);
    ADD_BRANCH(PhotPfIsoChHad);
    ADD_BRANCH(PhotPfIsoNeutralHad);
    ADD_BRANCH(PhotPfIsoPhot);
    ADD_BRANCH(PhotPfIsoPuChHad);
    ADD_BRANCH(PhotPfIsoEcalClus);
    ADD_BRANCH(PhotPfIsoHcalClus);

    //photon cluster shapes
    ADD_BRANCH_D(PhotE3x3, "Photon energy deposited in 3x3 ECAL crystal array. Divide this quantity by PhotScRawE to obtain the R9 variable.");
    ADD_BRANCH(PhotE1x5);
    ADD_BRANCH(PhotE2x5);
    ADD_BRANCH(PhotE5x5);
    ADD_BRANCH(PhotSigmaIetaIeta);
    //ADD_BRANCH(PhotSigmaIetaIphi);
    //ADD_BRANCH(PhotSigmaIphiIphi);
    ADD_BRANCH(PhotHoE);
    ADD_BRANCH(PhotEtaWidth);
    ADD_BRANCH(PhotPhiWidth);
    ADD_BRANCH(PhotR9);
    ADD_BRANCH(PhotE1x3);  
    ADD_BRANCH(PhotE2x2);
    ADD_BRANCH(PhotS4);
    ADD_BRANCH(PhotE1x5Full5x5);
    ADD_BRANCH(PhotE2x5Full5x5);
    ADD_BRANCH(PhotE3x3Full5x5);
    ADD_BRANCH(PhotE5x5Full5x5);
    ADD_BRANCH(PhotSigmaIetaIetaFull5x5);
    ADD_BRANCH(PhotR9Full5x5);

    //photon ES
    //ADD_BRANCH_D(PhotEsE, "Photon. Energy deposited in the preshower");
    //ADD_BRANCH_D(PhotEsSigmaIxIx, "Photon. Preshower cluster extend along x-axis");
    //ADD_BRANCH_D(PhotEsSigmaIyIy, "Photon. Preshower cluster extend along y-axis");
    //ADD_BRANCH_D(PhotEsSigmaIrIr, "Photon. \\sqrt(PhotEsSigmaIxIx**2+PhotEsSigmaIyIy**2)");

    //photon ID
    ADD_BRANCH_D(PhotId, "Photon Id. Field of bits described in BitFields.PhotId");
    ADD_BRANCH_D(PhotHasPixelSeed, "Pixel and tracker based variable to discreminate photons from electrons");
    ADD_BRANCH_D(PhotPassElVeto, "Conversion safe electron veto");

    //photon timing
    //ADD_BRANCH_D(PhotTime, "Photon. Timing from ECAL");
  }

  //PF Jets
  treeHelper_->addDescription("JetAk04", "Reconstricuted jets clustered with the anti-ket algorithm with distance parameter R = 0.4");
  ADD_BRANCH(JetAk04Pt);
  ADD_BRANCH(JetAk04Eta);
  ADD_BRANCH(JetAk04Phi);
  ADD_BRANCH(JetAk04E);
  ADD_BRANCH_D(JetAk04Id, "Id to reject fake jets from electronic noice");
  ADD_BRANCH_D(JetAk04PuId, "Id to reject jets from pile-up events");
  ADD_BRANCH_D(JetAk04PuMva, "MVA based descriminant for PU jets");
  ADD_BRANCH_D(JetAk04RawPt, "Jet Pt before corrections");
  ADD_BRANCH_D(JetAk04RawE, "Jet energy before corrections");
  ADD_BRANCH_D(JetAk04HfHadE, "Jet hadronic energy deposited in HF");
  ADD_BRANCH_D(JetAk04HfEmE, "Jet electromagnetic energy deposited in HF");
  ADD_BRANCH(JetAk04ChHadFrac);
  ADD_BRANCH(JetAk04NeutralHadAndHfFrac);
  ADD_BRANCH(JetAk04ChEmFrac);
  ADD_BRANCH(JetAk04NeutralEmFrac);
  ADD_BRANCH(JetAk04ChMult);
  ADD_BRANCH(JetAk04ConstCnt);
  ADD_BRANCH(JetAk04Beta);
  ADD_BRANCH(JetAk04BetaClassic);
  ADD_BRANCH(JetAk04BetaStar);
  ADD_BRANCH(JetAk04BetaStarClassic);
  ADD_BRANCH(JetAk04Rms);
  treeHelper_->addDescription("JetAk04BTag", "B tagging with different algorithms");
  ADD_BRANCH_D(JetAk04BTagCsv, "combinedSecondaryVertexBJetTags");
  ADD_BRANCH_D(JetAk04BTagCsvV1, "combinedSecondaryVertexV1BJetTags");
  ADD_BRANCH_D(JetAk04BTagCsvSLV1, "combinedSecondaryVertexSoftPFLeptonV1BJetTags");
  ADD_BRANCH_D(JetAk04BDiscCisvV2, "pfCombinedInclusiveSecondaryVertexV2BJetTags");
  ADD_BRANCH_D(JetAk04BDiscJp, "pfJetProbabilityBJetTags");
  ADD_BRANCH_D(JetAk04BDiscBjp, "pfJetBProbabilityBJetTags");
  ADD_BRANCH_D(JetAk04BDiscTche, "pfTrackCountingHighEffBJetTags");
  ADD_BRANCH_D(JetAk04BDiscTchp, "pfTrackCountingHighPurBJetTags");
  ADD_BRANCH_D(JetAk04BDiscSsvhe, "pfSimpleSecondaryVertexHighEffBJetTags");
  ADD_BRANCH_D(JetAk04BDiscSsvhp, "pfSimpleSecondaryVertexHighPurBJetTags");
  ADD_BRANCH_D(JetAk04PartFlav, "Quark-based jet flavor.");
  ADD_BRANCH_D(JetAk04HadFlav, "Hadron-based jet flavor.");
  ADD_BRANCH(JetAk04JecUncUp);
  ADD_BRANCH(JetAk04JecUncDwn);
  ADD_BRANCH(JetAk04ConstId);
  ADD_BRANCH(JetAk04ConstPt);
  ADD_BRANCH(JetAk04ConstEta);
  ADD_BRANCH(JetAk04ConstPhi);
  ADD_BRANCH(JetAk04ConstE);
  ADD_BRANCH(JetAk04GenJet);

  //Modified by Clement Leloup
  //PF Fat Jets
  if(fatJetMode_ == fatJetOn){

    treeHelper_->addDescription("JetAk08", "Reconstructed jets clustered with the anti-ket algorithm with distance parameter R = 0.8");
    ADD_BRANCH(JetAk08Pt);
    ADD_BRANCH(JetAk08Eta);
    ADD_BRANCH(JetAk08Phi);
    ADD_BRANCH(JetAk08E);
    ADD_BRANCH_D(JetAk08Id, "Id to reject fake jets from electronic noice");
    ADD_BRANCH_D(JetAk08RawPt, "Jet Pt before corrections");
    ADD_BRANCH_D(JetAk08RawE, "Jet energy before corrections");
    ADD_BRANCH_D(JetAk08HfHadE, "Jet hadronic energy deposited in HF");
    ADD_BRANCH_D(JetAk08HfEmE, "Jet electromagnetic energy deposited in HF");
    ADD_BRANCH(JetAk08ChHadFrac);
    ADD_BRANCH(JetAk08NeutralHadAndHfFrac);
    ADD_BRANCH(JetAk08ChEmFrac);
    ADD_BRANCH(JetAk08NeutralEmFrac);
    ADD_BRANCH(JetAk08ChMult);
    ADD_BRANCH(JetAk08ConstCnt);

    treeHelper_->addDescription("JetAk08BTag", "B tagging with different algorithms");
    ADD_BRANCH_D(JetAk08BTagCsv, "combinedSecondaryVertexBJetTags");
    ADD_BRANCH_D(JetAk08BTagCsvV1, "combinedSecondaryVertexV1BJetTags");
    ADD_BRANCH_D(JetAk08BTagCsvSLV1, "combinedSecondaryVertexSoftPFLeptonV1BJetTags");
    ADD_BRANCH_D(JetAk08BDiscCisvV2, "pfCombinedInclusiveSecondaryVertexV2BJetTags");
    ADD_BRANCH_D(JetAk08BDiscJp, "pfJetProbabilityBJetTags");
    ADD_BRANCH_D(JetAk08BDiscBjp, "pfJetBProbabilityBJetTags");
    ADD_BRANCH_D(JetAk08BDiscTche, "pfTrackCountingHighEffBJetTags");
    ADD_BRANCH_D(JetAk08BDiscTchp, "pfTrackCountingHighPurBJetTags");
    ADD_BRANCH_D(JetAk08BDiscSsvhe, "pfSimpleSecondaryVertexHighEffBJetTags");
    ADD_BRANCH_D(JetAk08BDiscSsvhp, "pfSimpleSecondaryVertexHighPurBJetTags");
    ADD_BRANCH_D(JetAk08PartFlav, "Quark-based jet flavor.");
    ADD_BRANCH_D(JetAk08HadFlav, "Hadron-based jet flavor.");
    ADD_BRANCH(JetAk08JecUncUp);
    ADD_BRANCH(JetAk08JecUncDwn);
    ADD_BRANCH(JetAk08ConstId);
    ADD_BRANCH(JetAk08ConstPt);
    ADD_BRANCH(JetAk08ConstEta);
    ADD_BRANCH(JetAk08ConstPhi);
    ADD_BRANCH(JetAk08ConstE);
    ADD_BRANCH(JetAk08GenJet);
    ADD_BRANCH_D(JetAk08PrunedMass, "Pruned Mass of the jet");
    ADD_BRANCH_D(JetAk08FilteredMass, "Filtered Mass of the jet");
    ADD_BRANCH_D(JetAk08SoftDropMass, "SoftDrop Mass of the jet");
    ADD_BRANCH_D(JetAk08TrimmedMass, "Trimmed Mass of the jet");
    ADD_BRANCH_D(JetAk08Tau1, "1-Subjettiness");
    ADD_BRANCH_D(JetAk08Tau2, "2-Subjettiness");
    ADD_BRANCH_D(JetAk08Tau3, "3-Subjettiness");
  }

  defineBitFields();

}

void Tupel::allocateTrigMap(int nTrigMax){
  std::vector<int> row(nTrigMax+2, 0);
  std::vector<std::vector<int> > newMap(nTrigMax+1, row);
  std::vector<std::string> newTrigNames(nTrigMax, "");
  for(unsigned i = 0; i < trigAccept_.size(); ++i){
    if(i < trigNames_.size()) newTrigNames[i] = trigNames_[i];
    for(unsigned j = 0; j < trigAccept_[i].size(); ++j){
      newMap[i][j] = trigAccept_[i][j];
    }
  }
  trigNames_.swap(newTrigNames);
  trigAccept_.swap(newMap);
}

void
Tupel::endJob(){
  //  delete jecUnc;
  //  myTree->Print();
  treeHelper_->fillDescriptionTree();

  if(triggerStat_) writeTriggerStat();
}

void
Tupel::writeTriggerStat(){
  std::ofstream f("trigger_stat.txt");
  if(!trigStatValid_){
    f << "Trigger statistics is not available. \n\n"
      "Events with different trigger index maps were processed. Trigger statistics"
      "is not supported for such case.\n\n";
    f << "Column S contains the number of events the trigger indicated in second column of is the only one fired.\n"
      << "The columns on the right of the column S contain for each combination of two triggers the number of events "
      "for which both triggers have been fired. The number in the column header refers to the trigger indices "
      "contained in the first column of the table.\n\n";
  } else {

    f << "Trigger statistics\n"
      << "------------------\n\n"
      << "Total number of processed events: " << analyzedEventCnt_ << "\n\n";

    f  << "#\tTrigger line\tAccept count\tAccept rate\tS";
    //sort the map per value by inverting key and value:
    std::vector<int> trigSortedIndex(trigNames_.size());
    size_t maxTrigForCol = 10;
    for(unsigned i = 0; i < trigSortedIndex.size(); ++i){
      trigSortedIndex[i] = i;
      if(i<maxTrigForCol) f << "\t" << (1+i);
    }
    f << "\n";
    sort(trigSortedIndex.begin(), trigSortedIndex.end(), TrigSorter(this));
    //    trigSortedIndex.resize(10);
    f << "0\tNone\t" << trigAccept_[0][0] << "\n";
    for(unsigned i = 0; i < trigSortedIndex.size(); ++i){
      unsigned ii = trigSortedIndex[i];
      f << (i+1) << "\t" << trigNames_[ii] << " (" << ii << ")"
	<< "\t" << trigAccept_[1+ii][0]
	<< "\t" << double(trigAccept_[1+ii][0]) / analyzedEventCnt_
	<< "\t" << trigAccept_[1+ii][1];
      for(unsigned j = 0; j < std::min(maxTrigForCol, trigSortedIndex.size()); ++j){
	unsigned jj = trigSortedIndex[j];
	f << "\t" << trigAccept_[1+ii][2+jj];
      }
      f << "\n";
    }
  }
}

void
Tupel::endRun(edm::Run const& iRun, edm::EventSetup const&){

  std::string desc = "List of MC event weights. Use the first one by default.";
  if(weightFromGenEventInfo_ == YES){
    desc += " The first element contains the GenInfoProduct weight.";
  } else if(weightFromGenEventInfo_ == NO){
    desc += " GenInfoProduct weight was not found and the first element was set to 1.";
  } else if(weightFromGenEventInfo_ == MIXTURE){
    desc += " GenInfoProduct weight was missing from some event(s). The first element was set to this weight when found and to 1 otherwise.";
  }

  if(weightsFromLhe_){
    desc += "Elements starting from index 1 contains the weights from LHEEventProduct.";
  }

  edm::Handle<LHERunInfoProduct> lheRun;
  iRun.getByToken(lheRunToken_, lheRun );

  if(!lheRun.failedToGet ()){
    const LHERunInfoProduct& myLHERunInfoProduct = *(lheRun.product());
    for (std::vector<LHERunInfoProduct::Header>::const_iterator iter = myLHERunInfoProduct.headers_begin();
	 iter != myLHERunInfoProduct.headers_end();
	 iter++){
      if(iter == myLHERunInfoProduct.headers_begin()){
	desc += "A description of the LHE weights in the order they appears in the vector is given below.\n";
      }
      if(iter->tag() == "initrwgt" && iter->size() > 0){
	desc += "\n";
	for(std::vector<std::string>::const_iterator it = iter->begin();
	    it != iter->end(); ++it){
	  desc += *it;
	}
	break;
      }
    }
  }

  //Suppresses spurious last line with "<" produced with CMSSW_7_4_1:
  if(desc.size() > 2 && desc[desc.size()-1] == '<'){
    std::string::size_type p = desc.find_last_not_of(" \n", desc.size()-2);
    if(p != std::string::npos) desc.erase(p + 1);
  }
  treeHelper_->addDescription("EvtWeights", desc.c_str());
}

bool Tupel::compTrigger(const char* a, const char* bv) const{
  int i = 0;
  for(;a[i]!=0 && bv[i]!=0; ++i){
    if(a[i]!= bv[i]) return false;
  }
  if(a[i]) return false;
  if(bv[i]==0) return true;
  if(bv[i] != '_') return false;
  if(bv[++i]!='v') return false;
  for(;;){
    if(bv[++i]==0) return true;
    if(!isdigit(bv[i])) return false;
  }
  return true;
};


DEFINE_FWK_MODULE(Tupel);
