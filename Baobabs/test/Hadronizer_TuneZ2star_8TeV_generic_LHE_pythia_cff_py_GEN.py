# Auto generated configuration file
# using: 
# Revision: 1.381.2.27 
# Source: /local/reps/CMSSW/CMSSW/Configuration/PyReleaseValidation/python/ConfigBuilder.py,v 
# with command line options: Configuration/GenProduction/python/EightTeV/Hadronizer_TuneZ2star_8TeV_generic_LHE_pythia_cff.py --step GEN --conditions START53_V7C::All --eventcontent RAWSIM --datatier GEN-SIM --filein file:unweighted_events.lhe --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('GEN')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.Generator_cff')
process.load('IOMC.EventVertexGenerators.VtxSmearedRealistic8TeVCollision_cfi')
process.load('GeneratorInterface.Core.genFilterSummary_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

process.load("FWCore.MessageService.MessageLogger_cfi")
# reduce verbosity
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(100)


# Input source
process.source = cms.Source("LHESource",
#  fileNames = cms.untracked.vstring('file:UNW.lhe')
  fileNames = cms.untracked.vstring('/store/user/myalvac/top/cmsgrid_final.lhe')
#   fileNames = cms.untracked.vstring('file:MeBu.lhe')
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.1 $'),
    annotation = cms.untracked.string('runs Z2* Pythia6'),
    name = cms.untracked.string('$Source: /afs/cern.ch/user/c/cvscmssw/public/CMSSW/CMSSW/Configuration/GenProduction/python/EightTeV/Hadronizer_TuneZ2star_8TeV_generic_LHE_pythia_cff.py,v $')
)

# Output definition

process.RAWSIMoutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    outputCommands = process.RAWSIMEventContent.outputCommands,
    fileName = cms.untracked.string('Hadronizer_TuneZ2star_8TeV_generic_LHE_pythia_cff_py_GEN.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM')
    ),
    SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('generation_step')
    )
)

# Additional output definition

# Other statements
process.genstepfilter.triggerConditions=cms.vstring("generation_step")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'START53_V7C::All', '')

process.generator = cms.EDFilter("Pythia6HadronizerFilter",
    pythiaHepMCVerbosity = cms.untracked.bool(True),
    maxEventsToPrint = cms.untracked.int32(0),
    pythiaPylistVerbosity = cms.untracked.int32(1),
    comEnergy = cms.double(13000.0),
    PythiaParameters = cms.PSet(
        pythiaUESettings = cms.vstring('MSTU(21)=1     ! Check on possible errors during program execution', 
            'MSTJ(22)=2     ! Decay those unstable particles', 
            'PARJ(71)=10 .  ! for which ctau  10 mm', 
            'MSTP(33)=0     ! no K factors in hard cross sections', 
            'MSTP(2)=1      ! which order running alphaS', 
            'MSTP(51)=10042 ! structure function chosen (external PDF CTEQ6L1)', 
            'MSTP(52)=2     ! work with LHAPDF', 
            'PARP(82)=1.921 ! pt cutoff for multiparton interactions', 
            'PARP(89)=1800. ! sqrts for which PARP82 is set', 
            'PARP(90)=0.227 ! Multiple interactions: rescaling power', 
            'MSTP(95)=6     ! CR (color reconnection parameters)', 
            'PARP(77)=1.016 ! CR', 
            'PARP(78)=0.538 ! CR', 
            'PARP(80)=0.1   ! Prob. colored parton from BBR', 
            'PARP(83)=0.356 ! Multiple interactions: matter distribution parameter', 
            'PARP(84)=0.651 ! Multiple interactions: matter distribution parameter', 
            'PARP(62)=1.025 ! ISR cutoff', 
            'MSTP(91)=1     ! Gaussian primordial kT', 
            'PARP(93)=10.0  ! primordial kT-max', 
            'MSTP(81)=21    ! multiple parton interactions 1 is Pythia default', 
            'MSTP(82)=4     ! Defines the multi-parton model'),
        processParameters = cms.vstring('MSEL=0          ! User defined processes', 
            'PMAS(5,1)=4.4   ! b quark mass', 
            'PMAS(6,1)=172.4 ! t quark mass'),
        parameterSets = cms.vstring('pythiaUESettings', 
            'processParameters')
    )
)


# Path and EndPath definitions
process.generation_step = cms.Path(process.pgen)
process.genfiltersummary_step = cms.EndPath(process.genFilterSummary)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.RAWSIMoutput_step = cms.EndPath(process.RAWSIMoutput)
#process.analyzeBasicPat = cms.EDAnalyzer("PatBasicAnalyzer",
#  electronSrc = cms.untracked.InputTag("selectedPatElectronsPFlow")
#)
process.tupel = cms.EDAnalyzer("Tupel",
#  trigger      = cms.InputTag( "patTrigger" ),
  triggerEvent = cms.InputTag( "patTriggerEvent" ),
  #triggerSummaryLabel = cms.InputTag("hltTriggerSummaryAOD","","HLT"), 
  photonSrc   = cms.untracked.InputTag("slimmedPhotons"),
  electronSrc = cms.untracked.InputTag("slimmedElectrons"),
  muonSrc     = cms.untracked.InputTag("slimmedMuons"),
  #tauSrc      = cms.untracked.InputTag("slimmedPatTaus"),
  jetSrc      = cms.untracked.InputTag("slimmedJets"),
  metSrc      = cms.untracked.InputTag("patMETsPF"),
  genSrc      = cms.untracked.InputTag("genParticles"),
  gjetSrc       = cms.untracked.InputTag('ak5GenJets'),
  muonMatch    = cms.string( 'muonTriggerMatchHLTMuons' ),
  muonMatch2    = cms.string( 'muonTriggerMatchHLTMuons2' ),
  elecMatch    = cms.string( 'elecTriggerMatchHLTElecs' ),
  mSrcRho      = cms.untracked.InputTag('fixedGridRhoFastjetAll'),#arbitrary rho now
  CalojetLabel = cms.untracked.InputTag('slimmedJets'), #same collection now BB 
  metSource = cms.VInputTag("slimmedMETs","slimmedMETs","slimmedMETs","slimmedMETs"), #no MET corr yet
  lheSource=cms.untracked.InputTag('source')
)

#process.genParticles.src = cms.InputTag("source");
process.genParticlesForJets.excludeFromResonancePids = cms.vuint32(12, 14, 16)
process.genParticlesForJets.ignoreParticleIDs = cms.vuint32(1000022, 1000012, 1000014, 1000016, 2000012,
    2000014, 2000016, 1000039, 5100039, 4000012,
    4000014, 4000016, 9900012, 9900014, 9900016,
    39, 12, 14, 16)


process.analysis_step= cms.Path(process.tupel)
process.TFileService = cms.Service("TFileService",
				     fileName = cms.string('MeBu_madgraph.root')
                                    #fileName = cms.string('/tmp/myalvac/MeBu.root' )
                                    )

# Schedule definition
#process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.analysis_step,process.endjob_step,process.RAWSIMoutput_step)
process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.analysis_step,process.endjob_step)
#process.schedule = cms.Schedule(process.generation_step,process.genfiltersummary_step,process.endjob_step)
# filter all path with the production filter sequence
for path in process.paths:
	getattr(process,path)._seq = process.generator * getattr(process,path)._seq 

