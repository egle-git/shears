import ROOT
from optparse import OptionParser
import math
from ROOT import TF2, TH2D, TCanvas

def main(options):
    
    directoryID = "MC_NUM_TightID_DEN_genTracks_PAR_pt_eta"
    directoryISO = "LooseISO_TightID_pt_eta"
    directoryTrigger = "IsoMu24_OR_IsoTkMu24_PtEtaBins"
    directoryTracks = ""

    rfile = "/afs/cern.ch/work/d/darcaro/private/CMSSW_8_0_26_patch2/src/shears/DYJets/EfficiencyTables/Eff_SF_ID_BF.root"
    fd = ROOT.TFile(rfile)
    #fd = ROOT.TFile(options.data)
    
    fileKeys = fd.GetListOfKeys()
    for iKey in fileKeys:
        print iKey.GetName()


    print fd.GetName()
    directory = fd.Get(directoryID)
    #print directory.GetName()
    directory.cd()
    keys = directory.GetListOfKeys()
    i = 0
    j = 0
    #for k in keys:
    #print k.GetName()
    ptetaHisto = TH2D()
    ptetaHisto = directory.Get("abseta_pt_ratio")
    print ptetaHisto.GetNbinsX
        #GetNbinsX
        #GetNbinsY
#GetBinContent
#GetBinErrorLow
#GetBinErrorUp




    import time
            
    dataOutput = "Efficiencies_"+options.output+"_data_"+(time.strftime("%m_%d_%Y"))+".txt"
    mcOutput = "Efficiencies_"+options.output+"_mc_"+(time.strftime("%m_%d_%Y"))+".txt"
    SFOutput = "ScaleFactors_"+options.output+"_"+(time.strftime("%m_%d_%Y"))+".txt"

    #outData = open(dataOutput, "w+")

    print "\n DATA"
    for i in range(len(etalow)):
        for j in range(len(ptlow)):
            fitvalsNumEL[j][i] = abs(fitvalsNumEL[j][i])
            fitvalsNumEH[j][i] = abs(fitvalsNumEH[j][i])
            print "%4.1f  %4.1f   %6.4f   %6.4f  %6.4f   %6.4f   %6.4f \n"%(etalow[i], etahigh[i], ptlow[j], pthigh[j], fitvalsNum[j][i], fitvalsNumEL[j][i], fitvalsNumEH[j][i])
            outData.write("%4.1f  %4.1f   %6.4f   %6.4f  %6.4f   %6.4f   %6.4f \n"%(etalow[i], etahigh[i], ptlow[j], pthigh[j], fitvalsNum[j][i], fitvalsNumEL[j][i], fitvalsNumEH[j][i]))

    print "\n MC"
    outMC = open(mcOutput, "w+")
    for i in range(len(etalow)):
        for j in range(len(ptlow)):
            fitvalsDenEL[j][i] = abs(fitvalsDenEL[j][i])
            fitvalsDenEH[j][i] = abs(fitvalsDenEH[j][i])
            print "%4.1f  %4.1f   %6.4f   %6.4f  %6.4f   %6.4f   %6.4f \n"%(etalow[i], etahigh[i], ptlow[j], pthigh[j], fitvalsDen[j][i], fitvalsDenEL[j][i], fitvalsDenEH[j][i])
            outMC.write("%4.1f  %4.1f   %6.4f   %6.4f  %6.4f   %6.4f   %6.4f \n"%(etalow[i], etahigh[i], ptlow[j], pthigh[j], fitvalsDen[j][i], fitvalsDenEL[j][i], fitvalsDenEH[j][i]))
            #out.write("%4.1f  %4.1f   %+6.4f   %+6.4f  %6.4f   %6.4f \n"%(eta0, eta1, pT0, pT1, fitvalsDen[i][j], fitvalsDenE[i][j]))
        
    #out.write("-------------^MC^-------------\n")
    outSF = open(SFOutput, "w+")
    print "\n SF"
    for i in range(len(etalow)):
        for j in range(len(ptlow)):       
            SF = fitvalsNum[j][i]/fitvalsDen[j][i]
            SFEL = SF * math.sqrt(pow(fitvalsNumEL[j][i]/fitvalsNum[j][i],2) + pow(fitvalsDenEL[j][i]/fitvalsDen[j][i],2))
            SFEH = SF * math.sqrt(pow(fitvalsNumEH[j][i]/fitvalsNum[j][i],2) + pow(fitvalsDenEH[j][i]/fitvalsDen[j][i],2))
            print "%4.1f  %4.1f   %4.1f   %4.1f  %6.4f  %6.4f  %6.4f\n"%(etalow[i], etahigh[i], ptlow[j], pthigh[j], SF, SFEL, SFEH)
            outSF.write("%4.1f  %4.1f   %4.1f   %4.1f  %6.4f  %6.4f  %6.4f\n"%(etalow[i], etahigh[i], ptlow[j], pthigh[j], SF, SFEL, SFEH))
        


if __name__ == "__main__":  
    parser = OptionParser()
    parser.add_option("-d", "--data", default="test.root", help="Input filename data")
    parser.add_option("-m", "--mc", default="test.root", help="Input filename mc")
    parser.add_option("-f", "--directory", default="GsfElectronToRECO/Medium", help="Directory with workspace")
    parser.add_option("-o", "--output", default="ID", help="Output filename")

    (options, arg) = parser.parse_args()
     
    main(options)
