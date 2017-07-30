from WMCore.Configuration import Configuration
config = Configuration()
config.section_('General')
config.General.transferOutputs = True
config.General.transferLogs = True
config.General.requestName = 'ZJetsAna_2_07_27_2017'
config.section_('JobType')
#config.JobType.outputFiles = [ 'HistoFiles.tgz' , 
#                               'analysis.log', 
#                               'LetterFractions.txt', 
#                               'mcYieldScale.txt']
config.JobType.outputFiles = [ 'HistoFiles.tgz' , 
                               'analysis.log'] 
                               

# Specify here the runZjets_newformat configuration file.
# Note: the job (job_crab.sh) will substitute the string 'lepSel' 
# DMu or DE
config.JobType.scriptArgs = ['cfg=vjets_lepSel_crab.cfg']
config.JobType.scriptArgs = ['maxEvents=100']
config.JobType.scriptExe = 'job_crab.sh'
config.JobType.pluginName = 'PrivateMC'
config.JobType.psetName = 'donothing_cfg.py'

#Specify here the path to the runZJets_newformat binary, RooUnfold library and RooUnfold .pcm files.
config.JobType.inputFiles = [ '../Main/runZJets_newformat', '../RooUnfold/libRooUnfold.so', '../RooUnfold/RooUnfoldDict_rdict.pcm', 'LetterFractions.txt', 'mcYieldScale.txt']

#Tarball with the efficiency tables. Can be created with the command tar -cxf EfficiencyTables.tgz EfficiencyTables to be run in DYJets direcrory
config.JobType.inputFiles  += ['../EfficiencyTables.tgz', 'unfolding.cfg' ]

#Specify here the ratio histograms to be used for unfolding uncertainties
#config.JobType.inputFiles += [ '../Ratios/ZNGoodJets_Zexc_ratio.root', '../Ratios/SecondJetEta_2_Zinc2jet_ratio.root', '../Ratios/FirstJetPt_2_Zinc1jet_ratio.root', '../Ratios/ThirdJetPt_2_Zinc3jet_ratio.root',  '../Ratios/JetsHT_2_Zinc3jet_ratio.root', '../Ratios/FirstJetEta_2_Zinc1jet_ratio.root', '../Ratios/ThirdJetEta_2_Zinc3jet_ratio.root', '../Ratios/JetsHT_2_Zinc2jet_ratio.root', '../Ratios/SecondJetPt_2_Zinc2jet_ratio.root', '../Ratios/JetsHT_2_Zinc1jet_ratio.root' ]

#Specify here the configuration files
#config.JobType.inputFiles += ['vjets_DMu_crab.cfg', 'vjets_DE_crab.cfg']
config.JobType.inputFiles += ['vjets_DMu_crab.cfg']

config.section_('Data')
config.Data.unitsPerJob = 1
config.Data.totalUnits = 2
config.Data.publication = False
config.Data.splitting = 'EventBased'
config.Data.outLFNDirBase = '/store/group/phys_smp/darcaro'
#config.Data.outLFNDirBase = TO_BE_FILLED
config.section_('User')
config.section_('Site')
config.Site.whitelist = ['T2_CH_CERN']
config.Site.storageSite = 'T2_CH_CERN'
