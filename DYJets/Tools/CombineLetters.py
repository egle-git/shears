#!/usr/bin/env python

#This script combines the LetterFraction files into one.

import os
import subprocess
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
            #print lines[iline].rstrip()
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

