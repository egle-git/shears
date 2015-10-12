import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("GrowBoababs")

process.source = cms.Source("PoolSource",
#                            fileNames =  cms.untracked.vstring(
#'/store/data/Run2015C/SinglePhoton/MINIAOD/PromptReco-v1/000/253/620/00000/6C6DC194-1C40-E511-A671-02163E014324.root')
)
#process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1000))

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string('ntuple.root' )
)

# setup 'analysis'  options
opt = VarParsing.VarParsing ('analysis')
# Addition options.
# Note: if you add an option, update the code which write the values 
# in the configuration dump, you can find at the end of this file.
opt.register('minRun', 1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, 'Gives an indication on the minimum run number included in the input samples.')
opt.register('maxRun', 999999, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, 'Gives an indication on the maximum run number include in the input samples.')
opt.register('prodEra', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, 'Production run era. Label used to identify a run period whose data are processed together.')
opt.register('recoTag', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, 'Tag of the recontruction.')
opt.register('isMC',    0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, 'Flag indicating if the input samples are from MC (1) or from the detector (0).')


opt.parseArguments()

if opt.prodEra == '13TeV_25ns':
  reapply_jec=True
  jec_file='Summer15_25nsV5_DATA.db'
  jec_file_tag = 'JetCorrectorParametersCollection_Summer15_25nsV5_DATA_AK4PFchs'
  mcGlobalTag='74X_mcRun2_asymptotic_v2'
elif opt.prodEra == '13TeV_50ns':
  reapply_jec=True
  jec_file = None
  mcGlobalTag='74X_mcRun2_startup_v2'
  dataGlobalTag='74X_dataRun2_v2'
else:
  reapply_jec = False
#endif

#--------------------------------------
#JEC
#
if reapply_jec:
  if jec_file:
    process.load("CondCore.DBCommon.CondDBCommon_cfi")
    from CondCore.DBCommon.CondDBSetup_cfi import *
    process.jec = cms.ESSource("PoolDBESSource",
          DBParameters = cms.PSet(
            messageLevel = cms.untracked.int32(0)
            ),
          timetype = cms.string('runnumber'),
          toGet = cms.VPSet(
          cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string(jec_file_tag),
                label  = cms.untracked.string('AK4PFchs')
                ),
          ), 
          connect = cms.string('sqlite:' + jec_file)
    )
    process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')
  else:
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
    from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
    if isMC:
      process.GlobalTag = GlobalTag(process.GlobalTag, mcGlobalTag, '')
      process.patJetCorrFactorsReapplyJEC = patJetCorrFactorsUpdated.clone(
      src = cms.InputTag("slimmedJets"),
      levels = ['L1FastJet', 
                'L2Relative',
                 'L3Absolute'],
      payload = 'AK4PFchs' ) # Make sure to choose the appropriate levels and payload here!
    else: #real data
      process.GlobalTag = GlobalTag(process.GlobalTag, dataGlobalTag, '')
      process.patJetCorrFactorsReapplyJEC = patJetCorrFactorsUpdated.clone(
      src = cms.InputTag("slimmedJets"),
      levels = ['L1FastJet', 
                'L2Relative',
                'L3Absolute',
                'L2L3Residual'],
      payload = 'AK4PFchs' ) # Make sure to choose the appropriate levels and payload here!
    #endif isMC
  #endif jec_file

  from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated

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

iFileName = "configDump_cfg.py"
file = open(iFileName,'w')
file.write(str(process.dumpPython()))
file.write('''
opt.minRun   = %d
opt.maxRun   = %d
opt.prodEra  = %s
opt.recoTag  = %s
opt.isMC     = %d
''' % (opt.minRun, opt.maxRun, opt.prodEra, opt.recoTag, opt.isMC))
file.close()
