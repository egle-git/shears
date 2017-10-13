
  TH1::SetDefaultSumw2();

  TH1F* h_nvtx = new TH1F("nvtx","nvtx",40,0,40);

  TH1F* h_nvtx_noWeight = new TH1F("nvtx_noWeight","nvtx_noWeight",40,0,40);
  
  TH1F* h_Elecpt = new TH1F("Elecpt","Elecpt",200,0,1000);

  TH1F*h_Eleceta   = new TH1F("Eleceta","eleceta",50,-5,5);
  TH1F*h_Elecphi   = new TH1F("Elecphi","elecphi",50,-5,5);

  TH1F* h_MZElec = new TH1F("MZElec","M(Z)#rightarrow #mu#mu",40, 70.,110.);
  TH1F* h_diElecpt = new TH1F("diElecpt","diElecpt",200,0,1000);
  TH1F* h_diElecrapidity = new TH1F("diElecrapidity","diElecrapidity",50,-2.5,2.5);
  TH1F* h_diElecphi = new TH1F("diElecphi","diElecphi",12,-3.2,3.2);
  TH1F*h_deltar = new TH1F("deltar","deltar",200,0,5);
  double n_jet_range[10]={-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5};
  TH1F* h_n_jet_30_unfold  = new TH1F("n_jet_30_unfold","n_jet_30_unfold",9,n_jet_range);
