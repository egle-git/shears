import FWCore.ParameterSet.Config as cms

process = cms.Process("GrowBoababs")

process.source = cms.Source("PoolSource",
#                            fileNames =  cms.untracked.vstring(
#'/store/data/Run2015C/SinglePhoton/MINIAOD/PromptReco-v1/000/253/620/00000/6C6DC194-1C40-E511-A671-02163E014324.root')
)
#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('ntuple.root' )
)

reapply_jec=False
isMC=False
mcGlobalTag='74X_mcRun2_asymptotic_v2'
dataGlobalTag='74X_dataRun2_v2'

#--------------------------------------
#JEC
#
if reapply_jec:
  process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
  from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
  if isMC:
    process.GlobalTag = GlobalTag(process.GlobalTag, mcGlobalTag, '')
  else:
    process.GlobalTag = GlobalTag(process.GlobalTag, dataGlobalTag, '')
  #endif iMC

  from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated
  process.patJetCorrFactorsReapplyJEC = patJetCorrFactorsUpdated.clone(
    src = cms.InputTag("slimmedJets"),
    levels = ['L1FastJet', 
              'L2Relative',
              'L3Absolute'],
    payload = 'AK4PFchs' ) # Make sure to choose the appropriate levels and payload here!

  from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetsUpdated
  process.patJetsReapplyJEC = patJetsUpdated.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
    )

  jetSrc = "patJetsReapplyJEC"
else:
  jetSrc = "slimmedJets"
#endif reapply_jec
#
#--------------------------------------------

process.tupel = cms.EDAnalyzer("Tupel",
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
  photonSrc   = cms.untracked.InputTag("slimmedPhotons"),
  electronSrc = cms.untracked.InputTag("slimmedElectrons"),
  muonSrc     = cms.untracked.InputTag("slimmedMuons"),
  jetSrc      = cms.untracked.InputTag(jetSrc),
  metSrc      = cms.untracked.InputTag("patMETsPF"),
  genSrc      = cms.untracked.InputTag("prunedGenParticles"),
  gjetSrc       = cms.untracked.InputTag('slimmedGenJets'),
  muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
  muonMatch2    = cms.string( 'muonTriggerMatchHLTMuons2' ),
  elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
  mSrcRho      = cms.untracked.InputTag('fixedGridRhoFastjetAll'),#arbitrary rho now
  CalojetLabel = cms.untracked.InputTag('slimmedJets'), #same collection now BB 
  metSource = cms.VInputTag("slimmedMETs","slimmedMETs","slimmedMETs","slimmedMETs"), #no MET corr yet
  lheSource=cms.untracked.InputTag('source')
)

from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlineSlimmedPrimaryVertices')
    )

process.p = cms.Path()

if reapply_jec:
  process.p += cms.Sequence( process.patJetCorrFactorsReapplyJEC + process.patJetsReapplyJEC )

process.p += process.tupel

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)

#process.out = cms.OutputModule("PoolOutputModule",
#                               fileName = cms.untracked.string('edm_out.root'))
#
#process.outpath = cms.EndPath(process.out)

#iFileName = "fileNameDump_cfg.py"
#file = open(iFileName,'w')
#file.write(str(process.dumpPython()))
#file.close()
