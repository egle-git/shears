# -- check whether all jobs are finished by checking whether all log files has the "[INFO ] Done writing output" line
# -- if not, print the resubmit commands for unfinished jobs

# -- usage:
# -- python3 find_unfinishedJob.py -d <directory with log file>
# -- e.g. python3 find_unfinishedJob.py -d default_ee_18-2024-11-29_06h15/logs

import os, sys
import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--dir", "-d", required=True, help="log directory")

args = parser.parse_args()
print("log directory = %s"  % args.dir)

list_jobNum = []
for fileName in os.listdir(args.dir):
  if "con" in fileName: continue

  isCompleted = False
  f_log = open("%s/%s" % (args.dir, fileName))
  lineToCheck = f_log.readlines()[-2]

  if "[INFO ] Done writing output" in lineToCheck:
    isCompleted = True

  if not isCompleted:
    jobNum = fileName.split("log-")[-1].split(".txt")[0]
    print("%s (jobNum = %s): line = %s" % (fileName, jobNum, lineToCheck.split("\n")[0]))
    list_jobNum.append( jobNum )

if len(list_jobNum) == 0:
  print("\n--> no incompleted jobs are found: ready to be hadd")
  dirName = args.dir.split("/logs")[0]
  cmd_hadd = "./auto-hadd.sh %s\n" % dirName
  print(cmd_hadd)
  sys.exit()

print("\nResubmit the unfinished jobs: ")

for jobNum in list_jobNum:
  scriptDir = args.dir.replace("/logs", "/scripts")
  scriptName = "%s/%s.sub" % (scriptDir, jobNum)

  cmd = "condor_submit %s" % scriptName
  print(cmd)

print("=============================")


