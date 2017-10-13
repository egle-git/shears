import FWCore.ParameterSet.Config as cms

process = cms.Process("BAOBAB")

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#"/store/data/Run2016B/DoubleMuon/MINIAOD/23Sep2016-v1/70000/02477A4E-C586-E611-BC6F-02163E013D1C.root"
"/store/mc/RunIISpring16MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PUSpring16RAWAODSIM_reHLT_80X_mcRun2_asymptotic_v14-v1/40000/00200284-F15C-E611-AA9B-002590574776.root"
#"/store/mc/RunIIFall15MiniAODv2/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/00000/0C765598-8BD1-E511-BF63-20CF3027A566.root"
#'/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/00000/06532BBC-05C8-E511-A60A-F46D043B3CE5.root'
))

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('ntuple.root' )
)

# Load the standard set of configuration modules
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag

# For 2015 MC data:
#process.GlobalTag = GlobalTag(process.GlobalTag, '76X_mcRun2_asymptotic_RunIIFall15DR76_v1', '')
#
# For 2016 real data:
#process.GlobalTag = GlobalTag(process.GlobalTag, '80X_dataRun2_2016SeptRepro_v4', '')
#
# For 2016 MC data:
process.GlobalTag = GlobalTag(process.GlobalTag, '80X_mcRun2_asymptotic_2016_miniAODv2_v1', '')

process.tupel = cms.EDAnalyzer("Tupel",
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
  candidateSw  = cms.untracked.string("off"), #on, off, or withTrack
  candidateSrc = cms.untracked.InputTag("packedPFCandidates"),
  photonSw     = cms.untracked.string("on"), #on or off
  photonSrc    = cms.untracked.InputTag("slimmedPhotons"),
  electronSrc  = cms.untracked.InputTag("slimmedElectrons"),
  muonSrc      = cms.untracked.InputTag("slimmedMuons"),
  tauSrc       = cms.untracked.InputTag("slimmedTaus"),
  jetSrc       = cms.untracked.InputTag("slimmedJets"),
  fatJetSw     = cms.untracked.string("on"),
  fatJetSrc    = cms.untracked.InputTag("slimmedJetsAK8"),
  genSrc       = cms.untracked.InputTag("prunedGenParticles"),
  gjetSrc      = cms.untracked.InputTag('slimmedGenJets'),
  gfatJetSrc   = cms.untracked.InputTag('slimmedGenJetsAK8'),
  muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
  muonMatch2   = cms.string( 'muonTriggerMatchHLTMuons2' ),
  elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
  mSrcRho      = cms.untracked.InputTag('fixedGridRhoFastjetAll'),#arbitrary rho now
  CalojetLabel = cms.untracked.InputTag('slimmedJets'), #same collection now BB 
  metSrcs      = cms.VInputTag("slimmedMETs","slimmedMETsNoHF","slimmedMETsPuppi"),
  lheSrc       = cms.untracked.InputTag('externalLHEProducer'),
  puSrc        = cms.untracked.InputTag('slimmedAddPileupInfo'),
  puMvaName    = cms.untracked.string('pileupJetId:fullDiscriminan'),
  puJetIdSrc   = cms.untracked.InputTag("pileupJetId"),
  pvSrc        = cms.untracked.InputTag('goodOfflinePrimaryVertices'),
  reducedBarrelRecHitCollection = cms.InputTag("reducedEgamma","reducedEBRecHits"),
  reducedEndcapRecHitCollection = cms.InputTag("reducedEgamma","reducedEERecHits"),
  reducedPreshowerRecHitCollection = cms.InputTag("reducedEgamma","reducedESRecHits"),
  triggerMenu = cms.untracked.string("2015")
)

from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlineSlimmedPrimaryVertices')
    )


process.p = cms.Path(
 process.tupel 
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)
