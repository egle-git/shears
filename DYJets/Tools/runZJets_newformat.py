#!/usr/bin/env python
# -*- mode: py -*-
#This script will allow for multiple threads to start for each job of runZJets. 
# 08_14_2017 - djarcaro

#done
import os
import subprocess
import sys
import argparse
import time
import imp
runZJetsHelper = imp.load_source('runZJetsHelper', '/data/djarcaro/CMSSW_8_0_25/src/shears/DYJets/Tools/runZJetsHelper.py')


def main(arguments):
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('-o', '--outputDir', help='Output directory for rootfiles', 
                        type=str, required=True)
    parser.add_argument('-m', '--maxEvents', help='Number of Events to run over', type=int, required=True)
    parser.add_argument('-t','--threads', help='Number of threads', type=int, required=True)
    parser.add_argument('-l', '--log', help='Log file number', type=int, required=True)
    
    args = parser.parse_args(arguments)

    debug=False
    testRun = False
    #SAMPLES = ["DATA"]
    SAMPLES = ["DATA","DYJETS","BACKGROUND"]
    NSYST = 0
    runZCommand = "/data/djarcaro/CMSSW_8_0_25/src/shears/DYJets/Main/runZJets_newformat"
    processes = []
    startTime = time.time()
    date=time.strftime("%m_%d_%y",time.localtime())

    for iSample in range(0,len(SAMPLES)):
        if(SAMPLES[iSample]=="DATA"):
            NSYST=3
        if(SAMPLES[iSample]=="DYJETS"):
            NSYST=9
        if(SAMPLES[iSample]=="BACKGROUND"):
            NSYST=7
        if(testRun):
            NSYST=1
        
    
        for iSyst in range(0,NSYST):

            #Main loop for starting the processes for each job
            for iJob in range(1,args.threads+1):
                command = []
                print "Trying job = %d" % iJob
                outputFileName = "ZJets_%s_%d_Syst%d_JOB%d_%s.log" % (SAMPLES[iSample],
                                                                      args.log,iSyst,iJob,date)
                print outputFileName
                outputFile = open(outputFileName,"w")
                print "runZJets_newformat histoDir=%s doWhat=%s whichSyst=%d nJobs=%d jobNum=%d" %(args.outputDir, SAMPLES[iSample], iSyst, args.threads, iJob)
                
                command.append(runZCommand)
                command.append("histoDir=%s" % args.outputDir)
                command.append("doWhat=%s" % SAMPLES[iSample])
                command.append("whichSyst=%d" % iSyst)
                command.append("nJobs=%d" % args.threads)
                command.append("jobNum=%d" % iJob)
                command.append("maxEvents=%d" % args.maxEvents)
        
                if debug: print command
                processes.append(subprocess.Popen(command,stdout=outputFile,stderr=outputFile))
        

            exitCodes = [p.wait() for p in processes]
            print exitCodes

            if (SAMPLES[iSample] == "DATA") and (iSyst == NSYST-1):
                print "Combining letter fraction for Data"
                runZJetsHelper.CombineLetters()
    

        runZJetsHelper.MoveLogs("ZJets_%s_%d_" % (SAMPLES[iSample],args.log),args.outputDir)
       

    print "Running hadd to combine histos"
    runZJetsHelper.CombineHistos(args.outputDir)
    #print "ZJets_%s_%d_" % (SAMPLES[iSample],args.log)
    #print args.outputDir
                            



    timerFile = open("StopWatch.log","a")
    endTime = time.time()
    totalTime = endTime - startTime
    timerFile.write("%s   threads=%d   time(m)=%d\n" % (date,args.threads,totalTime/60))
    timerFile.close()

if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))






