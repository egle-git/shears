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
    parser.add_argument('-t','--threads', help='Number of threads', type=int, required=True)
    parser.add_argument('-l', '--log', help='Log file number', type=int, required=True)
    args = parser.parse_args(arguments)

    debug=False
    SAMPLE = "DATA"
    runZCommand = "/data/djarcaro/CMSSW_8_0_25/src/shears/DYJets/Main/runZJets_newformat"
    processes = []
    date=time.strftime("%m_%d_%y",time.localtime())

    #Main loop for starting the processes for each job
    for iJob in range(1,args.threads+1):
        command = []
        print "Trying job = %d" % iJob
        outputFileName = "ZJets_%s_%d_JOB%d_%s.log" % (SAMPLE,args.log,iJob,date)
        print outputFileName
        outputFile = open(outputFileName,"w")
        print "runZJets_newformat histoDir=%s doWhat=%s nJobs=%d jobNum=%d" % (args.outputDir,
                                                                               SAMPLE,
                                                                               args.threads,
                                                                               iJob)
                                                                   
        command.append(runZCommand)
        command.append("histoDir=%s" % args.outputDir)
        command.append("doWhat=%s" % SAMPLE)
        command.append("nJobs=%d" % args.threads)
        command.append("jobNum=%d" % iJob)
        
        if debug: print command
        processes.append(subprocess.Popen(command,stdout=outputFile,stderr=outputFile))
        

    exitCodes = [p.wait() for p in processes]
    print exitCodes

    #Do anything here that needs to be done once all processes (jobs) are finished
    print "Running hadd to combine histos"
    runZJetsHelper.CombineHistos(args.outputDir)
    if SAMPLE == "DATA":
        print "Combining letter fraction for Data"
        runZJetsHelper.CombineLetters()
    runZJetsHelper.MoveLogs("ZJets_%s_%d_" % (SAMPLE,args.log),args.outputDir)


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))






