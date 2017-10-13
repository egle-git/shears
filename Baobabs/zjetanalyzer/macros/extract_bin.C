{

TFile *f= TFile::Open("../MD_DYJetstoLL_isMu_0_doUnf_1_isSS_0_jetPt_30_Tightjet_0_JES_0_JER_0_PU_0_ZPTweightcorr_0_24_04.root");


  TString var;
  char nam[100];
     ofstream myfile2;
      myfile2.open ("Table_MC_jet_PT.tex");
      myfile2.precision(4);

      for(int i=0 ; i<7;i++){
	myfile2<<"range"<<i<<endl;
      var= "inc_ljet_pt_y_fixbin_";
      var+=(i);

     TH1D *mc=(TH1D*)(f->Get(var));


 
	//myfile2<<"Njet & $d \\sigma / d(NJet)$ & stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
	for(int binn=1;binn<=mc->GetNbinsX();binn++){
	  myfile2<<(mc->GetBinCenter(binn)-((mc->GetBinWidth(binn))/2))
                <<"-"
                <<(mc->GetBinCenter(binn)+((mc->GetBinWidth(binn))/2))

                << "  & "<<mc->GetBinContent(binn)*3531.5*19700/30459425. <<"\\\\"<<endl;
        }
	myfile2<<"\\hline"<<endl;
	myfile2<<"\\end{tabular}}" <<endl;
	myfile2<<"\\label{tab:njetunf}" <<endl;
	myfile2<<"\\end{center}" <<endl;
	myfile2<<"\\end{sidewaystable}" <<endl;

}
}
