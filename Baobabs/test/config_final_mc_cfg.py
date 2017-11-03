import FWCore.ParameterSet.Config as cms
import os
import sys
import FWCore.ParameterSet.VarParsing as VarParsing

process = cms.Process("PAT")
###### Remove pattuple at output####
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from PhysicsTools.PatAlgos.patEventContent_cff import *

process.TotalEventCounter = cms.EDProducer("EventCountProducer")
process.TotalEventCounter = cms.EDProducer("EventCountProducer")
process.AfterPVFilterCounter = cms.EDProducer("EventCountProducer")
process.AfterNSFilterCounter = cms.EDProducer("EventCountProducer")
process.AfterPATCounter = cms.EDProducer("EventCountProducer")
process.AfterCandidatesCounter = cms.EDProducer("EventCountProducer")
process.AfterJetsCounter = cms.EDProducer("EventCountProducer")

process.out = cms.OutputModule("PoolOutputModule",
                               #fileName = cms.untracked.string('MC_summer2012.root'),
                               fileName = cms.untracked.string('PAT_CHS_summer2012.root'),
			       outputCommands = cms.untracked.vstring('drop *')	
				)

from PhysicsTools.PatAlgos.tools.coreTools import *
process.load("Configuration.Geometry.GeometryIdeal_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('RecoMET.METFilters.metFilters_cff')
process.load('RecoBTag/Configuration/RecoBTag_cff')
process.hcalLaserEventFilter.taggingMode   = cms.bool(True)
process.EcalDeadCellTriggerPrimitiveFilter.taggingMode=cms.bool(True)
process.eeBadScFilter.taggingMode           = cms.bool(True)
process.ecalLaserCorrFilter.taggingMode     = cms.bool(True)
process.trackingFailureFilter.VertexSource  = cms.InputTag('goodOfflinePrimaryVertices')
process.trackingFailureFilter.taggingMode   = cms.bool(True)
process.manystripclus53X.taggedMode         = cms.untracked.bool(True)
process.manystripclus53X.forcedValue        = cms.untracked.bool(False)
process.toomanystripclus53X.taggedMode      = cms.untracked.bool(True)
process.toomanystripclus53X.forcedValue     = cms.untracked.bool(False)

#isMC = False
isMC=True

## global tag for data
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
if isMC :
    process.GlobalTag.globaltag = 'START53_V27::All'
else :
    process.GlobalTag.globaltag = 'FT53_V21A_AN6::All'

# Jet energy corrections to use:
inputJetCorrLabel = ('AK5PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual'])
inputJetCorrLabelCalo = ('AK5Calo', ['L1FastJet', 'L2Relative', 'L3Absolute','L2L3Residual'])
if isMC:
     inputJetCorrLabel = ('AK5PFchs',['L1FastJet', 'L2Relative', 'L3Absolute'])
     inputJetCorrLabelCalo = ('AK5Calo',['L1FastJet', 'L2Relative', 'L3Absolute'])
##-------------------- Import the Jet RECO modules -----------------------
process.load('RecoJets.Configuration.RecoPFJets_cff')
##-------------------- Turn-on the FastJet density calculation -----------------------
process.kt6PFJets.doRhoFastjet = True
process.ak5PFJets.doAreaFastjet = True

process.patElectrons.electronIDSources = cms.PSet(
    #MVA
    mvaTrigV0 = cms.InputTag("mvaTrigV0"),
    mvaNonTrigV0 = cms.InputTag("mvaNonTrigV0"),
)

process.load('EgammaAnalysis.ElectronTools.electronIdMVAProducer_cfi')
process.mvaID = cms.Sequence(  process.mvaTrigV0 + process.mvaNonTrigV0 )


# Configure PAT to use PF2PAT instead of AOD sources
# this function will modify the PAT sequences.
from PhysicsTools.PatAlgos.tools.pfTools import *

# An empty postfix means that only PF2PAT is run,
# otherwise both standard PAT and PF2PAT are run. In the latter case PF2PAT
# collections have standard names + postfix (e.g. patElectronPFlow)  
from PhysicsTools.SelectorUtils.pvSelector_cfi import pvSelector

process.goodOfflinePrimaryVertices = cms.EDFilter(
    "PrimaryVertexObjectFilter",
    filterParams = pvSelector.clone( minNdof = cms.double(4.0), maxZ = cms.double(24.0) ),
    src=cms.InputTag('offlinePrimaryVertices')
    )
postfix = "PFlow"
jetAlgo="AK5"
usePF2PAT(process,runPF2PAT=True, jetAlgo=jetAlgo, runOnMC=isMC, postfix=postfix,
	  jetCorrections=inputJetCorrLabel,
          pvCollection=cms.InputTag('goodOfflinePrimaryVertices'),
          typeIMetCorrections=False)


process.load('CondCore.DBCommon.CondDBSetup_cfi')#
process.BTauMVAJetTagComputerRecord = cms.ESSource('PoolDBESSource',
    process.CondDBSetup,
    timetype = cms.string('runnumber'),
    toGet = cms.VPSet(cms.PSet(
        record = cms.string('BTauGenericMVAJetTagComputerRcd'),
        tag = cms.string('MVAComputerContainer_Retrained53X_JetTags_v2')
    )),
    connect = cms.string('frontier://FrontierProd/CMS_COND_PAT_000'),
    BlobStreamerName = cms.untracked.string('TBufferBlobStreamingService')
)
process.es_prefer_BTauMVAJetTagComputerRecord = cms.ESPrefer('PoolDBESSource','BTauMVAJetTagComputerRecord')

useGsfElectrons(process,postfix=postfix,dR="03")
process.patMuonsPFlow.pfMuonSource = cms.InputTag("pfSelectedMuonsPFlow") 
process.muonMatchPFlow.src = cms.InputTag("pfSelectedMuonsPFlow")

switchJetCollection(process,
    cms.InputTag('pfNoTau'+postfix),
    doJTA              = True,
    doBTagging         = True,
    btagInfo           = cms.vstring('impactParameterTagInfos','secondaryVertexTagInfos','softPFMuonsTagInfos','softPFElectronsTagInfos'),
    btagdiscriminators = cms.vstring('jetBProbabilityBJetTags','jetProbabilityBJetTags','trackCountingHighPurBJetTags','trackCountingHighEffBJetTags','simpleSecondaryVertexHighEffBJetTags','simpleSecondaryVertexHighPurBJetTags','combinedSecondaryVertexBJetTags','combinedSecondaryVertexV1BJetTags','combinedSecondaryVertexSoftPFLeptonV1BJetTags'),
    jetCorrLabel       = ('AK5PFchs', cms.vstring(['L1FastJet', 'L2Relative', 'L3Absolute'])),
    doType1MET         = False,
    genJetCollection   = cms.InputTag('ak5GenJetsNoNu'),
    doJetID            = False,
    postfix            = postfix
)

process.pfPileUpPFlow.checkClosestZVertex = False
process.pfPileUpIsoPFlow.checkClosestZVertex = cms.bool(False)

process.genParticlesForJetsNoNu.excludeFromResonancePids = cms.vuint32(12, 14, 16)

if not isMC : removeMCMatching(process, ['All'])
if not isMC : removeMCMatchingPF2PAT( process, '' )

# add pf met
from PhysicsTools.PatAlgos.tools.metTools import *
addPfMET(process, 'PF')

# Add PF jets
from PhysicsTools.PatAlgos.tools.jetTools import *
addJetCollection(process,cms.InputTag('ak5CaloJets'),
                'AK5', 'Calo',
                doJTA        = False,
                doBTagging   = False,
                jetCorrLabel = inputJetCorrLabelCalo,
                doType1MET   = True,
                doL1Cleaning = True,
                doL1Counters = False,
                genJetCollection=cms.InputTag("ak5GenJets"),
                doJetID      = False
)
switchJetCollection(process,cms.InputTag('ak5PFJets'),
                 doJTA        = True,
                 doBTagging   = True,
                 jetCorrLabel = inputJetCorrLabel,
                 doType1MET   = True,
                 genJetCollection=cms.InputTag("ak5GenJets"),
                 doJetID      = True
                 )
process.patJets.addTagInfos = True
process.patJets.tagInfoSources  = cms.VInputTag(
    cms.InputTag("secondaryVertexTagInfosAOD"),
   )

if not isMC :
    # require physics declared
    process.load('HLTrigger.special.hltPhysicsDeclared_cfi')
    process.hltPhysicsDeclared.L1GtReadoutRecordTag = 'gtDigis'

    # require scraping filter
    process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                        applyfilter = cms.untracked.bool(True),
                                        debugOn = cms.untracked.bool(False),
                                        numtrack = cms.untracked.uint32(10),
                                        thresh = cms.untracked.double(0.25)
                                        )
    # HB + HE noise filtering
    process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')
    
    
    process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                               vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                               minimumNDOF = cms.uint32(4) ,
                                               maxAbsZ = cms.double(24),
                                               maxd0 = cms.double(2)
                                               )

process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
process.load('RecoJets.Configuration.RecoPFJets_cff')

process.load("JetMETCorrections.Type1MET.pfMETCorrections_cff")
process.load("JetMETCorrections.Type1MET.pfMETCorrectionType0_cfi")
process.pfType1CorrectedMet.applyType0Corrections = cms.bool(False)
process.pfType1CorrectedMet.srcType1Corrections = cms.VInputTag( cms.InputTag('pfMETcorrType0'),
                                                                 cms.InputTag('pfJetMETcorr', 'type1')
                                                                 )




# Add the files
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring()

if isMC:
    readFiles.extend( [ #MC
        'file:/afs/cern.ch/user/b/bbilin/eos/cms/store/user/bbilin/DY_2012_53X.root' #MC
        ] )
else:
    readFiles.extend( [ #data
        '/store/data/Run2012A/DoubleMu/AOD/22Jan2013-v1/30000/FEF469F7-0882-E211-8351-0026189438E6.root'
       ] ) 

process.source = cms.Source("PoolSource",
                            fileNames = readFiles,
                            duplicateCheckMode = cms.untracked.string('noDuplicateCheck'),
                            )

if not isMC :
    process.source.inputCommands = cms.untracked.vstring("keep *", "drop *_MEtoEDMConverter_*_*")

process.tupel = cms.EDAnalyzer("Tupel",
  trigger      = cms.InputTag( "patTrigger" ),
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
  triggerSummaryLabel = cms.InputTag("hltTriggerSummaryAOD","","HLT"), 
  photonSrc   = cms.untracked.InputTag("selectedPhotons"),
  electronSrc = cms.untracked.InputTag("selectedPatElectronsPFlow"),
  muonSrc     = cms.untracked.InputTag("selectedPatMuonsPFlow"),
  #tauSrc      = cms.untracked.InputTag("selectedPatTaus"),
  jetSrc      = cms.untracked.InputTag("selectedPatJetsPFlow"),
  metSrc      = cms.untracked.InputTag("patMETsPF"),
  muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
  muonMatch2    = cms.string( 'muonTriggerMatchHLTMuons2' ),
  elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
  mSrcRho      = cms.untracked.InputTag('kt6PFJets','rho'),
  metSource       = cms.VInputTag("pfMETPFlow","pfMet","pfType1CorrectedMet","pfType1p2CorrectedMet"),
  CalojetLabel = cms.untracked.InputTag('selectedPatJetsAK5Calo')  
)

process.load("CMGTools.External.pujetidsequence_cff")

process.puJetIdChs.jets= cms.InputTag("selectedPatJetsPFlow")
process.puJetMvaChs.jets= cms.InputTag("selectedPatJetsPFlow")

process.puJetIdSqeuenceChs = cms.Sequence(process.puJetIdChs*process.puJetMvaChs)

#process.out.outputCommands.extend(["keep *_puJetId*_*_*", # input variables
#"keep *_puJetMva*_*_*" # final MVAs and working point flags
#])
process.pfIsolatedMuonsPFlow.isolationCut = 0.0
process.pfIsolatedElectronsPFlow.isolationCut = 0.0

process.patseq = cms.Sequence(    
    process.goodOfflinePrimaryVertices*
    getattr(process,"patPF2PATSequence"+postfix)
    )


#process.btagAnalyzer = cms.EDAnalyzer("WrappedEDAnalysisTasksAnalyzerBTag",
#                                      Jets = cms.InputTag("selectedPatJetsPFlow"),
#                                      bTagAlgo=cms.string('trackCounting'),
#                                      bins=cms.uint32(100),
#                                      lowerbin=cms.double(0.),
#                                      upperbin=cms.double(10.)
#)

# let it run
if isMC:
    process.p = cms.Path(
        process.kt6PFJets *
        process.mvaID *
        process.type0PFMEtCorrection*	process.producePFMETCorrections*
        process.patseq*
        process.patDefaultSequence*
	process.puJetIdSqeuenceChs* #for (PFlow) CHS objects
	process.btagging* 
        process.tupel
        )
else :
    process.p = cms.Path(
        process.HBHENoiseFilter*
        process.hcalLaserEventFilter *
        process.EcalDeadCellTriggerPrimitiveFilter *
        process.eeBadScFilter *
        process.ecalLaserCorrFilter *
        process.trkPOGFilters* 
        process.scrapingVeto*
        process.primaryVertexFilter*
        process.kt6PFJets *
        process.mvaID *
        process.type0PFMEtCorrection*	process.producePFMETCorrections*
        process.patseq*
        process.trackingFailureFilter *
        process.patDefaultSequence*
	process.puJetIdSqeuenceChs*
        process.tupel
        )
#process.outpath = cms.EndPath(
#    process.out
#    )

#if not postfix=="":
#    process.p += process.patDefaultSequence

#process.p +=process.tupel

# top projections in PF2PAT:
getattr(process,"pfNoPileUp"+postfix).enable = True
getattr(process,"pfNoMuon"+postfix).enable = False
getattr(process,"pfNoElectron"+postfix).enable = False
getattr(process,"pfNoTau"+postfix).enable = False 
getattr(process,"pfNoJet"+postfix).enable = True

# verbose flags for the PF2PAT modules
getattr(process,"pfNoMuon"+postfix).verbose = False


# rename output file
if isMC:
    process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('DYJetsToLL_ntuple.root' )
                                       )
else:
    process.TFileService = cms.Service("TFileService",
                                       fileName = cms.string('DoubleMu_Run2012_ntuple.root' )
                                       )

process.load("FWCore.MessageService.MessageLogger_cfi")
# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10)
# process all the events
process.maxEvents = cms.untracked.PSet(  input = cms.untracked.int32(10) )
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(False) )

## ---
## PAT trigger matching
## --
process.muonTriggerMatchHLTMuons = cms.EDProducer(
        # matching in DeltaR, sorting by best DeltaR
        #"PATTriggerMatcherDRDPtLessByR"
	"PATTriggerMatcherDRLessByR"
        # matcher input collections
        , src     = cms.InputTag( 'selectedPatMuonsPFlow' )
        , matched = cms.InputTag( 'patTrigger' )
        # selections of trigger objects
        , matchedCuts = cms.string( 'type( "TriggerMuon" ) && path( "HLT_Mu17_Mu8_v*")' ) # input does not yet have the 'saveTags' parameter in HLT
        # selection of matches
        , maxDPtRel   = cms.double( 0.5 ) 
        , maxDeltaR   = cms.double( 0.3 )
        , maxDeltaEta = cms.double( 0.2 ) # no effect here
        # definition of matcher output
        , resolveAmbiguities    = cms.bool( True )
        , resolveByMatchQuality = cms.bool( True )
        )
##############################
process.muonTriggerMatchHLTMuons2 = cms.EDProducer(
        ## matching in DeltaR, sorting by best DeltaR
        #"PATTriggerMatcherDRDPtLessByR"
        "PATTriggerMatcherDRLessByR"
        ## matcher input collections
        , src     = cms.InputTag( 'selectedPatMuonsPFlow' )
        , matched = cms.InputTag( 'patTrigger' )
        ## selections of trigger objects
        , matchedCuts = cms.string( 'type( "TriggerMuon" ) && path( "HLT_Mu17_TkMu8_v*")' ) # input does not yet have the 'saveTags' parameter in HLT
        ## selection of matches
        , maxDPtRel   = cms.double( 0.5 ) 
        , maxDeltaR   = cms.double( 0.3 )
        , maxDeltaEta = cms.double( 0.2 ) # no effect here
        ## definition of matcher output
        , resolveAmbiguities    = cms.bool( True )
        , resolveByMatchQuality = cms.bool( True )
        )
##############################
### ============

pathTriggerEle ='path("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v*",0,0) && filter("hltEle17TightIdLooseIsoEle8TightIdLooseIsoTrackIsoDoubleFilter")'

process.elecTriggerMatchHLTElecs = cms.EDProducer(
        # matching in DeltaR, sorting by best DeltaR
    "PATTriggerMatcherDRLessByR"
        # matcher input collections
    , src     = cms.InputTag( 'selectedPatElectronsPFlow' )
    , matched = cms.InputTag( 'patTrigger' )
        # selections of trigger objects
    , matchedCuts = cms.string(pathTriggerEle) # input does not yet have the 'saveTags' parameter in HLT
        # selection of matches
    , maxDPtRel   = cms.double( 0.5 ) 
    , maxDeltaR   = cms.double( 0.3 )
    , maxDeltaEta = cms.double( 0.2 ) # no effect here
        # definition of matcher output
    , resolveAmbiguities    = cms.bool( True )
    , resolveByMatchQuality = cms.bool( True )
)
## --
## Switch on PAT trigger
## --
from PhysicsTools.PatAlgos.tools.trigTools import *
switchOnTrigger( process ) # This is optional and can be omitted.
switchOnTriggerMatching( process, triggerMatchers = [ 'muonTriggerMatchHLTMuons','muonTriggerMatchHLTMuons2','elecTriggerMatchHLTElecs' ] )
# Switch to selected PAT objects in the trigger matching
removeCleaningFromTriggerMatching( process )
##############################
iFileName = "fileNameDump_cfg.py"
file = open(iFileName,'w')
file.write(str(process.dumpPython()))
file.close()

