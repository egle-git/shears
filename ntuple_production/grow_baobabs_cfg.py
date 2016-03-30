import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import re

process = cms.Process("GrowBoababs")

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 10
process.MessageLogger.suppressWarning = cms.untracked.vstring('ecalLaserCorrFilter','manystripclus53X','toomanystripclus53X')
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
#process.options.allowUnscheduled = cms.untracked.bool(True)

# Load the standard set of configuration modules
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.source = cms.Source("PoolSource",
                            fileNames =  cms.untracked.vstring(
'/store/mc/RunIIFall15MiniAODv2/WJetsToLNu_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/00000/0C765598-8BD1-E511-BF63-20CF3027A566.root'
#'/store/data/Run2015D/DoubleMuon/MINIAOD/PromptReco-v4/000/258/159/00000/0C6D4AB0-6F6C-E511-8A64-02163E0133CD.root'
#'/store/data/Run2015D/DoubleMuon/MINIAOD/16Dec2015-v1/10000/00039A2E-D7A7-E511-98EE-3417EBE64696.root'
  )
)

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(10))

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
opt.register('dataTier', '', VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, 'Data tier of input dataset, typically AOD, AODSIM, MINIAOD or MINIAODSIM')
opt.register('isMC',    -1, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, 'Flag indicating if the input samples are from MC (1) or from the detector (0).')
opt.register('makeEdm', 0, VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int, 'Switch for EDM output production. Use 0 (default) to disable it, 1 to enable it.')

opt.parseArguments()

if opt.isMC < 0 and len(process.source.fileNames) > 0:
  if re.match(r'.*/(MINI)?AODSIM/.*', process.source.fileNames[0]):
    print "MC dataset detected."
    opt.isMC = 1
  elif re.match(r'.*/(MINI)?AOD/.*', process.source.fileNames[0]):
    print "Real data dataset detected."
    opt.isMC = 0
  #endif
#endif

if opt.isMC < 0:
  raise Exception("Failed to detect data type. Data type need to be specify with the isMC cmsRun command line option")
#endif

#for 76x:
dataGlobalTag = '76X_dataRun2_16Dec2015_v0'
mcGlobalTag = '76X_mcRun2_asymptotic_RunIIFall15DR76_v1'
reapply_jec = True
jec_file = False
eg_corr = True   #photon and electron correction


#------------------------------------
#Condition DB tag
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag

if opt.isMC == 1:
  process.GlobalTag = GlobalTag(process.GlobalTag, mcGlobalTag, '')
else:
  process.GlobalTag = GlobalTag(process.GlobalTag, dataGlobalTag, '')
#fi

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
  #endif jec_file

  jec_levels = ['L1FastJet', 'L2Relative', 'L3Absolute'] 
  
  if not opt.isMC:
    jec_levels.append('L2L3Residual')
  #endif

  process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
  process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
    src = cms.InputTag("slimmedJets"),
    levels = jec_levels,
    payload = 'AK4PFchs' # Make sure to choose the appropriate levels and payload here!
  )

  process.updatedJets = process.patJetsUpdated.clone(
    jetSource = cms.InputTag("slimmedJets"),
    jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
    )

  jetSrc = "updatedJets"
  puMvaName = 'pileupJetIdUpdated:fullDiscriminant'
  process.updatedJets.userData.userFloats.src += [ puMvaName ]

  process.load("RecoJets.JetProducers.PileupJetID_cfi")
  process.pileupJetIdUpdated = process.pileupJetId.clone(
    jets=cms.InputTag("slimmedJets"),
    inputIsCorrected=True,
    applyJec=True,
    vertexes=cms.InputTag("offlineSlimmedPrimaryVertices")
    )
  #print process.pileupJetIdUpdated.dumpConfig()

  ### ---------------------------------------------------------------------------
  ### Removing the HF from the MET computation
  ### ---------------------------------------------------------------------------
  #process.noHFCands = cms.EDFilter("CandPtrSelector",
  #                                 src=cms.InputTag("packedPFCandidates"),
  #                                 cut=cms.string("abs(pdgId)!=1 && abs(pdgId)!=2 && abs(eta)<3.0")
  #                                 )

  #jets are rebuilt from those candidates by the tools, no need to do anything else
  ### =================================================================================

  from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
  process.load('Configuration.StandardSequences.MagneticField_38T_cff')

  #default configuration for miniAOD reprocessing, change the isData flag to run on data
  #for a full met computation, remove the pfCandColl input
  # runMetCorAndUncFromMiniAOD(process,
  #                           isData = not opt.isMC,
  #                           )

  # runMetCorAndUncFromMiniAOD(process,
  #                            isData=not opt.isMC,
  #                            pfCandColl=cms.InputTag("noHFCands"),
  #                            reclusterJets=True,   #needed for NoHF
  #                            recoMetFromPFCs=True, #needed for NoHF
  #                            postfix="NoHF"
  #                            )
else:
  jetSrc = "slimmedJets"
  puMvaName = 'pileupJetId:fullDiscriminant'
#endif reapply_jec
#
#--------------------------------------------

# Photon and electron correction
#
if eg_corr:
  process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    calibratedPatElectrons = cms.PSet(
        initialSeed = cms.untracked.uint32(757132),
        engineName = cms.untracked.string('TRandom3')
    ),
    calibratedPatPhotons = cms.PSet(
        initialSeed = cms.untracked.uint32(1294),
        engineName = cms.untracked.string('TRandom3')
    ),
  )

 # copied from  EgammaAnalysis/ElectronTools/python/calibratedPhotonsRun2_cfi.py:
  process.calibratedPatPhotons = cms.EDProducer("CalibratedPatPhotonProducerRun2",
                                                # input collections
                                                photons = cms.InputTag('slimmedPhotons'),                                                                                     # data or MC corrections
                                                # if isMC is false, data corrections are applied
                                                isMC = cms.bool(opt.isMC != 0),
                                                # set to True to get special "fake" smearing for synchronization. Use JUST in case of synchronization
                                                isSynchronization = cms.bool(False),
                                                correctionFile = cms.string("EgammaAnalysis/ElectronTools/data/76X_16DecRereco_2015"))
  
  #copied from  EgammaAnalysis/ElectronTools/python/calibratedElectronsRun2_cfi.py')
  process.calibratedPatElectrons = cms.EDProducer("CalibratedPatElectronProducerRun2", 
                                                  # input collections
                                                  electrons = cms.InputTag('slimmedElectrons'),
                                                  gbrForestName = cms.string("gedelectron_p4combination_25ns"),
                                                  # data or MC corrections
                                                  # if isMC is false, data corrections are applied
                                                  isMC = cms.bool(opt.isMC != 0),
                                                  # set to True to get special "fake" smearing for synchronization. Use JUST in case of synchronization
                                                  isSynchronization = cms.bool(False),
                                                  correctionFile = cms.string("EgammaAnalysis/ElectronTools/data/76X_16DecRereco_2015")
                                                  )
  electronSrc = "calibratedPatElectrons"
  photonSrc   = "calibratedPatPhotons"
else:
  electronSrc = "slimmedElectrons"
  photonSrc   = "slimmedPhotons"
#--------------------------------------------
  
from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector
process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlineSlimmedPrimaryVertices')
    )

process.tupel = cms.EDAnalyzer("Tupel",
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
  candidateSw  = cms.untracked.string("withTrack"), #on, off, or withTrack
  candidateSrc = cms.untracked.InputTag("packedPFCandidates"),
  photonSw     = cms.untracked.string("on"), #on or off
  photonSrc    = cms.untracked.InputTag(photonSrc),
  electronSrc  = cms.untracked.InputTag(electronSrc),
  muonSrc      = cms.untracked.InputTag("slimmedMuons"),
  jetSrc       = cms.untracked.InputTag(jetSrc),
  genSrc       = cms.untracked.InputTag("prunedGenParticles"),
  gjetSrc      = cms.untracked.InputTag('slimmedGenJets'),
  muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
  muonMatch2   = cms.string( 'muonTriggerMatchHLTMuons2' ),
  elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
  mSrcRho      = cms.untracked.InputTag('fixedGridRhoFastjetAll'),#arbitrary rho now
  CalojetLabel = cms.untracked.InputTag('slimmedJets'), #same collection now BB 
  metSrcs      = cms.VInputTag("slimmedMETs","slimmedMETsNoHF","slimmedMETsPuppi"),
  lheSrc       = cms.untracked.InputTag('source'),
  puSrc        = cms.untracked.InputTag('slimmedAddPileupInfo'),
  puMvaName    = cms.untracked.string(puMvaName),
  puJetIdSrc   = cms.untracked.InputTag("pileupJetIdUpdated"),
  pvSrc        = cms.untracked.InputTag('goodOfflinePrimaryVertices'),
  reducedBarrelRecHitCollection = cms.InputTag("reducedEgamma","reducedEBRecHits"),
  reducedEndcapRecHitCollection = cms.InputTag("reducedEgamma","reducedEERecHits"),
  reducedPreshowerRecHitCollection = cms.InputTag("reducedEgamma","reducedESRecHits")
)

process.p = cms.Path()

if reapply_jec:
  process.p += cms.Sequence( process.pileupJetIdUpdated + process.patJetCorrFactorsReapplyJEC + process.updatedJets )

if eg_corr:
  process.p += process.calibratedPatElectrons 
  process.p += process.calibratedPatPhotons

process.p += process.goodOfflinePrimaryVertices
process.p += process.tupel

if opt.makeEdm:
  process.out = cms.OutputModule("PoolOutputModule",
                                 fileName = cms.untracked.string('edm_out.root'),
                                 outputCommands = cms.untracked.vstring('keep *')
                                 )

  process.outpath = cms.EndPath(process.out)
#endif makeEdm

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
