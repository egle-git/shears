#!/usr/bin/env python

#This script combines the histos from the multithreading job
import os
import subprocess
HistoNames=["DMu_13TeV_DYJets_UNFOLDING_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_Data_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_ST_sch_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_ST_tW_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_ST_tch_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_STbar_tW_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_TT_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_Top_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_VV_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_WToLNu_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_WWTo2L2Nu_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_WZ_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_",
            "DMu_13TeV_ZZ_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_"
            ]

ls = subprocess.check_output('ls').splitlines()
#for iHisto in range(0,1):
for iHisto in range(0,len(HistoNames)):
    haddHistos=[]
    haddTarget=[]
    haddCommand=["hadd"]
    haddTarget=HistoNames[iHisto][0:len(HistoNames[iHisto])-1]+".root"
    for i in range(0,len(ls)):
        #print ls[i]
        if ls[i].find(HistoNames[iHisto]) == 0:
            haddHistos.append(ls[i])

    haddCommand.append(haddTarget)
    haddCommand += haddHistos
    #for j in range(0,len(haddCommand)):
    #    print haddCommand[j]

#    print haddCommand
    subprocess.call(haddCommand);
    removeCommand = ["rm"] + haddHistos
    #for k in range(0,len(removeCommand)):
    #    print removeCommand[k]
    subprocess.call(removeCommand)
    print "\nNext one\n"


#






    #fileLetterIn = open(ls[i],"r")
    #    lines = fileLetterIn.readlines()
     #   for iline in range(0,len(lines)):
     #       #print lines[iline].rstrip()
     #       if lines[iline].find("Total=") == 0:
     #           Total=float(lines[iline][lines[iline].find("=")+1:len(lines[iline])])
     #       else:
     #           if nFiles == 0:
     #               letters.append(lines[iline][0])
     #           nEvents[iline]+=float(lines[iline][lines[iline].find(";")+1:len(lines[iline])])
     #   nFiles+=1
     #   subprocess.call(['rm', ls[i]])

#fileLetterOut = open("LetterFractions.txt","w")
#for i in range(0,len(letters)):
#    #print letters[i]
#    print "%s;%F" % (letters[i],nEvents[i]/Total)
#    fileLetterOut.write("%s;%F\n" % (letters[i],nEvents[i]/Total))
#print Total
#fileLetterOut.write("Total=%d" % Total)
