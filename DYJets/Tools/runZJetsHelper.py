#!/usr/bin/env python

#This script combines the histos from the multithreading job
import os
import subprocess
import sys

def CombineHistos(histoDir):
    testMode = False #Set this and it will just print commands and not call them

    #Example of file name
    #DMu_13TeV_DYJets_UNFOLDING_Syst_1_Down_1.root

    HistoBasetest = ["DMu_13TeV_Data_"]

    HistoBase = ["DMu_13TeV_DYJets_UNFOLDING_",
                 "DMu_13TeV_Data_",
                 "DMu_13TeV_ST_sch_",
                 "DMu_13TeV_ST_tW_",
                 "DMu_13TeV_ST_tch_",
                 "DMu_13TeV_STbar_tW_",
                 "DMu_13TeV_TT_",
                 "DMu_13TeV_Top_",
                 "DMu_13TeV_VV_",
                 "DMu_13TeV_WToLNu_",
                 "DMu_13TeV_WWTo2L2Nu_",
                 "DMu_13TeV_WZ_",
                 "DMu_13TeV_ZZ_"]
    HistoDirection = ["",
                      "Down_",
                      "Up_"]
    #HistoEnd = "JetPtMin_30_JetEtaMax_24" //No longer needed after simplifying names 12_14_2017 DJALOG
    
    ls = subprocess.check_output(['ls',histoDir]).splitlines()

    HistoName = ""
    tmpHisto1 = ""
    tmpHisto2 = ""
    haddTarget = ""
    haddHistos = []
    #Example of file name
    #DMu_13TeV_DYJets_UNFOLDING_Syst_1_Down_1.root
    #for iHistoBase in HistoBasetest:
    for iHistoBase in HistoBase:
        HistoName = ""
        haddTarget = ""
        haddHistos = []
        HistoName += iHistoBase
        print HistoName
        for iHistoSyst in range(0,10):
            if(iHistoSyst == 0):
                tmpHisto1 = HistoName
            HistoName = tmpHisto1
            print "\nHistoName = tmpHisto1: %s\n" % HistoName
            HistoName += "Syst_%d_" % iHistoSyst
            print "HistoName += Syst_d_: %s\n" % HistoName
            for iHistoDirection in HistoDirection:
                if(iHistoDirection == HistoDirection[0]):
                    tmpHisto2 = HistoName
                HistoName = tmpHisto2

                if(iHistoSyst != 0 and iHistoDirection == HistoDirection[0]):
                    HistoName += "dummy"

                HistoName += iHistoDirection
                print "HistoName += iHistoDirection: %s\n" % HistoName
                #HistoName += HistoEnd
                haddHistos = []
                haddTarget = histoDir+"/"+HistoName[0:len(HistoName)-1]+".root"
                print "haddTarget = %s" % haddTarget

                for i in range(0,len(ls)):
                    if (ls[i].find(HistoName) != -1) and (ls[i].find("Max_24.root") == -1):
                        print "found: %s" % ls[i]
                        haddHistos.append(histoDir+"/"+ls[i])

                if len(haddHistos) > 0:
                    haddCommand=["hadd","-f"]
                    haddCommand.append(haddTarget)
                    haddCommand += haddHistos
                    if(not testMode): subprocess.call(haddCommand);
                    removeCommand = ["rm"] + haddHistos
                    if(not testMode): subprocess.call(removeCommand)
                    #print "\n"
                    print haddCommand
                    #print removeCommand
                    #print "\n"




def CombineLetters():

    ls = subprocess.check_output('ls').splitlines()
    nEvents=[0.0]*10
    Total=0.0
    nFiles=0
    letters=[]
    for i in range(0,len(ls)):
        if ls[i].find("LetterFractions_") == 0:
            #Need to read the files and combine the nEvents
            fileLetterIn = open(ls[i],"r")
            lines = fileLetterIn.readlines()
            for iline in range(0,len(lines)):
                if lines[iline].find("Total=") == 0:
                    Total=float(lines[iline][lines[iline].find("=")+1:len(lines[iline])])
                else:
                    if nFiles == 0:
                        letters.append(lines[iline][0])
                    nEvents[iline]+=float(lines[iline][lines[iline].find(";")+1:len(lines[iline])])
            nFiles+=1
            subprocess.call(['rm', ls[i]])

    fileLetterOut = open("LetterFractions.txt","w")
    for i in range(0,len(letters)):
    #print letters[i]
        print "%s;%F" % (letters[i],nEvents[i]/Total)
        fileLetterOut.write("%s;%F\n" % (letters[i],nEvents[i]/Total))
    print Total
    fileLetterOut.write("Total=%d" % Total)


def MoveLogs(logFileBase,histoDir):
    ls = subprocess.check_output('ls').splitlines()
    logDir = histoDir+"/logs"
    subprocess.call(['mkdir',logDir])
    for i in range(0,len(ls)):
        if ls[i].find(logFileBase) == 0:
            #Need to read the files and combine the nEvents
            #print ls[i]
            subprocess.call(['mv',ls[i],logDir])
    

#Place to test
def main(arguments):
    CombineHistos(arguments[0])



if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
