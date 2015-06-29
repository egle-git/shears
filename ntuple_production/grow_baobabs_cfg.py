import FWCore.ParameterSet.Config as cms

process = cms.Process("GrowBoababs")
process.source = cms.Source("PoolSource")

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('ntuple.root' )
)

process.tupel = cms.EDAnalyzer("Tupel",
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
  photonSrc   = cms.untracked.InputTag("slimmedPhotons"),
  electronSrc = cms.untracked.InputTag("slimmedElectrons"),
  muonSrc     = cms.untracked.InputTag("slimmedMuons"),
  jetSrc      = cms.untracked.InputTag("slimmedJets"),
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

process.p = cms.Path(
 process.tupel 
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
process.options.allowUnscheduled = cms.untracked.bool(True)


#iFileName = "fileNameDump_cfg.py"
#file = open(iFileName,'w')
#file.write(str(process.dumpPython()))
#file.close()
