# -- print commands to merge & download the shears outputs
# -- to make the script understand the shears output,
# -- the output directory name in shears should contain the string "%s_%s" % (channel, era)
# -- e.g. "ee_16pre", "mm_18"
# -- example
# python3 ShearsOutputDownloader.py \
# -u kplee \
# -s /user/kplee/Analysis/shears/240419_update_bVeto \
# -o output \
# -c all \
# -t ""

class ShearsOutput:
  def __init__(self):
    self.channel = ""
    self.era = ""
    self.shearsPath = ""
    self.dirName = ""

def SanityCheck(stdout, channel, era):
  if len(stdout) == 0:
    print("No directory is found matching to (channel, era) = (%s, %s)" % (channel, era))
    print("Check the input shears path or whether the directories follow the rule: includes the string (channel_era)")
    sys.exit()

  if len(stdout) > 1:
    print("More than 1 shears output directory for (channel, era) = (%s, %s)" % (channel, era))
    for line in stdout:
        print(line.strip('\n'))
    sys.exit()


def printCMD_hadd(shearsOutput):
  cmd = "./auto-hadd.sh %s" % shearsOutput.dirName
  print(cmd)
  return cmd

def printCMD_download(shearsOutput, outputDir):
  cmd = "scp '%s@mlong.iihe.ac.be:%s/%s/*.root' %s/%s/%s" % (args.user, shearsOutput.shearsPath, shearsOutput.dirName, outputDir, shearsOutput.channel, shearsOutput.era)
  print(cmd)
  return cmd


# -- main part  
import argparse
import paramiko
import os, sys

parser = argparse.ArgumentParser()
parser.add_argument('-u', '--user', default="kplee")
parser.add_argument('-s', '--shears', default="/user/kplee/Analysis/shears/231117_update_latestSetup")
parser.add_argument('-o', '--outputDir', default="./shearsOutput")
parser.add_argument('-c', '--channel', default="all")
parser.add_argument('-t', '--tag', default="")
# parser.add_argument('-v', '--verboseErrors', action='store_true', default=False)
args = parser.parse_args()

if "DYJets" not in args.shears:
  args.shears = args.shears + "/DYJets"

client = paramiko.SSHClient()
host_keys = client.load_system_host_keys()
client.connect('mlong.iihe.ac.be', username=args.user, password='')


list_channel = ["ee", "mm"]
list_era = ["16pre", "16post", "17", "18"]
list_shearsOutput = []

print("# -- find the shears output directory names in T2_BE_IIHE server...")
for channel in list_channel:

  if args.channel != "all" and channel != args.channel:
    continue

  for era in list_era:
    tag_dir = ""
    if args.tag == "": tag_dir = "%s_%s" % (channel, era)
    else:              tag_dir = "%s_%s_%s" % (args.tag, channel, era)
    cmd_find = "find '%s' -maxdepth 1 -type d -name *%s*" % (args.shears, tag_dir)
    # print(cmd_find)
    stdin, stdout, stderr = client.exec_command(cmd_find)

    list_line = []
    for line in stdout:
      list_line.append( line.strip('\n') )

    SanityCheck(list_line, channel, era)

    output = ShearsOutput()
    output.channel = channel
    output.era = era
    output.shearsPath = args.shears
    output.dirName = list_line[0].strip('\n').split("/")[-1]

    print("(channel, era) = (%s, %s) --> dirName = %s" % (output.channel, output.era, output.dirName))
    list_shearsOutput.append(output)

    # for line in stdout:
    #     print(line.strip('\n'))

list_cmd = []

print("\n# -- run at shears working directory:")
print("# -- cd %s" % args.shears)
for shearsOutput in list_shearsOutput:
  cmd = printCMD_hadd(shearsOutput)
  list_cmd.append(cmd)

# -- make output directories
for channel in list_channel:
  if args.channel != "all" and channel != args.channel:
    continue

  for era in list_era:
    dirPath = "%s/%s/%s" % (args.outputDir, channel, era)
    # -- does not exist: make it (recursively)
    if not os.path.exists(dirPath):
      os.makedirs(dirPath)

    list_dir = os.listdir(dirPath)
    # -- if it is not empty: give an error
    if len(list_dir) > 0:
      print("%s is not empty ... please check" % dirPath)
      sys.exit()

print("\n# -- run at local machine")
for shearsOutput in list_shearsOutput:
  cmd = printCMD_download(shearsOutput, args.outputDir)
  list_cmd.append(cmd)

client.close()

# -- write an info file in the output directory
f = open(args.outputDir+"/info.txt", "w")
f.write("shears directory: %s\n\n"% args.shears)
for cmd in list_cmd:
  f.write(cmd+"\n")
f.close()

# find '/user/kplee/Analysis/shears/231117_update_latestSetup/DYJets' -maxdepth 1 -type d -name *ee_16pre* 