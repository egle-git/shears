#!/bin/bash
for p in EgammaAnalysis/ElectronTools RecoJets/JetProducers; do
    s=`dirname $p`
    { mkdir $s && cp -r `readlink -f $CMSSW_RELEASE_BASE/src/$p` $s/; } || { echo "Failed to copy $p. Aborting."; exit 1; }
done

patch -p1 <<EOF
diff --git a/EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimatorCSA14.h b/EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimatorCSA14.h
index 4674c75..b872685 100644
--- a/EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimatorCSA14.h
+++ b/EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimatorCSA14.h
@@ -20,7 +20,6 @@
 #include "TMVA/Tools.h"
 #include "TMVA/Reader.h"
 
-
 #include "DataFormats/TrackReco/interface/Track.h"
 #include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
 #include "DataFormats/TrackReco/interface/TrackFwd.h"
@@ -39,7 +38,6 @@
 
 #include "DataFormats/PatCandidates/interface/Electron.h"
 
-
 class EGammaMvaEleEstimatorCSA14{
   public:
     EGammaMvaEleEstimatorCSA14();
diff --git a/EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h b/EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h
index ea5021b..d9983bb 100644
--- a/EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h
+++ b/EgammaAnalysis/ElectronTools/interface/ElectronEffectiveArea.h
@@ -20,7 +20,6 @@
 #ifndef STANDALONE
 #endif
 
-
 class ElectronEffectiveArea{
  public:
   ElectronEffectiveArea();
@@ -64,7 +63,6 @@ class ElectronEffectiveArea{
     static Double_t GetElectronEffectiveArea(ElectronEffectiveAreaType type, Double_t SCEta, 
                                              ElectronEffectiveAreaTarget EffectiveAreaTarget = kEleEAData2011) {
       
-      using namespace std;
       Double_t EffectiveArea = 0;
 
 
diff --git a/EgammaAnalysis/ElectronTools/interface/ElectronEnergyCalibrator.h b/EgammaAnalysis/ElectronTools/interface/ElectronEnergyCalibrator.h
index 7d3f637..77e1584 100644
--- a/EgammaAnalysis/ElectronTools/interface/ElectronEnergyCalibrator.h
+++ b/EgammaAnalysis/ElectronTools/interface/ElectronEnergyCalibrator.h
@@ -8,7 +8,6 @@
 #include <sstream>
 #include <iostream>
 
-
 namespace edm {
   class StreamID;
 }
@@ -41,61 +40,60 @@ struct linearityCorrectionValues
 
 class ElectronEnergyCalibrator
 {
-    public:
-        ElectronEnergyCalibrator( const std::string pathData, 
-                                  const std::string pathLinData,
-                                  const std::string dataset, 
-                                  int correctionsType, 
-                                  bool applyLinearityCorrection, 
-                                  double lumiRatio, 
-                                  bool isMC, 
-                                  bool updateEnergyErrors, 
-                                  bool verbose, 
-                                  bool synchronization
-                                ) : 
-                                  pathData_(pathData), 
-                                  pathLinData_(pathLinData), 
-                                  dataset_(dataset), 
-                                  correctionsType_(correctionsType), 
-                                  applyLinearityCorrection_(applyLinearityCorrection),
-                                  lumiRatio_(lumiRatio), 
-                                  isMC_(isMC), 
-                                  updateEnergyErrors_(updateEnergyErrors), 
-                                  verbose_(verbose), 
-                                  synchronization_(synchronization) 
-	    {
-		    init();
-    	}
-
-        void calibrate(SimpleElectron &electron, edm::StreamID const&);
-        void correctLinearity(SimpleElectron &electron);
+ public:
+ ElectronEnergyCalibrator(const std::string pathData, 
+			  const std::string pathLinData,
+			  const std::string dataset, 
+			  int correctionsType, 
+			  bool applyLinearityCorrection, 
+			  double lumiRatio, 
+			  bool isMC, 
+			  bool updateEnergyErrors, 
+			  bool verbose, 
+			  bool synchronization
+			  ) : 
+    pathData_(pathData), 
+    pathLinData_(pathLinData), 
+    dataset_(dataset), 
+    correctionsType_(correctionsType), 
+    applyLinearityCorrection_(applyLinearityCorrection),
+    lumiRatio_(lumiRatio), 
+    isMC_(isMC), 
+    updateEnergyErrors_(updateEnergyErrors), 
+    verbose_(verbose), 
+    synchronization_(synchronization) {
+      init();
+    }
 
-    private:
-        void init();
-        void splitString( const std::string &fullstr, 
-                          std::vector<std::string> &elements, 
-                          const std::string &delimiter
-                        );
-        double stringToDouble(const std::string &str);
-      
-        double newEnergy_ ;
-        double newEnergyError_ ;
-        
-        std::string pathData_;
-        std::string pathLinData_;
-        std::string dataset_;
-        int correctionsType_;
-        bool applyLinearityCorrection_;
-        double lumiRatio_;
-        bool isMC_;
-        bool updateEnergyErrors_;
-        bool verbose_;
-        bool synchronization_;
+    void calibrate(SimpleElectron &electron, edm::StreamID const&);
+    void correctLinearity(SimpleElectron &electron);
+				  
+ private:
+    void init();
+    void splitString( const std::string &fullstr, 
+		      std::vector<std::string> &elements, 
+		      const std::string &delimiter
+		      );
+    double stringToDouble(const std::string &str);
       
-        correctionValues corrValArray[100];
-        correctionValues corrValMC;
-        linearityCorrectionValues linCorrValArray[100];
-        int nCorrValRaw, nLinCorrValRaw;
+    double newEnergy_ ;
+    double newEnergyError_ ;
+    
+    std::string pathData_;
+    std::string pathLinData_;
+    std::string dataset_;
+    int correctionsType_;
+    bool applyLinearityCorrection_;
+    double lumiRatio_;
+    bool isMC_;
+    bool updateEnergyErrors_;
+    bool verbose_;
+    bool synchronization_;
+    
+    correctionValues corrValArray[100];
+    correctionValues corrValMC;
+    linearityCorrectionValues linCorrValArray[100];
+    int nCorrValRaw, nLinCorrValRaw;
 };
 
 #endif
diff --git a/EgammaAnalysis/ElectronTools/interface/EpCombinationTool.h b/EgammaAnalysis/ElectronTools/interface/EpCombinationTool.h
index cb0a098..38dba5b 100644
--- a/EgammaAnalysis/ElectronTools/interface/EpCombinationTool.h
+++ b/EgammaAnalysis/ElectronTools/interface/EpCombinationTool.h
@@ -12,17 +12,18 @@ class EpCombinationTool
     public:
         EpCombinationTool();
         ~EpCombinationTool();
+        // forbid copy and assignment, since we have a custom deleter
+        EpCombinationTool(const EpCombinationTool &other) = delete;
+        EpCombinationTool & operator=(const EpCombinationTool &other) = delete;
 
-        bool init(const std::string& regressionFile, const std::string& bdtName="");
-//        float combine(float energy, float energyError,
-//                float momentum, float momentumError, 
-//                int electronClass,
-//                bool isEcalDriven, bool isTrackerDriven, bool isEB);
-	void combine(SimpleElectron & mySimpleElectron);
+        bool init(const GBRForest *forest) ;
+        bool init(const std::string& regressionFile, const std::string& bdtName);
+	void combine(SimpleElectron & mySimpleElectron) const;
 
 
     private:
-        GBRForest* m_forest;
+        const GBRForest* m_forest;
+        bool  m_ownForest;
 
 };
 
diff --git a/EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h b/EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h
index 17b67df..e82f680 100644
--- a/EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h
+++ b/EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h
@@ -50,7 +50,6 @@
 #include "DataFormats/VertexReco/interface/Vertex.h"
 #include "DataFormats/VertexReco/interface/VertexFwd.h"
 
-
 class PFIsolationEstimator{
  public:
   PFIsolationEstimator();
@@ -61,7 +60,7 @@ class PFIsolationEstimator{
     kPhoton  =  1            // MVA for triggering electrons
   };
   
-
+  
   void     initializeElectronIsolation( Bool_t  bApplyVeto);
   void     initializePhotonIsolation( Bool_t  bApplyVeto);
   void     initializeElectronIsolationInRings( Bool_t  bApplyVeto, int iNumberOfRings, float fRingSize );
@@ -69,18 +68,18 @@ class PFIsolationEstimator{
   void     initializeRings(int iNumberOfRings, float fRingSize);
   Bool_t   isInitialized() const { return fisInitialized; }
   
-
+  
   float fGetIsolation(const reco::PFCandidate * pfCandidate,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection >  vertices );
   std::vector<float >  fGetIsolationInRings(const reco::PFCandidate * pfCandidate,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices);
   
    float fGetIsolation(const reco::Photon* photon,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection >  vertices );
-  std::vector<float >  fGetIsolationInRings(const reco::Photon* photon,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices);
+   std::vector<float >  fGetIsolationInRings(const reco::Photon* photon,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices);
 
    float fGetIsolation(const reco::GsfElectron* electron,const reco::PFCandidateCollection* pfParticlesColl,const reco::VertexRef vtx, edm::Handle< reco::VertexCollection >  vertices );
-  std::vector<float >  fGetIsolationInRings(const reco::GsfElectron* electron,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices);
+   std::vector<float >  fGetIsolationInRings(const reco::GsfElectron* electron,const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices);
 
 
-  reco::VertexRef chargedHadronVertex(edm::Handle< reco::VertexCollection > verticies, const reco::PFCandidate& pfcand );
+   reco::VertexRef chargedHadronVertex(edm::Handle< reco::VertexCollection > verticies, const reco::PFCandidate& pfcand );
 
   void setConeSize(float fValue = 0.4){ fConeSize = fValue;};
 
@@ -161,11 +160,11 @@ class PFIsolationEstimator{
   float                   fIsolationCharged;
   float                   fIsolationChargedAll;
   
-  std::vector<float >          fIsolationInRings;
-  std::vector<float >          fIsolationInRingsPhoton;
-  std::vector<float >          fIsolationInRingsNeutral;
-  std::vector<float >          fIsolationInRingsCharged;  
-  std::vector<float >          fIsolationInRingsChargedAll;
+  std::vector<float >     fIsolationInRings;
+  std::vector<float >     fIsolationInRingsPhoton;
+  std::vector<float >     fIsolationInRingsNeutral;
+  std::vector<float >     fIsolationInRingsCharged;  
+  std::vector<float >     fIsolationInRingsChargedAll;
 
   Bool_t                    checkClosestZVertex;
   float                     fConeSize;
@@ -226,7 +225,7 @@ class PFIsolationEstimator{
   float                   fVy;
   float                   fVz;
   
-  reco::SuperClusterRef   refSC;
+  reco::SuperClusterRef         refSC;
   bool                    pivotInBarrel;
 
   math::XYZVector         vtxWRTCandidate;
diff --git a/EgammaAnalysis/ElectronTools/interface/SimpleElectron.h b/EgammaAnalysis/ElectronTools/interface/SimpleElectron.h
index 093e439c9..5c81644 100644
--- a/EgammaAnalysis/ElectronTools/interface/SimpleElectron.h
+++ b/EgammaAnalysis/ElectronTools/interface/SimpleElectron.h
@@ -1,11 +1,15 @@
 #ifndef SimpleElectron_H
 #define SimpleElectron_H
+ 
+#ifndef SimpleElectron_STANDALONE
+#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"        
+#endif
 
 class SimpleElectron
 {
 	public:
 	    SimpleElectron(){}
-	    SimpleElectron( double run, 
+	    SimpleElectron( unsigned int run, 
                         double eClass, 
                         double r9, 
                         double scEnergy, 
@@ -36,32 +40,40 @@ class SimpleElectron
                         isMC_(isMC), 
                         isEcalDriven_(isEcalDriven), 
                         isTrackerDriven_(isTrackerDriven), 
+                        newEnergy_(regEnergy_), 
+                        newEnergyError_(regEnergyError_),
                         combinedMomentum_(combinedMomentum), 
-                        combinedMomentumError_(combinedMomentumError) 
+                        combinedMomentumError_(combinedMomentumError),
+                        scale_(1.0), smearing_(0.0)
         {}
 	    ~SimpleElectron(){}	
 
+#ifndef SimpleElectron_STANDALONE
+        explicit SimpleElectron(const reco::GsfElectron &in, unsigned int runNumber, bool isMC) ;
+        void writeTo(reco::GsfElectron & out) const ;
+#endif
+
     	//accessors
-    	double getNewEnergy(){return newEnergy_;}
-    	double getNewEnergyError(){return newEnergyError_;}
-    	double getCombinedMomentum(){return combinedMomentum_;}
-    	double getCombinedMomentumError(){return combinedMomentumError_;}
-    	double getScale(){return scale_;}
-    	double getSmearing(){return smearing_;}
-    	double getSCEnergy(){return scEnergy_;}
-    	double getSCEnergyError(){return scEnergyError_;}
-    	double getRegEnergy(){return regEnergy_;}
-    	double getRegEnergyError(){return regEnergyError_;}
-    	double getTrackerMomentum(){return trackMomentum_;}
-    	double getTrackerMomentumError(){return trackMomentumError_;}
-    	double getEta(){return eta_;}
-    	float getR9(){return r9_;}
-    	int getElClass(){return eClass_;}
-    	int getRunNumber(){return run_;}
-    	bool isEB(){return isEB_;}
-    	bool isMC(){return isMC_;}
-    	bool isEcalDriven(){return isEcalDriven_;}
-    	bool isTrackerDriven(){return isTrackerDriven_;}
+    	double getNewEnergy() const {return newEnergy_;}
+    	double getNewEnergyError() const {return newEnergyError_;}
+    	double getCombinedMomentum() const {return combinedMomentum_;}
+    	double getCombinedMomentumError() const {return combinedMomentumError_;}
+    	double getScale() const {return scale_;}
+    	double getSmearing() const {return smearing_;}
+    	double getSCEnergy() const {return scEnergy_;}
+    	double getSCEnergyError() const {return scEnergyError_;}
+    	double getRegEnergy() const {return regEnergy_;}
+    	double getRegEnergyError() const {return regEnergyError_;}
+    	double getTrackerMomentum() const {return trackMomentum_;}
+    	double getTrackerMomentumError() const {return trackMomentumError_;}
+    	double getEta() const {return eta_;}
+    	float getR9() const {return r9_;}
+    	int getElClass() const {return eClass_;}
+    	unsigned int getRunNumber() const {return run_;}
+    	bool isEB() const {return isEB_;}
+    	bool isMC() const {return isMC_;}
+    	bool isEcalDriven() const {return isEcalDriven_;}
+    	bool isTrackerDriven() const {return isTrackerDriven_;}
     
     	//setters
     	void setCombinedMomentum(double combinedMomentum){combinedMomentum_ = combinedMomentum;}
@@ -70,7 +82,7 @@ class SimpleElectron
     	void setNewEnergyError(double newEnergyError){newEnergyError_ = newEnergyError;}
 
 	private:
-    	double run_; 
+    	unsigned int run_; 
         double eClass_;
     	double r9_;
     	double scEnergy_; 
diff --git a/EgammaAnalysis/ElectronTools/plugins/CalibratedElectronProducer.cc b/EgammaAnalysis/ElectronTools/plugins/CalibratedElectronProducer.cc
index 298afe6..dd2db56 100644
--- a/EgammaAnalysis/ElectronTools/plugins/CalibratedElectronProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/CalibratedElectronProducer.cc
@@ -31,127 +31,123 @@
 
 #include <iostream>
 
-using namespace edm ;
-using namespace std ;
-using namespace reco ;
-
 CalibratedElectronProducer::CalibratedElectronProducer( const edm::ParameterSet & cfg )
 {
-    inputElectronsToken_ = consumes<reco::GsfElectronCollection>(cfg.getParameter<edm::InputTag>("inputElectronsTag"));
-
-    energyRegToken_      = consumes<edm::ValueMap<double> >(cfg.getParameter<edm::InputTag>("nameEnergyReg"));
-    energyErrorRegToken_ = consumes<edm::ValueMap<double> >(cfg.getParameter<edm::InputTag>("nameEnergyErrorReg"));
-
-    recHitCollectionEBToken_ = consumes<EcalRecHitCollection>(cfg.getParameter<edm::InputTag>("recHitCollectionEB"));
-    recHitCollectionEEToken_ = consumes<EcalRecHitCollection>(cfg.getParameter<edm::InputTag>("recHitCollectionEE"));
-
-
-    nameNewEnergyReg_      = cfg.getParameter<std::string>("nameNewEnergyReg");
-    nameNewEnergyErrorReg_ = cfg.getParameter<std::string>("nameNewEnergyErrorReg");
-    newElectronName_ = cfg.getParameter<std::string>("outputGsfElectronCollectionLabel");
-
-
-    dataset = cfg.getParameter<std::string>("inputDataset");
-    isMC = cfg.getParameter<bool>("isMC");
-    updateEnergyError = cfg.getParameter<bool>("updateEnergyError");
-    lumiRatio = cfg.getParameter<double>("lumiRatio");
-    correctionsType = cfg.getParameter<int>("correctionsType");
-    applyLinearityCorrection = cfg.getParameter<bool>("applyLinearityCorrection");
-    combinationType = cfg.getParameter<int>("combinationType");
-    verbose = cfg.getParameter<bool>("verbose");
-    synchronization = cfg.getParameter<bool>("synchronization");
-    combinationRegressionInputPath = cfg.getParameter<std::string>("combinationRegressionInputPath");
-    scaleCorrectionsInputPath = cfg.getParameter<std::string>("scaleCorrectionsInputPath");
-    linCorrectionsInputPath   = cfg.getParameter<std::string>("linearityCorrectionsInputPath");
-
-    //basic checks
-    if ( isMC && ( dataset != "Summer11" && dataset != "Fall11"
-        && dataset!= "Summer12" && dataset != "Summer12_DR53X_HCP2012"
-        && dataset != "Summer12_LegacyPaper" ) )
+  inputElectronsToken_ = consumes<reco::GsfElectronCollection>(cfg.getParameter<edm::InputTag>("inputElectronsTag"));
+  
+  energyRegToken_      = consumes<edm::ValueMap<double> >(cfg.getParameter<edm::InputTag>("nameEnergyReg"));
+  energyErrorRegToken_ = consumes<edm::ValueMap<double> >(cfg.getParameter<edm::InputTag>("nameEnergyErrorReg"));
+  
+  recHitCollectionEBToken_ = consumes<EcalRecHitCollection>(cfg.getParameter<edm::InputTag>("recHitCollectionEB"));
+  recHitCollectionEEToken_ = consumes<EcalRecHitCollection>(cfg.getParameter<edm::InputTag>("recHitCollectionEE"));
+
+  
+  nameNewEnergyReg_      = cfg.getParameter<std::string>("nameNewEnergyReg");
+  nameNewEnergyErrorReg_ = cfg.getParameter<std::string>("nameNewEnergyErrorReg");
+  newElectronName_ = cfg.getParameter<std::string>("outputGsfElectronCollectionLabel");
+
+
+  dataset = cfg.getParameter<std::string>("inputDataset");
+  isMC = cfg.getParameter<bool>("isMC");
+  updateEnergyError = cfg.getParameter<bool>("updateEnergyError");
+  lumiRatio = cfg.getParameter<double>("lumiRatio");
+  correctionsType = cfg.getParameter<int>("correctionsType");
+  applyLinearityCorrection = cfg.getParameter<bool>("applyLinearityCorrection");
+  combinationType = cfg.getParameter<int>("combinationType");
+  verbose = cfg.getParameter<bool>("verbose");
+  synchronization = cfg.getParameter<bool>("synchronization");
+  combinationRegressionInputPath = cfg.getParameter<std::string>("combinationRegressionInputPath");
+  scaleCorrectionsInputPath = cfg.getParameter<std::string>("scaleCorrectionsInputPath");
+  linCorrectionsInputPath   = cfg.getParameter<std::string>("linearityCorrectionsInputPath");
+  
+  //basic checks
+  if ( isMC && ( dataset != "Summer11" && dataset != "Fall11"
+		 && dataset!= "Summer12" && dataset != "Summer12_DR53X_HCP2012"
+		 && dataset != "Summer12_LegacyPaper" ) )
     {
-        throw cms::Exception("CalibratedgsfElectronProducer|ConfigError") << "Unknown MC dataset";
+      throw cms::Exception("CalibratedgsfElectronProducer|ConfigError") << "Unknown MC dataset";
     }
     if ( !isMC && ( dataset != "Prompt" && dataset != "ReReco"
-        && dataset != "Jan16ReReco" && dataset != "ICHEP2012"
-        && dataset != "Moriond2013" && dataset != "22Jan2013ReReco" ) )
-    {
+		    && dataset != "Jan16ReReco" && dataset != "ICHEP2012"
+		    && dataset != "Moriond2013" && dataset != "22Jan2013ReReco" ) )
+      {
         throw cms::Exception("CalibratedgsfElectronProducer|ConfigError") << "Unknown Data dataset";
     }
-
+    
     // Linearity correction only applied on combined momentum obtain with regression combination
     if(combinationType!=3 && applyLinearityCorrection)
-    {
+      {
         std::cout << "[CalibratedElectronProducer] "
-            << "Warning: you chose combinationType!=3 and applyLinearityCorrection=True. Linearity corrections are only applied on top of combination 3." << std::endl;
-    }
-
+		  << "Warning: you chose combinationType!=3 and applyLinearityCorrection=True. Linearity corrections are only applied on top of combination 3." << std::endl;
+      }
+    
     std::cout << "[CalibratedGsfElectronProducer] Correcting scale for dataset " << dataset << std::endl;
-
+    
     //initializations
     std::string pathToDataCorr;
     switch (correctionsType)
-    {
-        case 0:
-      	    break;
-    	case 1:
-    		if ( verbose )
-            {
-                std::cout << "You choose regression 1 scale corrections" << std::endl;
-            }
-    	    break;
-    	case 2:
-    		if ( verbose )
-            {
-                std::cout << "You choose regression 2 scale corrections." << std::endl;
-            }
-    	    break;
-    	case 3:
-            throw cms::Exception("CalibratedgsfElectronProducer|ConfigError")
-                << "You choose standard non-regression ecal energy scale corrections. They are not implemented yet.";
-    		break;
-    	default:
-            throw cms::Exception("CalibratedgsfElectronProducer|ConfigError")
-                << "Unknown correctionsType !!!" ;
-    }
-
+      {
+      case 0:
+	break;
+      case 1:
+	if ( verbose )
+	  {
+	    std::cout << "You choose regression 1 scale corrections" << std::endl;
+	  }
+	break;
+      case 2:
+	if ( verbose )
+	  {
+	    std::cout << "You choose regression 2 scale corrections." << std::endl;
+	  }
+	break;
+      case 3:
+	throw cms::Exception("CalibratedgsfElectronProducer|ConfigError")
+	  << "You choose standard non-regression ecal energy scale corrections. They are not implemented yet.";
+	break;
+      default:
+	throw cms::Exception("CalibratedgsfElectronProducer|ConfigError")
+	  << "Unknown correctionsType !!!" ;
+      }
+    
     theEnCorrector = new ElectronEnergyCalibrator
-        (
-            edm::FileInPath(scaleCorrectionsInputPath.c_str()).fullPath().c_str(),
-            edm::FileInPath(linCorrectionsInputPath.c_str()).fullPath().c_str(),
-            dataset,
-            correctionsType,
-            applyLinearityCorrection,
-            lumiRatio,
-            isMC,
-            updateEnergyError,
-            verbose,
-            synchronization
-        );
-
+      (
+       edm::FileInPath(scaleCorrectionsInputPath.c_str()).fullPath().c_str(),
+       edm::FileInPath(linCorrectionsInputPath.c_str()).fullPath().c_str(),
+       dataset,
+       correctionsType,
+       applyLinearityCorrection,
+       lumiRatio,
+       isMC,
+       updateEnergyError,
+       verbose,
+       synchronization
+       );
+    
     if ( verbose )
-    {
+      {
         std::cout<<"[CalibratedGsfElectronProducer] "
-        << "ElectronEnergyCalibrator object is created" << std::endl;
-    }
-
+		 << "ElectronEnergyCalibrator object is created" << std::endl;
+      }
+    
     myEpCombinationTool = new EpCombinationTool();
     myEpCombinationTool->init
-        (
-            edm::FileInPath(combinationRegressionInputPath.c_str()).fullPath().c_str(),
-            "CombinationWeight"
-        );
-
+      (
+       edm::FileInPath(combinationRegressionInputPath.c_str()).fullPath().c_str(),
+       "CombinationWeight"
+       );
+    
     myCombinator = new ElectronEPcombinator();
-
+    
     if ( verbose )
-    {
+      {
         std::cout << "[CalibratedGsfElectronProducer] "
-        << "Combination tools are created and initialized" << std::endl;
-    }
-
+		  << "Combination tools are created and initialized" << std::endl;
+      }
+    
     produces<edm::ValueMap<double> >(nameNewEnergyReg_);
     produces<edm::ValueMap<double> >(nameNewEnergyErrorReg_);
-    produces<GsfElectronCollection> (newElectronName_);
+    produces<reco::GsfElectronCollection> (newElectronName_);
     geomInitialized_ = false;
 }
 
@@ -160,220 +156,220 @@ CalibratedElectronProducer::~CalibratedElectronProducer()
 
 void CalibratedElectronProducer::produce( edm::Event & event, const edm::EventSetup & setup )
 {
-    if (!geomInitialized_)
+  if (!geomInitialized_)
     {
-        edm::ESHandle<CaloTopology> theCaloTopology;
-        setup.get<CaloTopologyRecord>().get(theCaloTopology);
-        ecalTopology_ = & (*theCaloTopology);
-
-        edm::ESHandle<CaloGeometry> theCaloGeometry;
-        setup.get<CaloGeometryRecord>().get(theCaloGeometry);
-        caloGeometry_ = & (*theCaloGeometry);
-        geomInitialized_ = true;
+      edm::ESHandle<CaloTopology> theCaloTopology;
+      setup.get<CaloTopologyRecord>().get(theCaloTopology);
+      ecalTopology_ = & (*theCaloTopology);
+      
+      edm::ESHandle<CaloGeometry> theCaloGeometry;
+      setup.get<CaloGeometryRecord>().get(theCaloGeometry);
+      caloGeometry_ = & (*theCaloGeometry);
+      geomInitialized_ = true;
     }
-
-    // Read GsfElectrons
-    edm::Handle<reco::GsfElectronCollection>  oldElectronsH ;
-    event.getByToken(inputElectronsToken_,oldElectronsH) ;
-
-    // Read RecHits
-    edm::Handle< EcalRecHitCollection > pEBRecHits;
-    edm::Handle< EcalRecHitCollection > pEERecHits;
-    event.getByToken( recHitCollectionEBToken_, pEBRecHits );
-    event.getByToken( recHitCollectionEEToken_, pEERecHits );
-
-    // ReadValueMaps
-    edm::Handle<edm::ValueMap<double> > valMapEnergyH;
-    event.getByToken(energyRegToken_,valMapEnergyH);
-    edm::Handle<edm::ValueMap<double> > valMapEnergyErrorH;
-    event.getByToken(energyErrorRegToken_,valMapEnergyErrorH);
-
-    // Prepare output collections
-    std::auto_ptr<GsfElectronCollection> electrons( new reco::GsfElectronCollection ) ;
-    // Fillers for ValueMaps:
-    std::auto_ptr<edm::ValueMap<double> > regrNewEnergyMap(new edm::ValueMap<double>() );
-    edm::ValueMap<double>::Filler energyFiller(*regrNewEnergyMap);
-
-    std::auto_ptr<edm::ValueMap<double> > regrNewEnergyErrorMap(new edm::ValueMap<double>() );
-    edm::ValueMap<double>::Filler energyErrorFiller(*regrNewEnergyErrorMap);
-
-    // first clone the initial collection
-    unsigned nElectrons = oldElectronsH->size();
-    for( unsigned iele = 0; iele < nElectrons; ++iele )
+  
+  // Read GsfElectrons
+  edm::Handle<reco::GsfElectronCollection>  oldElectronsH ;
+  event.getByToken(inputElectronsToken_,oldElectronsH) ;
+  
+  // Read RecHits
+  edm::Handle< EcalRecHitCollection > pEBRecHits;
+  edm::Handle< EcalRecHitCollection > pEERecHits;
+  event.getByToken( recHitCollectionEBToken_, pEBRecHits );
+  event.getByToken( recHitCollectionEEToken_, pEERecHits );
+
+  // ReadValueMaps
+  edm::Handle<edm::ValueMap<double> > valMapEnergyH;
+  event.getByToken(energyRegToken_,valMapEnergyH);
+  edm::Handle<edm::ValueMap<double> > valMapEnergyErrorH;
+  event.getByToken(energyErrorRegToken_,valMapEnergyErrorH);
+  
+  // Prepare output collections
+  std::auto_ptr<reco::GsfElectronCollection> electrons( new reco::GsfElectronCollection ) ;
+  // Fillers for ValueMaps:
+  std::auto_ptr<edm::ValueMap<double> > regrNewEnergyMap(new edm::ValueMap<double>() );
+  edm::ValueMap<double>::Filler energyFiller(*regrNewEnergyMap);
+  
+  std::auto_ptr<edm::ValueMap<double> > regrNewEnergyErrorMap(new edm::ValueMap<double>() );
+  edm::ValueMap<double>::Filler energyErrorFiller(*regrNewEnergyErrorMap);
+  
+  // first clone the initial collection
+  unsigned nElectrons = oldElectronsH->size();
+  for( unsigned iele = 0; iele < nElectrons; ++iele )
     {
       electrons->push_back((*oldElectronsH)[iele]);
     }
-
-    std::vector<double> regressionValues;
-    std::vector<double> regressionErrorValues;
-    regressionValues.reserve(nElectrons);
-    regressionErrorValues.reserve(nElectrons);
-
-    if ( correctionsType != 0 )
+  
+  std::vector<double> regressionValues;
+  std::vector<double> regressionErrorValues;
+  regressionValues.reserve(nElectrons);
+  regressionErrorValues.reserve(nElectrons);
+  
+  if ( correctionsType != 0 )
     {
-        for ( unsigned iele = 0; iele < nElectrons ; ++iele)
+      for ( unsigned iele = 0; iele < nElectrons ; ++iele)
         {
-            reco::GsfElectron & ele  ( (*electrons)[iele]);
-            reco::GsfElectronRef elecRef(oldElectronsH,iele);
-            double regressionEnergy = (*valMapEnergyH)[elecRef];
-            double regressionEnergyError = (*valMapEnergyErrorH)[elecRef];
-
-            regressionValues.push_back(regressionEnergy);
-            regressionErrorValues.push_back(regressionEnergyError);
-
-            //    r9
-            const EcalRecHitCollection * recHits=0;
-            if( ele.isEB() )
+	  reco::GsfElectron & ele  ( (*electrons)[iele]);
+	  reco::GsfElectronRef elecRef(oldElectronsH,iele);
+	  double regressionEnergy = (*valMapEnergyH)[elecRef];
+	  double regressionEnergyError = (*valMapEnergyErrorH)[elecRef];
+	  
+	  regressionValues.push_back(regressionEnergy);
+	  regressionErrorValues.push_back(regressionEnergyError);
+	  
+	  //    r9
+	  const EcalRecHitCollection * recHits=0;
+	  if( ele.isEB() )
             {
-                recHits = pEBRecHits.product();
+	      recHits = pEBRecHits.product();
             } else recHits = pEERecHits.product();
-
-            SuperClusterHelper mySCHelper( &(ele), recHits, ecalTopology_, caloGeometry_ );
-
-            int elClass = -1;
-            int run = event.run();
-
-            float r9 = mySCHelper.r9();
-            double correctedEcalEnergy = ele.correctedEcalEnergy();
-            double correctedEcalEnergyError = ele.correctedEcalEnergyError();
-            double trackMomentum = ele.trackMomentumAtVtx().R();
-            double trackMomentumError = ele.trackMomentumError();
-            double combinedMomentum = ele.p();
-            double combinedMomentumError = ele.p4Error(ele.candidateP4Kind());
-            // FIXME : p4Error not filled for pure tracker electrons
-            // Recompute it using the parametrization implemented in
-            // RecoEgamma/EgammaElectronAlgos/src/ElectronEnergyCorrector.cc::simpleParameterizationUncertainty()
-            if( !ele.ecalDrivenSeed() )
+	  
+	  SuperClusterHelper mySCHelper( &(ele), recHits, ecalTopology_, caloGeometry_ );
+	  
+	  int elClass = -1;
+	  int run = event.run();
+	  
+	  float r9 = mySCHelper.r9();
+	  double correctedEcalEnergy = ele.correctedEcalEnergy();
+	  double correctedEcalEnergyError = ele.correctedEcalEnergyError();
+	  double trackMomentum = ele.trackMomentumAtVtx().R();
+	  double trackMomentumError = ele.trackMomentumError();
+	  double combinedMomentum = ele.p();
+	  double combinedMomentumError = ele.p4Error(ele.candidateP4Kind());
+	  // FIXME : p4Error not filled for pure tracker electrons
+	  // Recompute it using the parametrization implemented in
+	  // RecoEgamma/EgammaElectronAlgos/src/ElectronEnergyCorrector.cc::simpleParameterizationUncertainty()
+	  if( !ele.ecalDrivenSeed() )
             {
-                double error = 999. ;
-                double momentum = (combinedMomentum<15. ? 15. : combinedMomentum);
-                if ( ele.isEB() )
+	      double error = 999. ;
+	      double momentum = (combinedMomentum<15. ? 15. : combinedMomentum);
+	      if ( ele.isEB() )
                 {
-                    float parEB[3] = { 5.24e-02,  2.01e-01, 1.00e-02};
-                    error = momentum * sqrt( pow(parEB[0]/sqrt(momentum),2) + pow(parEB[1]/momentum,2) + pow(parEB[2],2) );
+		  float parEB[3] = { 5.24e-02,  2.01e-01, 1.00e-02};
+		  error = momentum * sqrt( pow(parEB[0]/sqrt(momentum),2) + pow(parEB[1]/momentum,2) + pow(parEB[2],2) );
                 }
-                else if ( ele.isEE() )
+	      else if ( ele.isEE() )
                 {
-                    float parEE[3] = { 1.46e-01, 9.21e-01, 1.94e-03} ;
-                    error = momentum * sqrt( pow(parEE[0]/sqrt(momentum),2) + pow(parEE[1]/momentum,2) + pow(parEE[2],2) );
+		  float parEE[3] = { 1.46e-01, 9.21e-01, 1.94e-03} ;
+		  error = momentum * sqrt( pow(parEE[0]/sqrt(momentum),2) + pow(parEE[1]/momentum,2) + pow(parEE[2],2) );
                 }
-                combinedMomentumError = error;
+	      combinedMomentumError = error;
             }
-
-            if (ele.classification() == reco::GsfElectron::GOLDEN) {elClass = 0;}
-            if (ele.classification() == reco::GsfElectron::BIGBREM) {elClass = 1;}
-            if (ele.classification() == reco::GsfElectron::BADTRACK) {elClass = 2;}
-            if (ele.classification() == reco::GsfElectron::SHOWERING) {elClass = 3;}
-            if (ele.classification() == reco::GsfElectron::GAP) {elClass = 4;}
-
-            SimpleElectron mySimpleElectron
-                (
-                    run,
-                    elClass,
-                    r9,
-                    correctedEcalEnergy,
-                    correctedEcalEnergyError,
-                    trackMomentum,
-                    trackMomentumError,
-                    regressionEnergy,
-                    regressionEnergyError,
-                    combinedMomentum,
-                    combinedMomentumError,
-                    ele.superCluster()->eta(),
-                    ele.isEB(),
-                    isMC,
-                    ele.ecalDriven(),
-                    ele.trackerDrivenSeed()
-                );
-
-            // energy calibration for ecalDriven electrons
-            if ( ele.core()->ecalDrivenSeed() || correctionsType==2 || combinationType==3 )
+	  
+	  if (ele.classification() == reco::GsfElectron::GOLDEN) {elClass = 0;}
+	  if (ele.classification() == reco::GsfElectron::BIGBREM) {elClass = 1;}
+	  if (ele.classification() == reco::GsfElectron::BADTRACK) {elClass = 2;}
+	  if (ele.classification() == reco::GsfElectron::SHOWERING) {elClass = 3;}
+	  if (ele.classification() == reco::GsfElectron::GAP) {elClass = 4;}
+	  
+	  SimpleElectron mySimpleElectron
+	    (
+	     run,
+	     elClass,
+	     r9,
+	     correctedEcalEnergy,
+	     correctedEcalEnergyError,
+	     trackMomentum,
+	     trackMomentumError,
+	     regressionEnergy,
+	     regressionEnergyError,
+	     combinedMomentum,
+	     combinedMomentumError,
+	     ele.superCluster()->eta(),
+	     ele.isEB(),
+	     isMC,
+	     ele.ecalDriven(),
+	     ele.trackerDrivenSeed()
+	     );
+	  
+	  // energy calibration for ecalDriven electrons
+	  if ( ele.core()->ecalDrivenSeed() || correctionsType==2 || combinationType==3 )
             {
-                theEnCorrector->calibrate(mySimpleElectron, event.streamID());
-
-                // E-p combination
-
-                switch ( combinationType )
+	      theEnCorrector->calibrate(mySimpleElectron, event.streamID());
+	      
+	      // E-p combination
+	      
+	      switch ( combinationType )
                 {
-                    case 0:
-                        if ( verbose )
-                        {
-                            std::cout << "[CalibratedGsfElectronProducer] "
-                            << "You choose not to combine." << std::endl;
-                        }
-                        break;
-                    case 1:
-                        if ( verbose )
-                        {
-                            std::cout << "[CalibratedGsfElectronProducer] "
-                            << "You choose corrected regression energy for standard combination" << std::endl;
-                        }
-                        myCombinator->setCombinationMode(1);
-                        myCombinator->combine(mySimpleElectron);
-                        break;
-                    case 2:
-                        if ( verbose )
-                        {
-                            std::cout << "[CalibratedGsfElectronProducer] "
-                            << "You choose uncorrected regression energy for standard combination" << std::endl;
-                        }
-                        myCombinator->setCombinationMode(2);
-                        myCombinator->combine(mySimpleElectron);
-                        break;
-                    case 3:
-                        if ( verbose )
-                        {
-                            std::cout << "[CalibratedGsfElectronProducer] "
-                            << "You choose regression combination." << std::endl;
-                        }
-                        myEpCombinationTool->combine(mySimpleElectron);
-                        theEnCorrector->correctLinearity(mySimpleElectron);
-                        break;
-                    default:
-  		                throw cms::Exception("CalibratedgsfElectronProducer|ConfigError")
-                            << "Unknown combination Type !!!" ;
+		case 0:
+		  if ( verbose )
+		    {
+		      std::cout << "[CalibratedGsfElectronProducer] "
+				<< "You choose not to combine." << std::endl;
+		    }
+		  break;
+		case 1:
+		  if ( verbose )
+		    {
+		      std::cout << "[CalibratedGsfElectronProducer] "
+				<< "You choose corrected regression energy for standard combination" << std::endl;
+		    }
+		  myCombinator->setCombinationMode(1);
+		  myCombinator->combine(mySimpleElectron);
+		  break;
+		case 2:
+		  if ( verbose )
+		    {
+		      std::cout << "[CalibratedGsfElectronProducer] "
+				<< "You choose uncorrected regression energy for standard combination" << std::endl;
+		    }
+		  myCombinator->setCombinationMode(2);
+		  myCombinator->combine(mySimpleElectron);
+		  break;
+		case 3:
+		  if ( verbose )
+		    {
+		      std::cout << "[CalibratedGsfElectronProducer] "
+				<< "You choose regression combination." << std::endl;
+		    }
+		  myEpCombinationTool->combine(mySimpleElectron);
+		  theEnCorrector->correctLinearity(mySimpleElectron);
+		  break;
+		default:
+		  throw cms::Exception("CalibratedgsfElectronProducer|ConfigError")
+		    << "Unknown combination Type !!!" ;
                 }
-
-                math::XYZTLorentzVector oldMomentum = ele.p4() ;
-                math::XYZTLorentzVector newMomentum_ ;
-                newMomentum_ = math::XYZTLorentzVector
-                 ( oldMomentum.x()*mySimpleElectron.getCombinedMomentum()/oldMomentum.t(),
-                   oldMomentum.y()*mySimpleElectron.getCombinedMomentum()/oldMomentum.t(),
-                   oldMomentum.z()*mySimpleElectron.getCombinedMomentum()/oldMomentum.t(),
-                   mySimpleElectron.getCombinedMomentum() ) ;
-
-                ele.correctMomentum
-                    (
-                        newMomentum_,
-                        mySimpleElectron.getTrackerMomentumError(),
-                        mySimpleElectron.getCombinedMomentumError()
-                    );
-
-                if ( verbose )
+	      
+	      math::XYZTLorentzVector oldMomentum = ele.p4() ;
+	      math::XYZTLorentzVector newMomentum_ ;
+	      newMomentum_ = math::XYZTLorentzVector
+		( oldMomentum.x()*mySimpleElectron.getCombinedMomentum()/oldMomentum.t(),
+		  oldMomentum.y()*mySimpleElectron.getCombinedMomentum()/oldMomentum.t(),
+		  oldMomentum.z()*mySimpleElectron.getCombinedMomentum()/oldMomentum.t(),
+		  mySimpleElectron.getCombinedMomentum() ) ;
+	      
+	      ele.correctMomentum
+		(
+		 newMomentum_,
+		 mySimpleElectron.getTrackerMomentumError(),
+		 mySimpleElectron.getCombinedMomentumError()
+		 );
+	      
+	      if ( verbose )
                 {
-                    std::cout << "[CalibratedGsfElectronProducer] Combined momentum after saving "
-                        << ele.p4().t() << std::endl;
+		  std::cout << "[CalibratedGsfElectronProducer] Combined momentum after saving "
+			    << ele.p4().t() << std::endl;
                 }
             }// end of if (ele.core()->ecalDrivenSeed())
         }// end of loop on electrons
     } else
     {
-        if ( verbose )
+      if ( verbose )
         {
-            std::cout << "[CalibratedGsfElectronProducer] "
-            << "You choose not to correct. Uncorrected Regression Energy is taken." << std::endl;
+	  std::cout << "[CalibratedGsfElectronProducer] "
+		    << "You choose not to correct. Uncorrected Regression Energy is taken." << std::endl;
         }
     }
-
-    // Save the electrons
-    const edm::OrphanHandle<reco::GsfElectronCollection> gsfNewElectronHandle = event.put(electrons, newElectronName_) ;
-    energyFiller.insert(gsfNewElectronHandle,regressionValues.begin(),regressionValues.end());
-    energyFiller.fill();
-    energyErrorFiller.insert(gsfNewElectronHandle,regressionErrorValues.begin(),regressionErrorValues.end());
-    energyErrorFiller.fill();
-
-    event.put(regrNewEnergyMap,nameNewEnergyReg_);
-    event.put(regrNewEnergyErrorMap,nameNewEnergyErrorReg_);
+  
+  // Save the electrons
+  const edm::OrphanHandle<reco::GsfElectronCollection> gsfNewElectronHandle = event.put(electrons, newElectronName_) ;
+  energyFiller.insert(gsfNewElectronHandle,regressionValues.begin(),regressionValues.end());
+  energyFiller.fill();
+  energyErrorFiller.insert(gsfNewElectronHandle,regressionErrorValues.begin(),regressionErrorValues.end());
+  energyErrorFiller.fill();
+  
+  event.put(regrNewEnergyMap,nameNewEnergyReg_);
+  event.put(regrNewEnergyErrorMap,nameNewEnergyErrorReg_);
 }
 
 #include "FWCore/Framework/interface/MakerMacros.h"
diff --git a/EgammaAnalysis/ElectronTools/plugins/CalibratedPatElectronProducer.cc b/EgammaAnalysis/ElectronTools/plugins/CalibratedPatElectronProducer.cc
index bdac82f..9004b91 100644
--- a/EgammaAnalysis/ElectronTools/plugins/CalibratedPatElectronProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/CalibratedPatElectronProducer.cc
@@ -39,15 +39,9 @@
 
 #include <iostream>
 
-using namespace edm ;
-using namespace std ;
-using namespace reco ;
-using namespace pat ;
-
 CalibratedPatElectronProducer::CalibratedPatElectronProducer( const edm::ParameterSet & cfg )
-// : PatElectronBaseProducer(cfg)
 {
-    produces<ElectronCollection>();
+  produces<pat::ElectronCollection>();
 
     inputPatElectronsToken = consumes<edm::View<reco::Candidate> >(cfg.getParameter<edm::InputTag>("inputPatElectronsTag"));
     dataset = cfg.getParameter<std::string>("inputDataset");
@@ -160,9 +154,9 @@ void CalibratedPatElectronProducer::produce( edm::Event & event, const edm::Even
 
     edm::Handle<edm::View<reco::Candidate> > oldElectrons ;
     event.getByToken(inputPatElectronsToken,oldElectrons) ;
-    std::auto_ptr<ElectronCollection> electrons( new ElectronCollection ) ;
-    ElectronCollection::const_iterator electron ;
-    ElectronCollection::iterator ele ;
+    std::auto_ptr<pat::ElectronCollection> electrons( new pat::ElectronCollection ) ;
+    pat::ElectronCollection::const_iterator electron ;
+    pat::ElectronCollection::iterator ele ;
     // first clone the initial collection
     for
         (
@@ -195,7 +189,7 @@ void CalibratedPatElectronProducer::produce( edm::Event & event, const edm::Even
             double trackMomentumError = ele->trackMomentumError();
             double combinedMomentum = ele->p();
             double combinedMomentumError = 0;
-            if ( ele->candidateP4Kind() != GsfElectron::P4_UNKNOWN )
+            if ( ele->candidateP4Kind() != reco::GsfElectron::P4_UNKNOWN )
             {
               combinedMomentumError = ele->p4Error(ele->candidateP4Kind());
             }
diff --git a/EgammaAnalysis/ElectronTools/plugins/ElectronIdMVAProducer.cc b/EgammaAnalysis/ElectronTools/plugins/ElectronIdMVAProducer.cc
index ab6822b..da432ab 100644
--- a/EgammaAnalysis/ElectronTools/plugins/ElectronIdMVAProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/ElectronIdMVAProducer.cc
@@ -21,8 +21,6 @@
 // class declaration
 //
 
-using namespace std;
-using namespace reco;
 class ElectronIdMVAProducer : public edm::EDFilter {
 public:
   explicit ElectronIdMVAProducer(const edm::ParameterSet&);
@@ -40,8 +38,8 @@ private:
   edm::EDGetTokenT<EcalRecHitCollection> reducedEERecHitCollectionToken_;
   
   double _Rho;
-  string method_;
-  vector<string> mvaWeightFiles_;
+  std::string method_;
+  std::vector<std::string> mvaWeightFiles_;
   bool Trig_;
   bool NoIP_;
   
@@ -66,8 +64,8 @@ ElectronIdMVAProducer::ElectronIdMVAProducer(const edm::ParameterSet& iConfig) {
   eventrhoToken_ = consumes<double>(edm::InputTag("kt6PFJets", "rho"));
   reducedEBRecHitCollectionToken_ = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEBRecHitCollection"));
   reducedEERecHitCollectionToken_ = consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("reducedEERecHitCollection"));
-  method_ = iConfig.getParameter<string>("method");
-  std::vector<string> fpMvaWeightFiles = iConfig.getParameter<std::vector<std::string> >("mvaWeightFile");
+  method_ = iConfig.getParameter<std::string>("method");
+  std::vector<std::string> fpMvaWeightFiles = iConfig.getParameter<std::vector<std::string> >("mvaWeightFile");
   Trig_ = iConfig.getParameter<bool>("Trig");
   NoIP_ = iConfig.getParameter<bool>("NoIP");
   
@@ -84,7 +82,7 @@ ElectronIdMVAProducer::ElectronIdMVAProducer(const edm::ParameterSet& iConfig) {
   
   bool manualCat_ = true;
   
-  string path_mvaWeightFileEleID;
+  std::string path_mvaWeightFileEleID;
   for(unsigned ifile=0 ; ifile < fpMvaWeightFiles.size() ; ++ifile) {
     path_mvaWeightFileEleID = edm::FileInPath ( fpMvaWeightFiles[ifile].c_str() ).fullPath();
     mvaWeightFiles_.push_back(path_mvaWeightFileEleID);
@@ -110,24 +108,23 @@ ElectronIdMVAProducer::~ElectronIdMVAProducer()
 
 // ------------ method called on each new Event  ------------
 bool ElectronIdMVAProducer::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
-  using namespace edm;
-  
+
   std::auto_ptr<edm::ValueMap<float> > out(new edm::ValueMap<float>() );
   
-  Handle<reco::VertexCollection>  vertexCollection;
+  edm::Handle<reco::VertexCollection>  vertexCollection;
   iEvent.getByToken(vertexToken_, vertexCollection);
   
-  Vertex dummy;
-  const Vertex *pv = &dummy;
+  reco::Vertex dummy;
+  const reco::Vertex *pv = &dummy;
   if ( vertexCollection->size() != 0) {
     pv = &*vertexCollection->begin();
   } else { // create a dummy PV
-    Vertex::Error e;
+    reco::Vertex::Error e;
     e(0, 0) = 0.0015 * 0.0015;
     e(1, 1) = 0.0015 * 0.0015;
     e(2, 2) = 15. * 15.;
-    Vertex::Point p(0, 0, 0);
-    dummy = Vertex(p, e, 0, 0, 0);
+    reco::Vertex::Point p(0, 0, 0);
+    dummy = reco::Vertex(p, e, 0, 0, 0);
   }
   
   EcalClusterLazyTools lazyTools(iEvent, iSetup, reducedEBRecHitCollectionToken_, reducedEERecHitCollectionToken_);
@@ -136,7 +133,7 @@ bool ElectronIdMVAProducer::filter(edm::Event& iEvent, const edm::EventSetup& iS
   iSetup.get<TransientTrackRecord>().get("TransientTrackBuilder", builder);
   TransientTrackBuilder thebuilder = *(builder.product());
   
-  Handle<reco::GsfElectronCollection> egCollection;
+  edm::Handle<reco::GsfElectronCollection> egCollection;
   iEvent.getByToken(electronToken_,egCollection);
   const reco::GsfElectronCollection egCandidates = (*egCollection.product());
   
diff --git a/EgammaAnalysis/ElectronTools/plugins/ElectronPATIdMVAProducer.cc b/EgammaAnalysis/ElectronTools/plugins/ElectronPATIdMVAProducer.cc
index e884bdd..68ee1e9 100644
--- a/EgammaAnalysis/ElectronTools/plugins/ElectronPATIdMVAProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/ElectronPATIdMVAProducer.cc
@@ -21,8 +21,6 @@
 // class declaration
 //
 
-using namespace std;
-using namespace reco;
 class ElectronPATIdMVAProducer : public edm::EDProducer {
 	public:
 		explicit ElectronPATIdMVAProducer(const edm::ParameterSet&);
@@ -36,8 +34,8 @@ class ElectronPATIdMVAProducer : public edm::EDProducer {
   edm::EDGetTokenT<pat::ElectronCollection> electronToken_;
   double _Rho;
   edm::EDGetTokenT<double> eventrhoToken_;
-  string method_;
-  vector<string> mvaWeightFiles_;
+  std::string method_;
+  std::vector<std::string> mvaWeightFiles_;
 
 
   EGammaMvaEleEstimator* mvaID_;
@@ -58,8 +56,8 @@ class ElectronPATIdMVAProducer : public edm::EDProducer {
 ElectronPATIdMVAProducer::ElectronPATIdMVAProducer(const edm::ParameterSet& iConfig) {
         verbose_ = iConfig.getUntrackedParameter<bool>("verbose", false);
 	electronToken_ = consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electronTag"));
-	method_ = iConfig.getParameter<string>("method");
-	std::vector<string> fpMvaWeightFiles = iConfig.getParameter<std::vector<std::string> >("mvaWeightFile");
+	method_ = iConfig.getParameter<std::string>("method");
+	std::vector<std::string> fpMvaWeightFiles = iConfig.getParameter<std::vector<std::string> >("mvaWeightFile");
 	eventrhoToken_ = consumes<double>(iConfig.getParameter<edm::InputTag>("Rho"));
 
         produces<edm::ValueMap<float> >();
@@ -75,7 +73,7 @@ ElectronPATIdMVAProducer::ElectronPATIdMVAProducer(const edm::ParameterSet& iCon
 
         bool manualCat_ = true;
 
-	string path_mvaWeightFileEleID;
+	std::string path_mvaWeightFileEleID;
 	for(unsigned ifile=0 ; ifile < fpMvaWeightFiles.size() ; ++ifile) {
 	  path_mvaWeightFileEleID = edm::FileInPath ( fpMvaWeightFiles[ifile].c_str() ).fullPath();
 	  mvaWeightFiles_.push_back(path_mvaWeightFileEleID);
@@ -101,7 +99,6 @@ ElectronPATIdMVAProducer::~ElectronPATIdMVAProducer()
 
 // ------------ method called on each new Event  ------------
 void ElectronPATIdMVAProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {
-	using namespace edm;
 
         std::auto_ptr<edm::ValueMap<float> > out(new edm::ValueMap<float>() );
 
@@ -111,7 +108,7 @@ void ElectronPATIdMVAProducer::produce(edm::Event& iEvent, const edm::EventSetup
 
 
 
-	Handle<pat::ElectronCollection> egCollection;
+	edm::Handle<pat::ElectronCollection> egCollection;
 	iEvent.getByToken(electronToken_,egCollection);
         const pat::ElectronCollection egCandidates = (*egCollection.product());
 
diff --git a/EgammaAnalysis/ElectronTools/plugins/ElectronRegressionEnergyProducer.cc b/EgammaAnalysis/ElectronTools/plugins/ElectronRegressionEnergyProducer.cc
index 781262f..1b3fc64 100644
--- a/EgammaAnalysis/ElectronTools/plugins/ElectronRegressionEnergyProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/ElectronRegressionEnergyProducer.cc
@@ -27,10 +27,6 @@
 // class declaration
 //
 
-using namespace std;
-using namespace reco;
-using namespace edm;
-
 class ElectronRegressionEnergyProducer : public edm::EDFilter {
 public:
   explicit ElectronRegressionEnergyProducer(const edm::ParameterSet&);
@@ -127,7 +123,7 @@ bool ElectronRegressionEnergyProducer::filter(edm::Event& iEvent, const edm::Eve
   std::auto_ptr<edm::ValueMap<double> > regrEnergyErrorMap(new edm::ValueMap<double>() );
   edm::ValueMap<double>::Filler energyErrorFiller(*regrEnergyErrorMap);
 
-  Handle<reco::GsfElectronCollection> egCollection;
+  edm::Handle<reco::GsfElectronCollection> egCollection;
   iEvent.getByToken(electronToken_,egCollection);
   const reco::GsfElectronCollection egCandidates = (*egCollection.product());
 
@@ -147,7 +143,7 @@ bool ElectronRegressionEnergyProducer::filter(edm::Event& iEvent, const edm::Eve
   //**************************************************************************
   //Get Number of Vertices
   //**************************************************************************
-  Handle<reco::VertexCollection> hVertexProduct;
+  edm::Handle<reco::VertexCollection> hVertexProduct;
   iEvent.getByToken(hVertexToken_,hVertexProduct);
   const reco::VertexCollection inVertices = *(hVertexProduct.product());
 
@@ -169,7 +165,7 @@ bool ElectronRegressionEnergyProducer::filter(edm::Event& iEvent, const edm::Eve
   //Get Rho
   //**************************************************************************
   double rho = 0;
-  Handle<double> hRhoKt6PFJets;
+  edm::Handle<double> hRhoKt6PFJets;
   iEvent.getByToken(hRhoKt6PFJetsToken_, hRhoKt6PFJets);
   rho = (*hRhoKt6PFJets);
 
diff --git a/EgammaAnalysis/ElectronTools/plugins/PhotonIsoProducer.cc b/EgammaAnalysis/ElectronTools/plugins/PhotonIsoProducer.cc
index d3ba264..00c4631 100644
--- a/EgammaAnalysis/ElectronTools/plugins/PhotonIsoProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/PhotonIsoProducer.cc
@@ -22,8 +22,6 @@
 // class declaration
 //
 
-using namespace std;
-using namespace reco;
 class PhotonIsoProducer : public edm::EDFilter {
       public:
          explicit PhotonIsoProducer(const edm::ParameterSet&);
@@ -90,7 +88,6 @@ PhotonIsoProducer::~PhotonIsoProducer()
 
 // ------------ method called on each new Event  ------------
 bool PhotonIsoProducer::filter(edm::Event& iEvent, const edm::EventSetup& iSetup) {
-	using namespace edm;
 
         std::auto_ptr<edm::ValueMap<double> > chIsoMap(new edm::ValueMap<double>() );
 	edm::ValueMap<double>::Filler chFiller(*chIsoMap);
@@ -101,17 +98,17 @@ bool PhotonIsoProducer::filter(edm::Event& iEvent, const edm::EventSetup& iSetup
         std::auto_ptr<edm::ValueMap<double> > nhIsoMap(new edm::ValueMap<double>() );
 	edm::ValueMap<double>::Filler nhFiller(*nhIsoMap);
 
-	Handle<reco::VertexCollection>  vertexCollection;
+	edm::Handle<reco::VertexCollection>  vertexCollection;
 	iEvent.getByToken(vertexToken_, vertexCollection);
 
-	Handle<reco::PhotonCollection> phoCollection;
+	edm::Handle<reco::PhotonCollection> phoCollection;
 	iEvent.getByToken(photonToken_, phoCollection);
 	const reco::PhotonCollection *recoPho = phoCollection.product();
 
 	// All PF Candidate for alternate isolation
-	Handle<reco::PFCandidateCollection> pfCandidatesH;
+	edm::Handle<reco::PFCandidateCollection> pfCandidatesH;
 	iEvent.getByToken(particleFlowToken_, pfCandidatesH);
-	const  PFCandidateCollection thePfColl = *(pfCandidatesH.product());
+	const  reco::PFCandidateCollection thePfColl = *(pfCandidatesH.product());
 
         std::vector<double> chIsoValues;
 	std::vector<double> phIsoValues;
@@ -121,7 +118,7 @@ bool PhotonIsoProducer::filter(edm::Event& iEvent, const edm::EventSetup& iSetup
         nhIsoValues.reserve(phoCollection->size());
 
 	unsigned int ivtx = 0;
-	VertexRef myVtxRef(vertexCollection, ivtx);
+	reco::VertexRef myVtxRef(vertexCollection, ivtx);
 
 
         for (reco::PhotonCollection::const_iterator aPho = recoPho->begin(); aPho != recoPho->end(); ++aPho) {
diff --git a/EgammaAnalysis/ElectronTools/plugins/RegressionEnergyPatElectronProducer.cc b/EgammaAnalysis/ElectronTools/plugins/RegressionEnergyPatElectronProducer.cc
index 0ddae78..abbdcd5 100644
--- a/EgammaAnalysis/ElectronTools/plugins/RegressionEnergyPatElectronProducer.cc
+++ b/EgammaAnalysis/ElectronTools/plugins/RegressionEnergyPatElectronProducer.cc
@@ -14,20 +14,14 @@
 
 #include <iostream>
 
-using namespace edm ;
-using namespace std ;
-using namespace reco ;
-using namespace pat ;
-
-
 RegressionEnergyPatElectronProducer::RegressionEnergyPatElectronProducer( const edm::ParameterSet & cfg )
 {
 
-  inputGsfElectronsToken_ = mayConsume<GsfElectronCollection>(cfg.getParameter<edm::InputTag>("inputElectronsTag"));
-  inputPatElectronsToken_ = mayConsume<ElectronCollection>(cfg.getParameter<edm::InputTag>("inputElectronsTag"));
+  inputGsfElectronsToken_ = mayConsume<reco::GsfElectronCollection>(cfg.getParameter<edm::InputTag>("inputElectronsTag"));
+  inputPatElectronsToken_ = mayConsume<pat::ElectronCollection>(cfg.getParameter<edm::InputTag>("inputElectronsTag"));
   inputCollectionType_ = cfg.getParameter<uint32_t>("inputCollectionType");
   rhoInputToken_ = consumes<double>(cfg.getParameter<edm::InputTag>("rhoCollection"));
-  verticesInputToken_ = consumes<VertexCollection>(cfg.getParameter<edm::InputTag>("vertexCollection"));
+  verticesInputToken_ = consumes<reco::VertexCollection>(cfg.getParameter<edm::InputTag>("vertexCollection"));
   energyRegressionType_ = cfg.getParameter<uint32_t>("energyRegressionType");
   regressionInputFile_ = cfg.getParameter<std::string>("regressionInputFile");
   recHitCollectionEBToken_ = mayConsume<EcalRecHitCollection>(cfg.getParameter<edm::InputTag>("recHitCollectionEB"));
@@ -50,7 +44,7 @@ RegressionEnergyPatElectronProducer::RegressionEnergyPatElectronProducer( const
   if (inputCollectionType_ == 0) {
     // do nothing
   } else if (inputCollectionType_ == 1) {
-    produces<ElectronCollection>();
+    produces<pat::ElectronCollection>();
   } else {
     throw cms::Exception("InconsistentParameters")  << " inputCollectionType should be either 0 (GsfElectrons) or 1 (pat::Electrons) " << std::endl;
   }
@@ -107,7 +101,7 @@ void RegressionEnergyPatElectronProducer::produce( edm::Event & event, const edm
   //**************************************************************************
   //Get Number of Vertices
   //**************************************************************************
-  Handle<reco::VertexCollection> hVertexProduct;
+  edm::Handle<reco::VertexCollection> hVertexProduct;
   event.getByToken(verticesInputToken_,hVertexProduct);
   const reco::VertexCollection inVertices = *(hVertexProduct.product());
 
@@ -129,7 +123,7 @@ void RegressionEnergyPatElectronProducer::produce( edm::Event & event, const edm
   //Get Rho
   //**************************************************************************
   double rho = 0;
-  Handle<double> hRhoKt6PFJets;
+  edm::Handle<double> hRhoKt6PFJets;
   event.getByToken(rhoInputToken_, hRhoKt6PFJets);
   rho = (*hRhoKt6PFJets);
 
@@ -144,8 +138,8 @@ void RegressionEnergyPatElectronProducer::produce( edm::Event & event, const edm
     event.getByToken( recHitCollectionEEToken_, pEERecHits );
   }
 
-  edm::Handle<GsfElectronCollection> gsfCollectionH ;
-  edm::Handle<ElectronCollection> patCollectionH;
+  edm::Handle<reco::GsfElectronCollection> gsfCollectionH ;
+  edm::Handle<pat::ElectronCollection> patCollectionH;
   if ( inputCollectionType_ == 0 ) {
     event.getByToken ( inputGsfElectronsToken_,gsfCollectionH )  ;
     nElectrons_ = gsfCollectionH->size();
@@ -156,7 +150,7 @@ void RegressionEnergyPatElectronProducer::produce( edm::Event & event, const edm
   }
 
   // prepare the two even if only one is used
-  std::auto_ptr<ElectronCollection> patElectrons( new ElectronCollection ) ;
+  std::auto_ptr<pat::ElectronCollection> patElectrons( new pat::ElectronCollection ) ;
 
   // Fillers for ValueMaps:
   std::auto_ptr<edm::ValueMap<double> > regrEnergyMap(new edm::ValueMap<double>() );
@@ -175,7 +169,7 @@ void RegressionEnergyPatElectronProducer::produce( edm::Event & event, const edm
 
   for(unsigned iele=0; iele < nElectrons_ ; ++iele) {
 
-    const GsfElectron * ele = ( inputCollectionType_ == 0 ) ? &(*gsfCollectionH)[iele] : &(*patCollectionH)[iele] ;
+    const reco::GsfElectron * ele = ( inputCollectionType_ == 0 ) ? &(*gsfCollectionH)[iele] : &(*patCollectionH)[iele] ;
     if (debug_) {
       std::cout << "***********************************************************************\n";
       std::cout << "Run Lumi Event: " << event.id().run() << " " << event.luminosityBlock() << " " << event.id().event() << "\n";
@@ -536,7 +530,7 @@ void RegressionEnergyPatElectronProducer::produce( edm::Event & event, const edm
 	energyValues.push_back(RegressionMomentum);
 	energyErrorValues.push_back(RegressionMomentumError);
       } else {
-	cout << "Error: RegressionType = " << energyRegressionType_ << " is not supported.\n";
+	std::cout << "Error: RegressionType = " << energyRegressionType_ << " is not supported.\n";
       }
 
       if(inputCollectionType_ == 1) {
diff --git a/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimator.cc b/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimator.cc
index 13aff41..03e2cd0 100644
--- a/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimator.cc
+++ b/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimator.cc
@@ -2,7 +2,6 @@
 #include "EgammaAnalysis/ElectronTools/interface/EGammaMvaEleEstimator.h"
 #include <cmath>
 #include <vector>
-using namespace std;
 
 #ifndef STANDALONE
 #include "DataFormats/TrackReco/interface/Track.h"
@@ -22,7 +21,6 @@ using namespace std;
 #include "DataFormats/Common/interface/RefToPtr.h"
 #include <cstdio>
 #include <zlib.h>
-using namespace reco;
 #endif
 
 //--------------------------------------------------------------------------------------------------
@@ -486,7 +484,7 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
   }
 
   if (fMVAType != EGammaMvaEleEstimator::kTrig) {
-    std::cout << "Error: This method should be called for kTrig MVA only" << endl;
+    std::cout << "Error: This method should be called for kTrig MVA only" <<std::endl;
     return -9999;
   }
 
@@ -530,8 +528,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
   }
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+    std::cout << " *** Inside the class fMethodname " << fMethodname << std::endl;
+    std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -552,8 +550,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
 	 << " d0 " << fMVAVar_d0  
 	 << " ip3d " << fMVAVar_ip3d  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+    std::cout << " ### MVA " << mva << std::endl;
   }
 
 
@@ -591,7 +589,7 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
   }
 
   if (fMVAType != EGammaMvaEleEstimator::kTrigNoIP) {
-    std::cout << "Error: This method should be called for kTrigNoIP MVA only" << endl;
+    std::cout << "Error: This method should be called for kTrigNoIP MVA only" <<std::endl;
     return -9999;
   }
 
@@ -633,8 +631,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
   }
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+    std::cout << " *** Inside the class fMethodname " << fMethodname << std::endl;
+    std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -654,8 +652,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
 	 << " rho " << fMVAVar_rho 
 	 << " PreShowerOverRaw " << fMVAVar_PreShowerOverRaw  
        	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+    std::cout << " ### MVA " << mva << std::endl;
   }
 
 
@@ -695,7 +693,7 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
   }
 
   if (fMVAType != EGammaMvaEleEstimator::kNonTrig) {
-    std::cout << "Error: This method should be called for kNonTrig MVA only" << endl;
+    std::cout << "Error: This method should be called for kNonTrig MVA only" <<std::endl;
     return -9999;
   }
 
@@ -739,8 +737,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
 
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+    std::cout << " *** Inside the class fMethodname " << fMethodname << std::endl;
+    std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -759,8 +757,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(Double_t fbrem,
 	 << " eleEoPout " << fMVAVar_eleEoPout  
 	 << " PreShowerOverRaw " << fMVAVar_PreShowerOverRaw  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+    std::cout << " ### MVA " << mva << std::endl;
   }
 
 
@@ -827,7 +825,7 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(Double_t fbrem,
   fMVAVar_spp             = spp;
   fMVAVar_etawidth        = etawidth;
   fMVAVar_phiwidth        = phiwidth;
-  fMVAVar_OneMinusE1x5E5x5= max(min(double(OneMinusE1x5E5x5),2.0),-1.0);
+  fMVAVar_OneMinusE1x5E5x5= std::max(std::min(double(OneMinusE1x5E5x5),2.0),-1.0);
   fMVAVar_R9              = (R9 > 5) ? 5: R9;
 
   fMVAVar_HoE             = HoE;
@@ -868,8 +866,8 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(Double_t fbrem,
   }
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+    std::cout << " *** Inside the class fMethodname " << fMethodname << std::endl;
+    std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -906,8 +904,8 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(Double_t fbrem,
          << " NeutralHadronIso_DR0p4To0p5 " <<  NeutralHadronIso_DR0p4To0p5
          << " Rho " <<  Rho
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+    std::cout << " ### MVA " << mva << std::endl;
   }
 
   return mva;
@@ -965,27 +963,27 @@ Double_t EGammaMvaEleEstimator::isoMvaValue(Double_t Pt,
   Double_t mva = fTMVAReader[bin]->EvaluateMVA(fTMVAMethod[bin]);
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout  << "ChargedIso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << std::endl;
+   std::cout  << "ChargedIso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_ChargedIso_DR0p0To0p1   << " "
           << fMVAVar_ChargedIso_DR0p1To0p2   << " "
           << fMVAVar_ChargedIso_DR0p2To0p3 << " "
           << fMVAVar_ChargedIso_DR0p3To0p4 << " "
-          << fMVAVar_ChargedIso_DR0p4To0p5 << endl;
-    cout  << "PF Gamma Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+          << fMVAVar_ChargedIso_DR0p4To0p5 <<std::endl;
+   std::cout  << "PF Gamma Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_GammaIso_DR0p0To0p1 << " "
           << fMVAVar_GammaIso_DR0p1To0p2 << " "
           << fMVAVar_GammaIso_DR0p2To0p3 << " "
           << fMVAVar_GammaIso_DR0p3To0p4 << " "
-          << fMVAVar_GammaIso_DR0p4To0p5 << endl;
-    cout  << "PF Neutral Hadron Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+          << fMVAVar_GammaIso_DR0p4To0p5 <<std::endl;
+   std::cout  << "PF Neutral Hadron Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_NeutralHadronIso_DR0p0To0p1 << " "
           << fMVAVar_NeutralHadronIso_DR0p1To0p2 << " "
           << fMVAVar_NeutralHadronIso_DR0p2To0p3 << " "
           << fMVAVar_NeutralHadronIso_DR0p3To0p4 << " "
           << fMVAVar_NeutralHadronIso_DR0p4To0p5 << " "
-          << endl;
-    cout << " ### MVA " << mva << endl;
+          <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
   return mva;
@@ -1008,7 +1006,7 @@ Double_t EGammaMvaEleEstimator::mvaValue(const reco::GsfElectron& ele,
   }
 
   if ( (fMVAType != EGammaMvaEleEstimator::kTrig) && (fMVAType != EGammaMvaEleEstimator::kNonTrig )) {
-    std::cout << "Error: This method should be called for kTrig or kNonTrig MVA only" << endl;
+    std::cout << "Error: This method should be called for kTrig or kNonTrig MVA only" <<std::endl;
     return -9999;
   }
   
@@ -1098,8 +1096,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const reco::GsfElectron& ele,
 
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -1119,8 +1117,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const reco::GsfElectron& ele,
 	 << " d0 " << fMVAVar_d0  
 	 << " ip3d " << fMVAVar_ip3d  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
 
@@ -1143,7 +1141,7 @@ Double_t EGammaMvaEleEstimator::mvaValue(const reco::GsfElectron& ele,
   }
   
   if (fMVAType != EGammaMvaEleEstimator::kTrigNoIP) {
-    std::cout << "Error: This method should be called for kTrigNoIP MVA only" << endl;
+    std::cout << "Error: This method should be called for kTrigNoIP MVA only" <<std::endl;
     return -9999;
   }
 
@@ -1208,8 +1206,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const reco::GsfElectron& ele,
 
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -1231,8 +1229,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const reco::GsfElectron& ele,
 	 << " rho " << fMVAVar_rho
       // << " EoPout " << fMVAVar_EoPout  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
 
@@ -1333,8 +1331,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const pat::Electron& ele,
   }
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout << " fbrem " <<  fMVAVar_fbrem
          << " kfchi2 " << fMVAVar_kfchi2
          << " mykfhits " << fMVAVar_kfhits
          << " gsfchi2 " << fMVAVar_gsfchi2
@@ -1358,8 +1356,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const pat::Electron& ele,
          << " d0 " << fMVAVar_d0
          << " ip3d " << fMVAVar_ip3d
          << " eta " << fMVAVar_eta
-         << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+         << " pt " << fMVAVar_pt <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
   return mva;
@@ -1377,7 +1375,7 @@ Double_t EGammaMvaEleEstimator::mvaValue(const pat::Electron& ele,
   }
 
   if ( (fMVAType != EGammaMvaEleEstimator::kTrigNoIP) ) {
-    std::cout << "Error: This method should be called for kTrigNoIP mva only" << endl;
+    std::cout << "Error: This method should be called for kTrigNoIP mva only" <<std::endl;
     return -9999;
   }
   
@@ -1440,8 +1438,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const pat::Electron& ele,
 
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -1463,8 +1461,8 @@ Double_t EGammaMvaEleEstimator::mvaValue(const pat::Electron& ele,
 	 << " rho " << fMVAVar_rho
       // << " EoPout " << fMVAVar_EoPout  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout << " ### MVA " << mva << endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
 
@@ -1634,7 +1632,7 @@ Double_t EGammaMvaEleEstimator::isoMvaValue(const reco::GsfElectron& ele,
   fMVAVar_NeutralHadronIso_DR0p4To0p5 = TMath::Max(TMath::Min((tmpNeutralHadronIso_DR0p4To0p5 - Rho*ElectronEffectiveArea::GetElectronEffectiveArea(ElectronEffectiveArea::kEleNeutralHadronIsoDR0p4To0p5, fMVAVar_eta, EATarget))/ele.pt(), 2.5), 0.0);
  
   if (printDebug) {
-    cout << "UseBinnedVersion=" << fUseBinnedVersion << " -> BIN: " << fMVAVar_eta << " " << fMVAVar_pt << " : " << GetMVABin(fMVAVar_eta,fMVAVar_pt) << endl;
+   std::cout << "UseBinnedVersion=" << fUseBinnedVersion << " -> BIN: " << fMVAVar_eta << " " << fMVAVar_pt << " : " << GetMVABin(fMVAVar_eta,fMVAVar_pt) <<std::endl;
   }
 
   // evaluate
@@ -1651,27 +1649,27 @@ Double_t EGammaMvaEleEstimator::isoMvaValue(const reco::GsfElectron& ele,
 
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout  << "ChargedIso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout  << "ChargedIso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_ChargedIso_DR0p0To0p1   << " "
           << fMVAVar_ChargedIso_DR0p1To0p2   << " "
           << fMVAVar_ChargedIso_DR0p2To0p3 << " "
           << fMVAVar_ChargedIso_DR0p3To0p4 << " "
-          << fMVAVar_ChargedIso_DR0p4To0p5 << endl;
-    cout  << "PF Gamma Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+          << fMVAVar_ChargedIso_DR0p4To0p5 <<std::endl;
+   std::cout  << "PF Gamma Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_GammaIso_DR0p0To0p1 << " "
           << fMVAVar_GammaIso_DR0p1To0p2 << " "
           << fMVAVar_GammaIso_DR0p2To0p3 << " "
           << fMVAVar_GammaIso_DR0p3To0p4 << " "
-          << fMVAVar_GammaIso_DR0p4To0p5 << endl;
-    cout  << "PF Neutral Hadron Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+          << fMVAVar_GammaIso_DR0p4To0p5 <<std::endl;
+   std::cout  << "PF Neutral Hadron Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_NeutralHadronIso_DR0p0To0p1 << " "
           << fMVAVar_NeutralHadronIso_DR0p1To0p2 << " "
           << fMVAVar_NeutralHadronIso_DR0p2To0p3 << " "
           << fMVAVar_NeutralHadronIso_DR0p3To0p4 << " "
           << fMVAVar_NeutralHadronIso_DR0p4To0p5 << " "
-          << endl;
-    cout << " ### MVA " << mva << endl;
+          <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
   
 
@@ -1725,7 +1723,7 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(const reco::GsfElectron& e
   fMVAVar_etawidth        =  ele.superCluster()->etaWidth();
   fMVAVar_phiwidth        =  ele.superCluster()->phiWidth();
   fMVAVar_OneMinusE1x5E5x5        =  (ele.e5x5()) !=0. ? 1.-(ele.e1x5()/ele.e5x5()) : -1. ;
-  fMVAVar_OneMinusE1x5E5x5 = max(min(double(fMVAVar_OneMinusE1x5E5x5),2.0),-1.0);
+  fMVAVar_OneMinusE1x5E5x5 = std::max(std::min(double(fMVAVar_OneMinusE1x5E5x5),2.0),-1.0);
   fMVAVar_R9              =  myEcalCluster.e3x3(*(ele.superCluster()->seed())) / ele.superCluster()->rawEnergy();
   if (fMVAVar_R9 > 5) fMVAVar_R9 = 5;
 
@@ -1874,7 +1872,7 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(const reco::GsfElectron& e
     fMVAVar_NeutralHadronIso_DR0p4To0p5 = TMath::Max(TMath::Min((tmpNeutralHadronIso_DR0p4To0p5)/ele.pt(), 2.5), 0.0);
     fMVAVar_rho = Rho;
   } else {
-    cout << "Warning: Type " << fMVAType << " is not supported.\n";
+   std::cout << "Warning: Type " << fMVAType << " is not supported.\n";
   }
 
   // evaluate
@@ -1889,8 +1887,8 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(const reco::GsfElectron& e
 
 
   if(printDebug) {
-    cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << endl;
-    cout << " fbrem " <<  fMVAVar_fbrem  
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -1910,28 +1908,28 @@ Double_t EGammaMvaEleEstimator::IDIsoCombinedMvaValue(const reco::GsfElectron& e
 	 << " d0 " << fMVAVar_d0  
 	 << " ip3d " << fMVAVar_ip3d  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << endl;
-    cout  << "ChargedIso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+	 << " pt " << fMVAVar_pt <<std::endl;
+   std::cout  << "ChargedIso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_ChargedIso_DR0p0To0p1   << " "
           << fMVAVar_ChargedIso_DR0p1To0p2   << " "
           << fMVAVar_ChargedIso_DR0p2To0p3 << " "
           << fMVAVar_ChargedIso_DR0p3To0p4 << " "
-          << fMVAVar_ChargedIso_DR0p4To0p5 << endl;
-    cout  << "PF Gamma Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+          << fMVAVar_ChargedIso_DR0p4To0p5 <<std::endl;
+   std::cout  << "PF Gamma Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_GammaIso_DR0p0To0p1 << " "
           << fMVAVar_GammaIso_DR0p1To0p2 << " "
           << fMVAVar_GammaIso_DR0p2To0p3 << " "
           << fMVAVar_GammaIso_DR0p3To0p4 << " "
-          << fMVAVar_GammaIso_DR0p4To0p5 << endl;
-    cout  << "PF Neutral Hadron Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
+          << fMVAVar_GammaIso_DR0p4To0p5 <<std::endl;
+   std::cout  << "PF Neutral Hadron Iso ( 0.0 | 0.1 | 0.2 | 0.3 | 0.4 | 0.5 ): " 
           << fMVAVar_NeutralHadronIso_DR0p0To0p1 << " "
           << fMVAVar_NeutralHadronIso_DR0p1To0p2 << " "
           << fMVAVar_NeutralHadronIso_DR0p2To0p3 << " "
           << fMVAVar_NeutralHadronIso_DR0p3To0p4 << " "
           << fMVAVar_NeutralHadronIso_DR0p4To0p5 << " "
-          << endl;
-    cout  << "Rho : " << Rho << endl;
-    cout << " ### MVA " << mva << endl;
+          <<std::endl;
+   std::cout  << "Rho : " << Rho <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
 
diff --git a/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimatorCSA14.cc b/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimatorCSA14.cc
index 6dda654..6b1dc69 100644
--- a/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimatorCSA14.cc
+++ b/EgammaAnalysis/ElectronTools/src/EGammaMvaEleEstimatorCSA14.cc
@@ -259,7 +259,7 @@ Double_t EGammaMvaEleEstimatorCSA14::mvaValue(const reco::GsfElectron& ele,
   }
 
   if ( (fMVAType != EGammaMvaEleEstimatorCSA14::kTrig) && (fMVAType != EGammaMvaEleEstimatorCSA14::kNonTrig) && (fMVAType != EGammaMvaEleEstimatorCSA14::kNonTrigPhys14) ) {
-    std::cout << "Error: This method should be called for kTrig or kNonTrig or kNonTrigPhys14 MVA only" << std::endl;
+    std::cout << "Error: This method should be called for kTrig or kNonTrig or kNonTrigPhys14 MVA only" <<std::endl;
     return -9999;
   }
  
@@ -352,8 +352,8 @@ Double_t EGammaMvaEleEstimatorCSA14::mvaValue(const reco::GsfElectron& ele,
 
 
   if(printDebug) {
-    std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << std::endl;
-    std::cout << " fbrem " <<  fMVAVar_fbrem  
+   std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+   std::cout << " fbrem " <<  fMVAVar_fbrem  
       	 << " kfchi2 " << fMVAVar_kfchi2  
 	 << " mykfhits " << fMVAVar_kfhits  
 	 << " gsfchi2 " << fMVAVar_gsfchi2  
@@ -373,8 +373,8 @@ Double_t EGammaMvaEleEstimatorCSA14::mvaValue(const reco::GsfElectron& ele,
 	 << " d0 " << fMVAVar_d0  
 	 << " ip3d " << fMVAVar_ip3d  
 	 << " eta " << fMVAVar_eta  
-	 << " pt " << fMVAVar_pt << std::endl;
-    std::cout << " ### MVA " << mva << std::endl;
+	 << " pt " << fMVAVar_pt <<std::endl;
+   std::cout << " ### MVA " << mva <<std::endl;
   }
 
 
@@ -393,7 +393,7 @@ Double_t EGammaMvaEleEstimatorCSA14::mvaValue(const pat::Electron& ele,
     }
     
     if ( (fMVAType != EGammaMvaEleEstimatorCSA14::kTrig) && (fMVAType != EGammaMvaEleEstimatorCSA14::kNonTrig) && (fMVAType != EGammaMvaEleEstimatorCSA14::kNonTrigPhys14) ) {
-        std::cout << "Error: This method should be called for kTrig or kNonTrig or kNonTrigPhys14 MVA only" << std::endl;
+        std::cout << "Error: This method should be called for kTrig or kNonTrig or kNonTrigPhys14 MVA only" <<std::endl;
         return -9999;
     }
     
@@ -456,8 +456,8 @@ Double_t EGammaMvaEleEstimatorCSA14::mvaValue(const pat::Electron& ele,
     
     
     if(printDebug) {
-        std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType << std::endl;
-        std::cout << " fbrem " <<  fMVAVar_fbrem
+       std::cout << " *** Inside the class fMethodname " << fMethodname << " fMVAType " << fMVAType <<std::endl;
+       std::cout << " fbrem " <<  fMVAVar_fbrem
         << " kfchi2 " << fMVAVar_kfchi2
         << " mykfhits " << fMVAVar_kfhits
         << " gsfchi2 " << fMVAVar_gsfchi2
@@ -475,8 +475,8 @@ Double_t EGammaMvaEleEstimatorCSA14::mvaValue(const pat::Electron& ele,
         << " IoEmIoP " << fMVAVar_IoEmIoP  
         << " eleEoPout " << fMVAVar_eleEoPout  
         << " eta " << fMVAVar_eta
-        << " pt " << fMVAVar_pt << std::endl;
-        std::cout << " ### MVA " << mva << std::endl;
+        << " pt " << fMVAVar_pt <<std::endl;
+       std::cout << " ### MVA " << mva <<std::endl;
     }
     
     
diff --git a/EgammaAnalysis/ElectronTools/src/ElectronEnergyCalibrator.cc b/EgammaAnalysis/ElectronTools/src/ElectronEnergyCalibrator.cc
index c6e8de6..10d1ddd 100644
--- a/EgammaAnalysis/ElectronTools/src/ElectronEnergyCalibrator.cc
+++ b/EgammaAnalysis/ElectronTools/src/ElectronEnergyCalibrator.cc
@@ -19,12 +19,6 @@
 #include <sstream>
 #include <iostream>
 
-using std::string;
-using std::vector;
-using std::ifstream;
-using std::istringstream;
-using std::cout;
-using namespace edm;
 
 void ElectronEnergyCalibrator::init()
 {
@@ -35,7 +29,7 @@ void ElectronEnergyCalibrator::init()
             std::cout << "[ElectronEnergyCalibrator] Initialization in DATA mode" << std::endl;
         }
 
-    	ifstream fin(pathData_.c_str());
+	    std::ifstream fin(pathData_.c_str());
     
     	if (!fin){
     		     throw cms::Exception("Configuration")
@@ -49,9 +43,9 @@ void ElectronEnergyCalibrator::init()
                     << pathData_ << " succesfully opened" << std::endl;
             }
     
-        	string s;
-        	vector<string> selements;
-        	string delimiter = ",";
+	    std::string s;
+	    std::vector<std::string> selements;
+	    std::string delimiter = ",";
         	nCorrValRaw = 0;	
     	
         	while ( !fin.eof() ) 
@@ -88,7 +82,7 @@ void ElectronEnergyCalibrator::init()
         // linearity corrections data
         if(applyLinearityCorrection_) 
         {
-            ifstream finlin(pathLinData_.c_str());
+	  std::ifstream finlin(pathLinData_.c_str());
 
             if (!finlin)
             {
@@ -101,9 +95,9 @@ void ElectronEnergyCalibrator::init()
                     std::cout<<"[ElectronEnergyCalibrator] File with Linearity Corrections "<<pathLinData_<<" succesfully opened"<<std::endl;
                 }
 
-                string s;
-                vector<string> selements;
-                string delimiter = ",";
+                std::string s;
+                std::vector<std::string> selements;
+                std::string delimiter = ",";
                 nLinCorrValRaw = 0;	
 
                 while ( !finlin.eof() ) 
@@ -145,12 +139,12 @@ void ElectronEnergyCalibrator::init()
     }
 }
 
-void ElectronEnergyCalibrator::splitString(const string &fullstr, vector<string> &elements, const string &delimiter)
+void ElectronEnergyCalibrator::splitString(const std::string &fullstr, std::vector<std::string> &elements, const std::string &delimiter)
 {
-	string::size_type lastpos = fullstr.find_first_not_of(delimiter, 0);
-	string::size_type pos     = fullstr.find_first_of(delimiter, lastpos);
+	std::string::size_type lastpos = fullstr.find_first_not_of(delimiter, 0);
+	std::string::size_type pos     = fullstr.find_first_of(delimiter, lastpos);
 	
-	while ( ( string::npos != pos ) || ( string::npos != lastpos ) ) 
+	while ( ( std::string::npos != pos ) || ( std::string::npos != lastpos ) ) 
     {
 	    elements.push_back(fullstr.substr(lastpos, pos-lastpos));
 	    lastpos = fullstr.find_first_not_of(delimiter, pos);
@@ -158,9 +152,9 @@ void ElectronEnergyCalibrator::splitString(const string &fullstr, vector<string>
 	}
 }
 
-double ElectronEnergyCalibrator::stringToDouble(const string &str)
+double ElectronEnergyCalibrator::stringToDouble(const std::string &str)
 {
-	istringstream stm;
+  std::istringstream stm;
 	double val = 0;
 	stm.str(str);
 	stm >> val;
diff --git a/EgammaAnalysis/ElectronTools/src/EpCombinationTool.cc b/EgammaAnalysis/ElectronTools/src/EpCombinationTool.cc
index 995c218..dc212dc 100644
--- a/EgammaAnalysis/ElectronTools/src/EpCombinationTool.cc
+++ b/EgammaAnalysis/ElectronTools/src/EpCombinationTool.cc
@@ -4,14 +4,10 @@
 #include <TSystem.h>
 #include <math.h>
 #include <vector>
-#include <iostream>
-
-using namespace std;
-
 
 /*****************************************************************/
 EpCombinationTool::EpCombinationTool():
-    m_forest(NULL)
+    m_forest(NULL), m_ownForest(false)
 /*****************************************************************/
 {
 }
@@ -22,25 +18,27 @@ EpCombinationTool::EpCombinationTool():
 EpCombinationTool::~EpCombinationTool()
 /*****************************************************************/
 {
-    if(m_forest) delete m_forest;
+    if(m_ownForest) delete m_forest;
 }
 
 
 /*****************************************************************/
-bool EpCombinationTool::init(const string& regressionFileName, const string& bdtName)
+bool EpCombinationTool::init(const std::string& regressionFileName, const std::string& bdtName)
 /*****************************************************************/
 {
     TFile* regressionFile = TFile::Open(regressionFileName.c_str());
     if(!regressionFile)
     {
-        cout<<"ERROR: Cannot open regression file "<<regressionFileName<<"\n";
+      std::cout<<"ERROR: Cannot open regression file "<<regressionFileName<<"\n";
         return false;
     }
+    if(m_ownForest) delete m_forest;
     m_forest = (GBRForest*) regressionFile->Get(bdtName.c_str());
+    m_ownForest = true;
     //regressionFile->GetObject(bdtName.c_str(), m_forest); 
     if(!m_forest)
     {
-        cout<<"ERROR: Cannot find forest "<<bdtName<<" in "<<regressionFileName<<"\n";
+      std::cout<<"ERROR: Cannot find forest "<<bdtName<<" in "<<regressionFileName<<"\n";
         regressionFile->Close();
         return false;
     }
@@ -48,15 +46,22 @@ bool EpCombinationTool::init(const string& regressionFileName, const string& bdt
     return true;
 }
 
+bool EpCombinationTool::init(const GBRForest *forest) 
+{
+    if(m_ownForest) delete m_forest;
+    m_forest = forest;
+    m_ownForest = false;
+    return true;
+}
 
 
 /*****************************************************************/
-void EpCombinationTool::combine(SimpleElectron & mySimpleElectron)
+void EpCombinationTool::combine(SimpleElectron & mySimpleElectron) const
 /*****************************************************************/
 {
     if(!m_forest)
     {
-        cout<<"ERROR: The combination tool is not initialized\n";
+      std::cout<<"ERROR: The combination tool is not initialized\n";
         return;
     }
 
@@ -82,7 +87,7 @@ void EpCombinationTool::combine(SimpleElectron & mySimpleElectron)
             (energy*momentumError/momentum/momentum));
 
     // fill input variables
-    float* regressionInputs = new float[11];
+    float regressionInputs[11];
     regressionInputs[0]  = energy;
     regressionInputs[1]  = energyRelError;
     regressionInputs[2]  = momentum;
@@ -116,6 +121,4 @@ void EpCombinationTool::combine(SimpleElectron & mySimpleElectron)
         mySimpleElectron.setCombinedMomentum(combinedMomentum);
         mySimpleElectron.setCombinedMomentumError(combinedMomentumError);
     }
-
-    delete[] regressionInputs;
 }
diff --git a/EgammaAnalysis/ElectronTools/src/PFIsolationEstimator.cc b/EgammaAnalysis/ElectronTools/src/PFIsolationEstimator.cc
index 011cd75..05c3773 100644
--- a/EgammaAnalysis/ElectronTools/src/PFIsolationEstimator.cc
+++ b/EgammaAnalysis/ElectronTools/src/PFIsolationEstimator.cc
@@ -2,7 +2,6 @@
 #include "EgammaAnalysis/ElectronTools/interface/PFIsolationEstimator.h"
 #include <cmath>
 #include "DataFormats/Math/interface/deltaR.h"
-using namespace std;
 
 #ifndef STANDALONE
 #include "DataFormats/TrackReco/interface/Track.h"
@@ -21,10 +20,6 @@ using namespace std;
 
 #endif
 
-using namespace reco;
-
-
-
 //--------------------------------------------------------------------------------------------------
 PFIsolationEstimator::PFIsolationEstimator() :
 fisInitialized(kFALSE)
@@ -205,7 +200,7 @@ void PFIsolationEstimator::initializeRings(int iNumberOfRings, float fRingSize){
 float PFIsolationEstimator::fGetIsolation(const reco::PFCandidate * pfCandidate, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
  
   fGetIsolationInRings( pfCandidate, pfParticlesColl, vtx, vertices);
-  refSC = SuperClusterRef();
+  refSC = reco::SuperClusterRef();
   fIsolation = fIsolationInRings[0];
   
   return fIsolation;
@@ -213,7 +208,7 @@ float PFIsolationEstimator::fGetIsolation(const reco::PFCandidate * pfCandidate,
 
 
 //--------------------------------------------------------------------------------------------------
-vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::PFCandidate * pfCandidate, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
+std::vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::PFCandidate * pfCandidate, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
 
   int isoBin;
 
@@ -289,7 +284,7 @@ float PFIsolationEstimator::fGetIsolation(const reco::Photon * photon, const rec
 
 
 //--------------------------------------------------------------------------------------------------
-vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::Photon * photon, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
+std::vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::Photon * photon, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
 
   int isoBin;
   
@@ -394,7 +389,7 @@ float PFIsolationEstimator::fGetIsolation(const reco::GsfElectron * electron, co
 
 
 //--------------------------------------------------------------------------------------------------
-vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::GsfElectron * electron, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
+std::vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::GsfElectron * electron, const reco::PFCandidateCollection* pfParticlesColl,reco::VertexRef vtx, edm::Handle< reco::VertexCollection > vertices) {
 
   int isoBin;
   
@@ -415,7 +410,7 @@ vector<float >  PFIsolationEstimator::fGetIsolationInRings(const reco::GsfElectr
   fVx =  electron->vx();
   fVy =  electron->vy();
   fVz =  electron->vz();
-  iMissHits = electron->gsfTrack()->hitPattern().numberOfHits(HitPattern::MISSING_INNER_HITS);
+  iMissHits = electron->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS);
   
   //  if(electron->ecalDrivenSeed())
   refSC = electron->superCluster();
@@ -579,7 +574,7 @@ float  PFIsolationEstimator::isChargedParticleVetoed(const reco::PFCandidate* pf
 //-----------------------------------------------------------------------------------------------------
 float  PFIsolationEstimator::isChargedParticleVetoed(const reco::PFCandidate* pfIsoCand,reco::VertexRef vtxMain, edm::Handle< reco::VertexCollection >  vertices  ){
   
-  VertexRef vtx = chargedHadronVertex(vertices,  *pfIsoCand );
+  reco::VertexRef vtx = chargedHadronVertex(vertices,  *pfIsoCand );
   if(vtx.isNull())
     return -999.;
   
@@ -685,7 +680,7 @@ float  PFIsolationEstimator::isChargedParticleVetoed(const reco::PFCandidate* pf
 
 
 //--------------------------------------------------------------------------------------------------
- VertexRef  PFIsolationEstimator::chargedHadronVertex(  edm::Handle< reco::VertexCollection > verticesColl, const reco::PFCandidate& pfcand ){
+reco::VertexRef  PFIsolationEstimator::chargedHadronVertex(  edm::Handle< reco::VertexCollection > verticesColl, const reco::PFCandidate& pfcand ){
 
   //code copied from Florian's PFNoPU class
     
@@ -728,7 +723,7 @@ float  PFIsolationEstimator::isChargedParticleVetoed(const reco::PFCandidate* pf
   if (nFoundVertex>0){
     if (nFoundVertex!=1)
       edm::LogWarning("TrackOnTwoVertex")<<"a track is shared by at least two verteces. Used to be an assert";
-    return  VertexRef( verticesColl, iVertex);
+    return  reco::VertexRef( verticesColl, iVertex);
   }
   // no vertex found with this track. 
 
@@ -750,11 +745,11 @@ float  PFIsolationEstimator::isChargedParticleVetoed(const reco::PFCandidate* pf
     }
 
     if( foundVertex ) 
-      return  VertexRef( verticesColl, iVertex);  
+      return  reco::VertexRef( verticesColl, iVertex);  
   
   }
    
-  return  VertexRef( );
+  return  reco::VertexRef( );
 }
 
 
diff --git a/EgammaAnalysis/ElectronTools/test/ElectronIDValidationAnalyzer.cc b/EgammaAnalysis/ElectronTools/test/ElectronIDValidationAnalyzer.cc
index f4c467b..c254b71 100644
--- a/EgammaAnalysis/ElectronTools/test/ElectronIDValidationAnalyzer.cc
+++ b/EgammaAnalysis/ElectronTools/test/ElectronIDValidationAnalyzer.cc
@@ -167,8 +167,6 @@ ElectronIDValidationAnalyzer::~ElectronIDValidationAnalyzer()
 void
 ElectronIDValidationAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
 {
-   // using namespace edm;
-
    edm::Handle<edm::ValueMap<float> > full5x5sieie;
    edm::Handle<edm::View<reco::GsfElectron> > collection;
    edm::Handle<edm::ValueMap<bool> > id_decisions;
diff --git a/EgammaAnalysis/ElectronTools/test/ElectronIsoAnalyzer.cc b/EgammaAnalysis/ElectronTools/test/ElectronIsoAnalyzer.cc
index 685c789..bd5d2f7 100644
--- a/EgammaAnalysis/ElectronTools/test/ElectronIsoAnalyzer.cc
+++ b/EgammaAnalysis/ElectronTools/test/ElectronIsoAnalyzer.cc
@@ -44,9 +44,7 @@
 // class decleration
 //
 
-using namespace edm;
-using namespace reco;
-using namespace std;
+
 class ElectronIsoAnalyzer : public edm::EDAnalyzer {
    public:
       explicit ElectronIsoAnalyzer(const edm::ParameterSet&);
@@ -59,7 +57,7 @@ class ElectronIsoAnalyzer : public edm::EDAnalyzer {
       virtual void endJob() ;
 
 
-  ParameterSet conf_;
+  edm::ParameterSet conf_;
 
 
   unsigned int ev;
@@ -175,9 +173,9 @@ ElectronIsoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iS
 
 
 
-  Handle<GsfElectronCollection> theEGammaCollection;
+  edm::Handle<reco::GsfElectronCollection> theEGammaCollection;
   iEvent.getByToken(tokenGsfElectrons_,theEGammaCollection);
-  const GsfElectronCollection theEGamma = *(theEGammaCollection.product());
+  const reco::GsfElectronCollection theEGamma = *(theEGammaCollection.product());
 
   // rho for isolation
   edm::Handle<double> rhoIso_h;
@@ -207,9 +205,9 @@ ElectronIsoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iS
 
     float eff_area_phnh = ElectronEffectiveArea::GetElectronEffectiveArea(effAreaGammaPlusNeutralHad_, abseta, effAreaTarget_);
 
-    double myRho = max<double>(0.,rhoIso);
+    double myRho = std::max<double>(0.,rhoIso);
 
-    float myPfIsoPuCorr = charged + max<float>(0.f, (photon+neutral) - eff_area_phnh*myRho);
+    float myPfIsoPuCorr = charged + std::max<float>(0.f, (photon+neutral) - eff_area_phnh*myRho);
 
 
     if(verbose_) {
@@ -230,7 +228,7 @@ ElectronIsoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iS
       neutralBarrel_->Fill(neutral/myElectronRef->pt());
       sumBarrel_->Fill((charged+photon+neutral)/myElectronRef->pt());
       sumCorrBarrel_->Fill(myPfIsoPuCorr/myElectronRef->pt());
-      missHitsBarrel_->Fill(myElectronRef->gsfTrack()->hitPattern().numberOfHits(HitPattern::MISSING_INNER_HITS));
+      missHitsBarrel_->Fill(myElectronRef->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
 
     } else {
       chargedEndcaps_ ->Fill(charged/myElectronRef->pt());
@@ -238,7 +236,7 @@ ElectronIsoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iS
       neutralEndcaps_->Fill(neutral/myElectronRef->pt());
       sumEndcaps_->Fill((charged+photon+neutral)/myElectronRef->pt());
       sumCorrEndcaps_->Fill(myPfIsoPuCorr/myElectronRef->pt());
-      missHitsEndcap_->Fill(myElectronRef->gsfTrack()->hitPattern().numberOfHits(HitPattern::MISSING_INNER_HITS));
+      missHitsEndcap_->Fill(myElectronRef->gsfTrack()->hitPattern().numberOfHits(reco::HitPattern::MISSING_INNER_HITS));
     }
   }
 
@@ -254,7 +252,7 @@ ElectronIsoAnalyzer::beginJob(const edm::EventSetup&)
 // ------------ method called once each job just after ending the event loop  ------------
 void
 ElectronIsoAnalyzer::endJob() {
-  cout << " endJob:: #events " << ev << endl;
+  std::cout << " endJob:: #events " << ev << std::endl;
 }
 
 //define this as a plug-in
diff --git a/EgammaAnalysis/ElectronTools/test/ElectronTestAnalyzer.cc b/EgammaAnalysis/ElectronTools/test/ElectronTestAnalyzer.cc
index d9fd9f0..44c5701 100644
--- a/EgammaAnalysis/ElectronTools/test/ElectronTestAnalyzer.cc
+++ b/EgammaAnalysis/ElectronTools/test/ElectronTestAnalyzer.cc
@@ -63,9 +63,7 @@
 // class decleration
 //
 
-using namespace edm;
-using namespace reco;
-using namespace std;
+
 class ElectronTestAnalyzer : public edm::EDAnalyzer {
 public:
   explicit ElectronTestAnalyzer(const edm::ParameterSet&);
@@ -86,10 +84,10 @@ private:
   
   bool trainTrigPresel(const reco::GsfElectron& ele);
   
-  ParameterSet conf_;
+  edm::ParameterSet conf_;
   
-  edm::EDGetTokenT<GsfElectronCollection> gsfEleToken_;
-  edm::EDGetTokenT<GenParticleCollection> genToken_;
+  edm::EDGetTokenT<reco::GsfElectronCollection> gsfEleToken_;
+  edm::EDGetTokenT<reco::GenParticleCollection> genToken_;
   //edm::EDGetTokenT<edm::HepMCProduct>  mcTruthToken_;
   edm::EDGetTokenT<reco::VertexCollection> vertexToken_;
   //edm::EDGetTokenT<reco::PFCandidateCollection> pfCandToken_;
@@ -177,9 +175,9 @@ private:
 //
 ElectronTestAnalyzer::ElectronTestAnalyzer(const edm::ParameterSet& iConfig):
   conf_(iConfig),
-  gsfEleToken_(consumes<GsfElectronCollection>(edm::InputTag("gsfElectrons"))),
-  genToken_(consumes<GenParticleCollection>(edm::InputTag("genParticles"))),
-  //mcTruthToken_(consumes<edm::HepMCProduct>(edm::InputTag("VtxSmeared"))),
+  gsfEleToken_(consumes<reco::GsfElectronCollection>(edm::InputTag("gsfElectrons"))),
+  genToken_(consumes<reco::GenParticleCollection>(edm::InputTag("genParticles"))),
+  //mcTruthToken_(consumes<edm::HepMCProduct>(edm::InputTag("generator"))),
   vertexToken_(consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices"))),
   //pfCandToken_(consumes<reco::PFCandidateCollection>(edm::InputTag("particleFlow"))),
   eventrhoToken_(consumes<double>(edm::InputTag("kt6PFJets", "rho"))),
@@ -290,18 +288,18 @@ ElectronTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& i
 
 	ElectronTestAnalyzer::evaluate_mvas(iEvent, iSetup);
 
-  Handle<GsfElectronCollection> theEGammaCollection;
+	edm::Handle<reco::GsfElectronCollection> theEGammaCollection;
   iEvent.getByToken(gsfEleToken_,theEGammaCollection);
-  const GsfElectronCollection theEGamma = *(theEGammaCollection.product());
+  const reco::GsfElectronCollection theEGamma = *(theEGammaCollection.product());
 
-  Handle<GenParticleCollection> genParticles;
+  edm::Handle<reco::GenParticleCollection> genParticles;
   iEvent.getByToken(genToken_,genParticles);
-  //InputTag  mcTruthToken_(string("VtxSmeared"));
+  //InputTag  mcTruthToken_(string("generator"));
   //edm::Handle<edm::HepMCProduct> pMCTruth;
   //iEvent.getByToken(mcTruthToken_,pMCTruth);
   //const HepMC::GenEvent* genEvent = pMCTruth->GetEvent();
 
-  Handle<reco::VertexCollection> thePrimaryVertexColl;
+  edm::Handle<reco::VertexCollection> thePrimaryVertexColl;
   iEvent.getByToken(vertexToken_,thePrimaryVertexColl);
 
   _Rho=0;
@@ -311,17 +309,17 @@ ElectronTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& i
 
 
 
-  Vertex dummy;
-  const Vertex *pv = &dummy;
+  reco::Vertex dummy;
+  const reco::Vertex *pv = &dummy;
   if (thePrimaryVertexColl->size() != 0) {
     pv = &*thePrimaryVertexColl->begin();
   } else { // create a dummy PV
-    Vertex::Error e;
+    reco::Vertex::Error e;
     e(0, 0) = 0.0015 * 0.0015;
     e(1, 1) = 0.0015 * 0.0015;
     e(2, 2) = 15. * 15.;
-    Vertex::Point p(0, 0, 0);
-    dummy = Vertex(p, e, 0, 0, 0);
+    reco::Vertex::Point p(0, 0, 0);
+    dummy = reco::Vertex(p, e, 0, 0, 0);
   }
   EcalClusterLazyTools lazyTools(iEvent, iSetup, reducedEBRecHitCollectionToken_, reducedEERecHitCollectionToken_);
 
@@ -342,7 +340,7 @@ ElectronTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& i
 
   for(size_t i(0); i < genParticles->size(); i++)
   {
-    const GenParticle &genPtcl = (*genParticles)[i];
+    const reco::GenParticle &genPtcl = (*genParticles)[i];
     float etamc= genPtcl.eta();
     float phimc= genPtcl.phi();
     float ptmc = genPtcl.pt();
@@ -399,7 +397,7 @@ ElectronTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& i
 
 
 	  if(debug)
-	    cout << "************************* New Good Event:: " << ev << " *************************" << endl;
+	    std::cout << "************************* New Good Event:: " << ev << " *************************" << std::endl;
 
 	  // ********************* Triggering electrons
 
@@ -441,7 +439,7 @@ ElectronTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& i
 	  h_mva_nonTrig->Fill(mvaNonTrigMthd1);
 
 	  if(debug)
-	    cout << "Non-Triggering:: MyMVA Method-1 " << mvaNonTrigMthd1 << " MyMVA Method-2 " << mvaNonTrigMthd2 <<endl;
+	    std::cout << "Non-Triggering:: MyMVA Method-1 " << mvaNonTrigMthd1 << " MyMVA Method-2 " << mvaNonTrigMthd2 <<std::endl;
 
 	  if(elePresel) {
 	    mvaTrigNonIp = myMVATrigNoIPV0->mvaValue( (theEGamma[j]), *pv, _Rho,/*thebuilder,*/lazyTools, debugMVAclass);
@@ -478,9 +476,9 @@ ElectronTestAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& i
 	  }
 
 	  if(debug)
-	    cout << "Triggering:: ElePreselection " << elePresel
+	    std::cout << "Triggering:: ElePreselection " << elePresel
 		 << " MyMVA Method-1 " << mvaTrigMthd1
-	  << " MyMVA Method-2 " << mvaTrigMthd2 << endl;
+		      << " MyMVA Method-2 " << mvaTrigMthd2 << std::endl;
 	}
       } // End Loop on RECO electrons
     } // End if MC electrons selection
@@ -563,8 +561,8 @@ void ElectronTestAnalyzer::myVar(const reco::GsfElectron& ele,
 
 
   if(printDebug) {
-    cout << " My Local Variables " << endl;
-    cout << " fbrem " <<  myMVAVar_fbrem
+    std::cout << " My Local Variables " << std::endl;
+   std::cout << " fbrem " <<  myMVAVar_fbrem
       	 << " kfchi2 " << myMVAVar_kfchi2
 	 << " mykfhits " << myMVAVar_kfhits
 	 << " gsfchi2 " << myMVAVar_gsfchi2
@@ -588,7 +586,7 @@ void ElectronTestAnalyzer::myVar(const reco::GsfElectron& ele,
 	 << " d0 " << myMVAVar_d0
 	 << " ip3d " << myMVAVar_ip3d
 	 << " eta " << myMVAVar_eta
-	 << " pt " << myMVAVar_pt << endl;
+	     << " pt " << myMVAVar_pt << std::endl;
   }
   return;
 }
@@ -665,7 +663,7 @@ bool ElectronTestAnalyzer::trainTrigPresel(const reco::GsfElectron& ele) {
        ele.dr03TkSumPt()/ele.pt() < 0.2 &&
        ele.dr03EcalRecHitSumEt()/ele.pt() < 0.2 &&
        ele.dr03HcalTowerSumEt()/ele.pt() < 0.2 &&
-       ele.gsfTrack()->hitPattern().numberOfLostHits(HitPattern::MISSING_INNER_HITS) == 0)
+       ele.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS) == 0)
       myTrigPresel = true;
   }
   else {
@@ -674,7 +672,7 @@ bool ElectronTestAnalyzer::trainTrigPresel(const reco::GsfElectron& ele) {
        ele.dr03TkSumPt()/ele.pt() < 0.2 &&
        ele.dr03EcalRecHitSumEt()/ele.pt() < 0.2 &&
        ele.dr03HcalTowerSumEt()/ele.pt() < 0.2 &&
-       ele.gsfTrack()->hitPattern().numberOfLostHits(HitPattern::MISSING_INNER_HITS) == 0)
+       ele.gsfTrack()->hitPattern().numberOfLostHits(reco::HitPattern::MISSING_INNER_HITS) == 0)
       myTrigPresel = true;
   }
 
@@ -691,7 +689,7 @@ ElectronTestAnalyzer::beginJob(const edm::EventSetup&)
 // ------------ method called once each job just after ending the event loop  ------------
 void
 ElectronTestAnalyzer::endJob() {
-  cout << " endJob:: #events " << ev << endl;
+ std::cout << " endJob:: #events " << ev <<std::endl;
 }
 
 void
@@ -712,11 +710,11 @@ ElectronTestAnalyzer::evaluate_mvas(const edm::Event& iEvent, const edm::EventSe
 // 	iEvent.getByToken(pfCandToken_, hPfCandProduct);
 //   const reco::PFCandidateCollection &inPfCands = *(hPfCandProduct.product());
 
-  Handle<GsfElectronCollection> theEGammaCollection;
+  edm::Handle<reco::GsfElectronCollection> theEGammaCollection;
   iEvent.getByToken(gsfEleToken_,theEGammaCollection);
-  const GsfElectronCollection inElectrons = *(theEGammaCollection.product());
+  const reco::GsfElectronCollection inElectrons = *(theEGammaCollection.product());
 
-  Handle<reco::MuonCollection> hMuonProduct;
+  edm::Handle<reco::MuonCollection> hMuonProduct;
   iEvent.getByToken(muonToken_, hMuonProduct);
   const reco::MuonCollection inMuons = *(hMuonProduct.product());
 
@@ -784,7 +782,7 @@ ElectronTestAnalyzer::evaluate_mvas(const edm::Event& iEvent, const edm::EventSe
  for (reco::GsfElectronCollection::const_iterator iE = inElectrons.begin();
        iE != inElectrons.end(); ++iE) {
 
-		GsfElectron ele = *iE;
+   reco::GsfElectron ele = *iE;
 
  		double idmva = myMVATrigV0->mvaValue(ele,
 					pvCol->at(0),
@@ -792,7 +790,7 @@ ElectronTestAnalyzer::evaluate_mvas(const edm::Event& iEvent, const edm::EventSe
 					lazyTools);
 
 
-		cout << "idmva = " << idmva << endl;
+		std::cout << "idmva = " << idmva <<std::endl;
 
 	}
 
diff --git a/EgammaAnalysis/ElectronTools/test/MiniAODElectronIDValidationAnalyzer.cc b/EgammaAnalysis/ElectronTools/test/MiniAODElectronIDValidationAnalyzer.cc
index fab590b..95fcac9 100644
--- a/EgammaAnalysis/ElectronTools/test/MiniAODElectronIDValidationAnalyzer.cc
+++ b/EgammaAnalysis/ElectronTools/test/MiniAODElectronIDValidationAnalyzer.cc
@@ -177,7 +177,6 @@ MiniAODElectronIDValidationAnalyzer::~MiniAODElectronIDValidationAnalyzer()
 void
 MiniAODElectronIDValidationAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
 {
-   // using namespace edm;
 
   //edm::Handle<edm::ValueMap<float> > full5x5sieie;
   edm::Handle<edm::View<pat::Electron> > collection;
diff --git a/RecoJets/JetProducers/interface/PileupJetIdAlgo.h b/RecoJets/JetProducers/interface/PileupJetIdAlgo.h
index 99edda1..41a4d89 100644
--- a/RecoJets/JetProducers/interface/PileupJetIdAlgo.h
+++ b/RecoJets/JetProducers/interface/PileupJetIdAlgo.h
@@ -64,8 +64,8 @@ protected:
 	PileupJetIdentifier internalId_;
 	variables_list_t variables_;
 
-	std::unique_ptr<TMVA::Reader> reader_, reader_jteta_0_2_, reader_jteta_2_2p5_, reader_jteta_2p5_3_, reader_jteta_3_5_;
-	std::string    tmvaWeights_, tmvaWeights_jteta_0_2_, tmvaWeights_jteta_2_2p5_, tmvaWeights_jteta_2p5_3_, tmvaWeights_jteta_3_5_, tmvaMethod_; 
+	std::unique_ptr<TMVA::Reader> reader_, reader_jteta_0_2p5_, reader_jteta_2p5_2p75_, reader_jteta_2p75_3_, reader_jteta_3_5_;
+	std::string    tmvaWeights_, tmvaWeights_jteta_0_2p5_, tmvaWeights_jteta_2p5_2p75_, tmvaWeights_jteta_2p75_3_, tmvaWeights_jteta_3_5_, tmvaMethod_; 
 	std::vector<std::string>  tmvaVariables_, tmvaVariables_jteta_0_3_, tmvaVariables_jteta_3_5_;
 	std::vector<std::string>  tmvaSpectators_;
 	std::map<std::string,std::string>  tmvaNames_;
diff --git a/RecoJets/JetProducers/python/PileupJetIDCutParams_cfi.py b/RecoJets/JetProducers/python/PileupJetIDCutParams_cfi.py
index 40c3b43..de02ade 100644
--- a/RecoJets/JetProducers/python/PileupJetIDCutParams_cfi.py
+++ b/RecoJets/JetProducers/python/PileupJetIDCutParams_cfi.py
@@ -1,6 +1,31 @@
 import FWCore.ParameterSet.Config as cms
 
 ###########################################################
+## Working points for the 76X training
+###########################################################
+full_76x_chs_wp  = cms.PSet(
+    #4 Eta Categories  0-2.5 2.5-2.75 2.75-3.0 3.0-5.0
+    
+    #Tight Id            
+    Pt010_Tight    = cms.vdouble(0.09,-0.37,-0.24,-0.21),
+    Pt1020_Tight   = cms.vdouble(0.09,-0.37,-0.24,-0.21),
+    Pt2030_Tight   = cms.vdouble(0.09,-0.37,-0.24,-0.21),
+    Pt3050_Tight   = cms.vdouble(0.52,-0.19,-0.06,-0.03),
+
+    #Medium Id
+    Pt010_Medium   = cms.vdouble(-0.58,-0.52,-0.40,-0.36),
+    Pt1020_Medium  = cms.vdouble(-0.58,-0.52,-0.40,-0.36),
+    Pt2030_Medium  = cms.vdouble(-0.58,-0.52,-0.40,-0.36),
+    Pt3050_Medium  = cms.vdouble(-0.20,-0.39,-0.24,-0.19),
+
+    #Loose Id
+    Pt010_Loose    = cms.vdouble(-0.96,-0.62,-0.53,-0.49),
+    Pt1020_Loose   = cms.vdouble(-0.96,-0.62,-0.53,-0.49),
+    Pt2030_Loose   = cms.vdouble(-0.96,-0.62,-0.53,-0.49),
+    Pt3050_Loose   = cms.vdouble(-0.93,-0.52,-0.39,-0.31)   
+)
+
+###########################################################
 ## Working points for the 74X training
 ###########################################################
 full_74x_chs_wp  = cms.PSet(
diff --git a/RecoJets/JetProducers/python/PileupJetIDParams_cfi.py b/RecoJets/JetProducers/python/PileupJetIDParams_cfi.py
index dda2318..4352114 100644
--- a/RecoJets/JetProducers/python/PileupJetIDParams_cfi.py
+++ b/RecoJets/JetProducers/python/PileupJetIDParams_cfi.py
@@ -1,6 +1,54 @@
 import FWCore.ParameterSet.Config as cms
 from RecoJets.JetProducers.PileupJetIDCutParams_cfi import *
-
+####################################################################################################################                                                                                      
+full_76x_chs = cms.PSet(
+ impactParTkThreshold = cms.double(1.) ,
+ cutBased = cms.bool(False),
+ etaBinnedWeights = cms.bool(True),
+ tmvaWeights_jteta_0_2p5    = cms.string("RecoJets/JetProducers/data/pileupJetId_76x_Eta0to2p5_BDT.weights.xml.gz"),
+ tmvaWeights_jteta_2p5_2p75 = cms.string("RecoJets/JetProducers/data/pileupJetId_76x_Eta2p5to2p75_BDT.weights.xml.gz"),
+ tmvaWeights_jteta_2p75_3   = cms.string("RecoJets/JetProducers/data/pileupJetId_76x_Eta2p75to3_BDT.weights.xml.gz"),
+ tmvaWeights_jteta_3_5      = cms.string("RecoJets/JetProducers/data/pileupJetId_76x_Eta3to5_BDT.weights.xml.gz"),
+ tmvaMethod  = cms.string("JetIDMVAHighPt"),
+ version = cms.int32(-1),
+ tmvaVariables_jteta_0_3 = cms.vstring(
+    "nvtx",
+    "dR2Mean"     ,
+    "nParticles"     ,
+    "nCharged" ,
+    "majW" ,
+    "minW",
+    "frac01"  ,
+    "frac02"      ,
+    "frac03"   ,
+    "frac04"   ,
+    "ptD"   ,
+    "beta"   ,
+    "pull"   ,
+    "jetR"   ,
+    "jetRchg"   ,
+    ),
+ tmvaVariables_jteta_3_5 = cms.vstring(
+    "nvtx",
+    "dR2Mean"     ,
+    "nParticles"     ,
+    "majW" ,
+    "minW",
+    "frac01"  ,
+    "frac02"      ,
+    "frac03"   ,
+    "frac04"   ,
+    "ptD"   ,
+    "pull"   ,
+    "jetR"   ,
+    ),
+ tmvaSpectators = cms.vstring(
+    "jetPt"   ,
+    "jetEta"   ,
+    ),
+ JetIdParams = full_76x_chs_wp,
+ label = cms.string("full")
+ )
 ####################################################################################################################                                                                                      
 full_74x_chs = cms.PSet(
  impactParTkThreshold = cms.double(1.) ,
diff --git a/RecoJets/JetProducers/python/PileupJetID_cfi.py b/RecoJets/JetProducers/python/PileupJetID_cfi.py
index 6cf7650..178307e 100644
--- a/RecoJets/JetProducers/python/PileupJetID_cfi.py
+++ b/RecoJets/JetProducers/python/PileupJetID_cfi.py
@@ -7,9 +7,10 @@ _stdalgos_5x = cms.VPSet(full_5x,cutbased,PhilV1)
 
 #_chsalgos_4x = cms.VPSet(full,   cutbased) 
 _chsalgos_5x = cms.VPSet(full_5x_chs,cutbased)
-_chsalgos_7x = cms.VPSet(full_74x_chs,cutbased)
+_chsalgos_74x = cms.VPSet(full_74x_chs,cutbased)
+_chsalgos_76x = cms.VPSet(full_76x_chs,cutbased)
 
-_stdalgos    = _chsalgos_7x
+_stdalgos    = _chsalgos_76x
 
 # Calculate+store variables and run MVAs
 pileupJetId = cms.EDProducer('PileupJetIdProducer',
diff --git a/RecoJets/JetProducers/src/PileupJetIdAlgo.cc b/RecoJets/JetProducers/src/PileupJetIdAlgo.cc
index 2cb16d2..e9c86b5 100644
--- a/RecoJets/JetProducers/src/PileupJetIdAlgo.cc
+++ b/RecoJets/JetProducers/src/PileupJetIdAlgo.cc
@@ -27,10 +27,10 @@ PileupJetIdAlgo::PileupJetIdAlgo(const edm::ParameterSet & ps, bool runMvas)
 	  {
 	    etaBinnedWeights_ = ps.getParameter<bool>("etaBinnedWeights");
 	    if(etaBinnedWeights_){
-	      tmvaWeights_jteta_0_2_        = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_0_2")).fullPath();
-	      tmvaWeights_jteta_2_2p5_      = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_2_2p5")).fullPath();
-	      tmvaWeights_jteta_2p5_3_      = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_2p5_3")).fullPath();
-	      tmvaWeights_jteta_3_5_        = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_3_5")).fullPath();
+	      tmvaWeights_jteta_0_2p5_    = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_0_2p5")).fullPath();
+	      tmvaWeights_jteta_2p5_2p75_ = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_2p5_2p75")).fullPath();
+	      tmvaWeights_jteta_2p75_3_   = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_2p75_3")).fullPath();
+	      tmvaWeights_jteta_3_5_      = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights_jteta_3_5")).fullPath();
 	    }
 	    else{
 	      tmvaWeights_                  = edm::FileInPath(ps.getParameter<std::string>("tmvaWeights")).fullPath();  
@@ -142,10 +142,10 @@ void setPtEtaPhi(const reco::Candidate & p, float & pt, float & eta, float &phi
 void PileupJetIdAlgo::bookReader()
 {
 	if(etaBinnedWeights_){
-		reader_jteta_0_2_ = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
-		reader_jteta_2_2p5_ = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
-		reader_jteta_2p5_3_ = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
-		reader_jteta_3_5_ = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
+		reader_jteta_0_2p5_    = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
+		reader_jteta_2p5_2p75_ = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
+		reader_jteta_2p75_3_   = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
+		reader_jteta_3_5_      = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
 	} else {
 		reader_ = std::unique_ptr<TMVA::Reader>(new TMVA::Reader("!Color:Silent"));
 	}
@@ -154,9 +154,9 @@ void PileupJetIdAlgo::bookReader()
 		if(  tmvaNames_[*it].empty() ) { 
 			tmvaNames_[*it] = *it;
 		}
-		reader_jteta_0_2_->AddVariable( *it, variables_[ tmvaNames_[*it] ].first );
-		reader_jteta_2_2p5_->AddVariable( *it, variables_[ tmvaNames_[*it] ].first );
-		reader_jteta_2p5_3_->AddVariable( *it, variables_[ tmvaNames_[*it] ].first );
+		reader_jteta_0_2p5_->AddVariable( *it, variables_[ tmvaNames_[*it] ].first );
+		reader_jteta_2p5_2p75_->AddVariable( *it, variables_[ tmvaNames_[*it] ].first );
+		reader_jteta_2p75_3_->AddVariable( *it, variables_[ tmvaNames_[*it] ].first );
 	  }
 	  for(std::vector<std::string>::iterator it=tmvaVariables_jteta_3_5_.begin(); it!=tmvaVariables_jteta_3_5_.end(); ++it) {
 		if(  tmvaNames_[*it].empty() ) { 
@@ -177,18 +177,18 @@ void PileupJetIdAlgo::bookReader()
 			tmvaNames_[*it] = *it;
 		}
 		if(etaBinnedWeights_){
-			reader_jteta_0_2_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
-			reader_jteta_2_2p5_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
-			reader_jteta_2p5_3_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
+			reader_jteta_0_2p5_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
+			reader_jteta_2p5_2p75_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
+			reader_jteta_2p75_3_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
 			reader_jteta_3_5_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
 		} else {
 			reader_->AddSpectator( *it, variables_[ tmvaNames_[*it] ].first );
 		}
 	}
 	if(etaBinnedWeights_){
-		reco::details::loadTMVAWeights(reader_jteta_0_2_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_0_2_.c_str() ); 
-		reco::details::loadTMVAWeights(reader_jteta_2_2p5_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_2_2p5_.c_str() ); 
-		reco::details::loadTMVAWeights(reader_jteta_2p5_3_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_2p5_3_.c_str() ); 
+		reco::details::loadTMVAWeights(reader_jteta_0_2p5_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_0_2p5_.c_str() ); 
+		reco::details::loadTMVAWeights(reader_jteta_2p5_2p75_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_2p5_2p75_.c_str() ); 
+		reco::details::loadTMVAWeights(reader_jteta_2p75_3_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_2p75_3_.c_str() ); 
 		reco::details::loadTMVAWeights(reader_jteta_3_5_.get(),  tmvaMethod_.c_str(), tmvaWeights_jteta_3_5_.c_str() ); 
 	} else {
 		reco::details::loadTMVAWeights(reader_.get(),  tmvaMethod_.c_str(), tmvaWeights_.c_str() ); 
@@ -211,9 +211,9 @@ void PileupJetIdAlgo::runMva()
 			internalId_.mva_ = -2.;
 		} else {
 			if(etaBinnedWeights_){
-			  if(std::abs(internalId_.jetEta_)<=2.) internalId_.mva_ = reader_jteta_0_2_->EvaluateMVA( tmvaMethod_.c_str() );
-			  else if(std::abs(internalId_.jetEta_)<=2.5) internalId_.mva_ = reader_jteta_2_2p5_->EvaluateMVA( tmvaMethod_.c_str() );
-			  else if(std::abs(internalId_.jetEta_)<=3.) internalId_.mva_ = reader_jteta_2p5_3_->EvaluateMVA( tmvaMethod_.c_str() );
+			  if(std::abs(internalId_.jetEta_)<=2.5) internalId_.mva_ = reader_jteta_0_2p5_->EvaluateMVA( tmvaMethod_.c_str() );
+			  else if(std::abs(internalId_.jetEta_)>2.5 && std::abs(internalId_.jetEta_)<=2.75) internalId_.mva_ = reader_jteta_2p5_2p75_->EvaluateMVA( tmvaMethod_.c_str() );
+			  else if(std::abs(internalId_.jetEta_)>2.75 && std::abs(internalId_.jetEta_)<=3.) internalId_.mva_ = reader_jteta_2p75_3_->EvaluateMVA( tmvaMethod_.c_str() );
 			  else internalId_.mva_ = reader_jteta_3_5_->EvaluateMVA( tmvaMethod_.c_str() );
 			} else {
 			  internalId_.mva_ = reader_->EvaluateMVA( tmvaMethod_.c_str() );
@@ -494,17 +494,25 @@ PileupJetIdentifier PileupJetIdAlgo::computeIdVariables(const reco::Jet * jet, f
 	if ( lLeadEm == nullptr )   { lLeadEm   = lTrail; }
 	if ( lLeadCh == nullptr )   { lLeadCh   = lTrail; }
 	
-	internalId_.nCharged_    = pfjet->chargedMultiplicity();
-	internalId_.nNeutrals_   = pfjet->neutralMultiplicity();
-	internalId_.chgEMfrac_   = pfjet->chargedEmEnergy()    /jet->energy();
-	internalId_.neuEMfrac_   = pfjet->neutralEmEnergy()    /jet->energy();
-	internalId_.chgHadrfrac_ = pfjet->chargedHadronEnergy()/jet->energy();
-	internalId_.neuHadrfrac_ = pfjet->neutralHadronEnergy()/jet->energy();
-	internalId_.nParticles_ = jet->numberOfDaughters();
-
+	if( patjet != nullptr ) { // to enable running on MiniAOD slimmedJets
+	 internalId_.nCharged_    = patjet->chargedMultiplicity();
+	 internalId_.nNeutrals_   = patjet->neutralMultiplicity();
+	 internalId_.chgEMfrac_   = patjet->chargedEmEnergy()    /jet->energy();
+	 internalId_.neuEMfrac_   = patjet->neutralEmEnergy()    /jet->energy();
+	 internalId_.chgHadrfrac_ = patjet->chargedHadronEnergy()/jet->energy();
+	 internalId_.neuHadrfrac_ = patjet->neutralHadronEnergy()/jet->energy();
+	} else {
+	 internalId_.nCharged_    = pfjet->chargedMultiplicity();
+	 internalId_.nNeutrals_   = pfjet->neutralMultiplicity();
+	 internalId_.chgEMfrac_   = pfjet->chargedEmEnergy()    /jet->energy();
+	 internalId_.neuEMfrac_   = pfjet->neutralEmEnergy()    /jet->energy();
+	 internalId_.chgHadrfrac_ = pfjet->chargedHadronEnergy()/jet->energy();
+	 internalId_.neuHadrfrac_ = pfjet->neutralHadronEnergy()/jet->energy();
+	}
+        internalId_.nParticles_   = jet->nConstituents();
 
 	///////////////////////pull variable///////////////////////////////////
-	float sumW(0.0), sumW2(0.0);
+	float sumW2(0.0);
 	float sum_deta(0.0),sum_dphi(0.0);
 	float ave_deta(0.0), ave_dphi(0.0);
 	for (size_t j = 0; j < jet->numberOfDaughters(); j++) {
@@ -520,7 +528,7 @@ PileupJetIdentifier PileupJetIdAlgo::computeIdVariables(const reco::Jet * jet, f
 	  float dphi = reco::deltaPhi(*part, *jet);
 	  sum_deta     += deta*weight2;
 	  sum_dphi     += dphi*weight2;
-	  if (sumW > 0) {
+	  if (sumW2 > 0) {
 	    ave_deta = sum_deta/sumW2;
 	    ave_dphi = sum_dphi/sumW2;
 	  }
EOF