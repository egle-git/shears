from ROOT import TH2D, TFile

# -- convert ROOT histogram to .txt file
class Converter:
  def __init__(self, inputName, histName, outputName):
    self.inputName = inputName
    self.histName = histName
    self.outputName = outputName

  def Convert(self):
    f_input = TFile(self.inputName)
    h2D_sf = f_input.Get(self.histName)
    # f_input.Close()

    f_output = open(self.outputName, "w")

    nEtaBin = h2D_sf.GetNbinsX()
    nPtBin  = h2D_sf.GetNbinsY()

    for i_eta in range(0, nEtaBin):
      i_etaBin = i_eta+1

      etaMin = h2D_sf.GetXaxis().GetBinLowEdge(i_etaBin)
      etaMax = h2D_sf.GetXaxis().GetBinLowEdge(i_etaBin+1)

      for i_pt in range(0, nPtBin):
        i_ptBin = i_pt+1

        ptMin = h2D_sf.GetYaxis().GetBinLowEdge(i_ptBin)
        ptMax = h2D_sf.GetYaxis().GetBinLowEdge(i_ptBin+1)

        value = h2D_sf.GetBinContent(i_etaBin, i_ptBin)
        error = h2D_sf.GetBinError(i_etaBin, i_ptBin)

        line = "%f\t%f\t%f\t%f\t%f\t%f\t%f" % (etaMin, etaMax, ptMin, ptMax, value, error, error)
        f_output.write(line+"\n")

    f_output.close()

    print( "[%s/%s --> %s]\n" % (self.inputName, self.histName, self.outputName) )


class MultiConverter:
  def __init__(self):
    self.list_converter = []

  def Register(self, inputName, histName, outputName):
    converter = Converter(inputName, histName, outputName)
    self.list_converter.append(converter)

  def Convert(self):
    for converter in self.list_converter:
      converter.Convert()


if __name__ == "__main__":
  converter = MultiConverter()
  converter.Register("16pre/eff_el_RECO_16pre_custom.root", "sf", "16pre/eff_el_RECO_16pre_custom.txt")
  converter.Register("16pre/eff_el_ID_16pre_custom.root",   "sf", "16pre/eff_el_ID_16pre_custom.txt")
  converter.Register("16pre/2016a_Ele12Leg2_MediumID_Plus.root",  "sf", "16pre/2016a_Ele12Leg2_MediumID_Plus.txt")
  converter.Register("16pre/2016a_Ele12Leg2_MediumID_Minus.root", "sf", "16pre/2016a_Ele12Leg2_MediumID_Minus.txt")
  converter.Register("16pre/2016a_Ele23Leg1_MediumID_Plus.root",  "sf", "16pre/2016a_Ele23Leg1_MediumID_Plus.txt")
  converter.Register("16pre/2016a_Ele23Leg1_MediumID_Minus.root", "sf", "16pre/2016a_Ele23Leg1_MediumID_Minus.txt")
  converter.Register("16pre/2016a_DZ.root", "sf", "16pre/2016a_DZ.txt")

  converter.Register("16post/eff_el_RECO_16post_custom.root", "sf", "16post/eff_el_RECO_16post_custom.txt")
  converter.Register("16post/eff_el_ID_16post_custom.root",   "sf", "16post/eff_el_ID_16post_custom.txt")
  converter.Register("16post/2016b_Ele12Leg2_MediumID_Plus.root",  "sf", "16post/2016b_Ele12Leg2_MediumID_Plus.txt")
  converter.Register("16post/2016b_Ele12Leg2_MediumID_Minus.root", "sf", "16post/2016b_Ele12Leg2_MediumID_Minus.txt")
  converter.Register("16post/2016b_Ele23Leg1_MediumID_Plus.root",  "sf", "16post/2016b_Ele23Leg1_MediumID_Plus.txt")
  converter.Register("16post/2016b_Ele23Leg1_MediumID_Minus.root", "sf", "16post/2016b_Ele23Leg1_MediumID_Minus.txt")
  converter.Register("16post/2016b_DZ.root", "sf", "16post/2016b_DZ.txt")

  converter.Register("17/eff_el_RECO_17_custom.root", "sf", "17/eff_el_RECO_17_custom.txt")
  converter.Register("17/eff_el_ID_17_custom.root",   "sf", "17/eff_el_ID_17_custom.txt")
  converter.Register("17/2017_Ele12Leg2_MediumID_Plus.root",  "sf", "17/2017_Ele12Leg2_MediumID_Plus.txt")
  converter.Register("17/2017_Ele12Leg2_MediumID_Minus.root", "sf", "17/2017_Ele12Leg2_MediumID_Minus.txt")
  converter.Register("17/2017_Ele23Leg1_MediumID_Plus.root",  "sf", "17/2017_Ele23Leg1_MediumID_Plus.txt")
  converter.Register("17/2017_Ele23Leg1_MediumID_Minus.root", "sf", "17/2017_Ele23Leg1_MediumID_Minus.txt")

  converter.Register("18/eff_el_RECO_18_custom.root", "sf", "18/eff_el_RECO_18_custom.txt")
  converter.Register("18/eff_el_ID_18_custom.root",   "sf", "18/eff_el_ID_18_custom.txt")
  converter.Register("18/2018_Ele12Leg2_MediumID_Plus.root",  "sf", "18/2018_Ele12Leg2_MediumID_Plus.txt")
  converter.Register("18/2018_Ele12Leg2_MediumID_Minus.root", "sf", "18/2018_Ele12Leg2_MediumID_Minus.txt")
  converter.Register("18/2018_Ele23Leg1_MediumID_Plus.root",  "sf", "18/2018_Ele23Leg1_MediumID_Plus.txt")
  converter.Register("18/2018_Ele23Leg1_MediumID_Minus.root", "sf", "18/2018_Ele23Leg1_MediumID_Minus.txt")

  converter.Convert()
