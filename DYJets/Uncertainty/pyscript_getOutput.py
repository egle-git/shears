import argparse
import os
import time
import sys

# -- usage:
# -- python pyscript_getOutput.py -m collect -o <output directory name (default: result_date)>
# -- python pyscript_getOutput.py -m distribute -i <input directory name>

# -- (Directory, list of root files) combination
# -- should be updated accordingly if more results are made
dic_result = {
  "Acceptance" : {"DYAcceptance.root", "TheoryPrediction_VariousPDF.root", "TheoryPrediction_VariousPDF_aboveM200.root"},

  "Stat" : {"Uncertainty_and_Covariance_Stat_ee.root", 
            "Uncertainty_and_Covariance_Stat_mm.root" },

  "Background" : {"Unfolded_And_Uncertainty_Bkg_ee.root", 
                  "Unfolded_And_Uncertainty_Bkg_mm.root" },

  "Pileup" : {"Unfolded_And_Uncertainty_pileup_ee.root", 
              "Unfolded_And_Uncertainty_pileup_mm.root" },

  "L1Prefiring" : {"Unfolded_And_Uncertainty_L1Pref_ee.root", 
                   "Unfolded_And_Uncertainty_L1Pref_mm.root" },

  "Luminosity" : {"Unfolded_And_Uncertainty_lumi_ee.root", 
                  "Unfolded_And_Uncertainty_lumi_mm.root" },

  "EffSF" : {"Unfolded_SystVar_EffSF_ee.root", "UncAndCov_EffSF_ee.root", 
             "Unfolded_SystVar_EffSF_mm.root", "UncAndCov_EffSF_mm.root" },

  "Theory" : {"Unfolded_SystVar_Theory_ee.root", "Unfolded_SystVar_Theory_ee_FPS.root",
              "Unfolded_SystVar_Theory_mm.root", "Unfolded_SystVar_Theory_mm_FPS.root" },
  
  "elE": {"Unfolded_SystVar_elE.root", "UncAndCov_elE.root" },

  "muP" : {"Unfolded_SystVar_muP.root", "UncAndCov_muP.root" },

  "Unfolding" : {"UncAndCov_Unfolding_ee.root",
                 "UncAndCov_Unfolding_mm.root"},

  "Summary" : {"UncAndCov_All_ee.root", "UncAndCov_All_ee_FPS.root",
               "UncAndCov_All_mm.root", "UncAndCov_All_mm_FPS.root"},

  "Combination": {"CombinedResult_DYXSec_fid.root", "CombinedResult_DYXSec_FPS.root"},
}

if __name__ == '__main__':
  parser = argparse.ArgumentParser()

  parser.add_argument('-m', '--mode',
                      type = str,
                      help = 'collect or distribute')

  parser.add_argument('-o', '--output',
                      type = str,
                      default = "result",
                      help = 'output directory with the collected root files (only when the mode is "collect")')

  parser.add_argument('-i', '--input',
                      type = str,
                      default = "",
                      help = 'input directory with all root files (only when the mode is "distribute")')

  args = parser.parse_args()

  print("--- input argument ---")
  print("mode = %s" % args.mode)
  if args.mode == "collect" :
    print("output = %s" % args.output)
  elif args.mode == "distribute" :
    print("input = %s" % args.input)
    if args.input == "":
      print("input directory should be provided")
      sys.exit()
  else:
    print('mode should be "collect" or "distribute"')
    sys.exit()
  print("----------------------\n")


  if args.output == "result":
    timeInfo = time.localtime()
    yearInfo = "%d" % timeInfo.tm_year
    yearInfo = yearInfo.split("20")[-1]
    dateTag = "%s%d%d" % (yearInfo, timeInfo.tm_mon, timeInfo.tm_mday)

    args.output = args.output + "_" + dateTag

  if args.mode == "collect":
    if os.path.exists(args.output):
      print("directory = %s already exists" % args.output)
      sys.exit()

    os.makedirs(args.output)

    for key in dic_result.keys():
      dirName = key
      list_rootFile = dic_result[key]

      for rootFile in list_rootFile:
        filePath = "%s/%s" % (dirName, rootFile)
        if not os.path.exists(filePath):
          print("File = %s does not exist!" % filePath)
          sys.exit()

        if os.path.exists("%s/%s" % (args.output, rootFile)):
          print("ROOT file = %s already exists in the output directory = %s" % (rootFile, args.output))
          sys.exit()

        cmd_cp = "cp %s %s" % (filePath, args.output)
        # print(cmd_cp)
        os.system(cmd_cp)

    print("All root files are saved in %s" % args.output)


  if args.mode == "distribute":
    if not os.path.exists(args.input):
      print("Input directory = %s does not exist" % args.input)
      sys.exit()

    for key in dic_result.keys():
      dirName = key
      list_rootFile = dic_result[key]

      for rootFile in list_rootFile:
        filePath = "%s/%s" % (args.input, rootFile)
        if not os.path.exists(filePath):
          print("File = %s does not exist in the input directory!", filePath)
          sys.exit()

        if os.path.exists( "%s/%s" % (dirName, rootFile) ):
          print("File = %s already exists in %s!" % (rootFile, dirName))
          sys.exit()

        cmd_cp = "cp %s %s" % (filePath, dirName)
        # print(cmd_cp)
        os.system(cmd_cp)


    print("All root files in %s are distributed in the current working space" % args.input)







