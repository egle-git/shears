 /*
  TH1F*   = new TH1F("","",);
  ->Sumw2();
  */
  ///////////////////////////////////////
 int nJetPt_Zinc1jet(11);
  double jetPt_Zinc1jet[12] = {30, 40, 52, 68, 88, 113, 144, 184, 234, 297, 377, 480};
  int nJetPt_Zinc2jet(10);
  double jetPt_Zinc2jet[11] = {30, 40, 52, 68, 88, 113, 144, 184, 234, 297, 377};

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

  TH1F* h_n_jet_30  = new TH1F("n_jet_30","n_jet_30",8,-0.5,7.5);
  TH1F* h_n_jet_30BE_only  = new TH1F("n_jet_30BE_only","n_jet_30BE_only",8,-0.5,7.5);
  TH1F* h_n_jet_30F_only  = new TH1F("n_jet_30F_only","n_jet_30F_only",8,-0.5,7.5);
  TH1F* h_n_jet_30BE   = new TH1F("n_jet_30BE","n_jet_30BE",8,-0.5,7.5);
  TH1F* h_n_jet_30F  = new TH1F("n_jet_30F","n_jet_30F",8,-0.5,7.5);
  TH1F* h_n_jet_30B  = new TH1F("n_jet_30B","n_jet_30B",8,-0.5,7.5);
 TH1F* h_n_jet_30B_only  = new TH1F("n_jet_30B_only","n_jet_30B_only",8,-0.5,7.5);

  TH1F* h_n_jet_50  = new TH1F("n_jet_50","n_jet_50",16,-0.5,15.5);
  TH1F* h_n_jet_50BE_only  = new TH1F("n_jet_50BE_only","n_jet_50BE_only",16,-0.5,15.5);
  TH1F* h_n_jet_50F_only  = new TH1F("n_jet_50F_only","n_jet_50F_only",16,-0.5,15.5);
  TH1F* h_n_jet_50BE   = new TH1F("n_jet_50BE","n_jet_50BE",16,-0.5,15.5);
  TH1F* h_n_jet_50F  = new TH1F("n_jet_50F","n_jet_50F",16,-0.5,15.5);
  TH1F* h_n_jet_50B  = new TH1F("n_jet_50B","n_jet_50B",16,-0.5,15.5);
 TH1F* h_n_jet_50B_only  = new TH1F("n_jet_50B_only","n_jet_50B_only",16,-0.5,15.5);

  TH1F* h_n_inc_jet_30  = new TH1F("n_inc_jet_30","n_inc_jet_30",16,-0.5,15.5);
  TH1F* h_n_inc_jet_30BE_only  = new TH1F("n_inc_jet_30BE_only","n_inc_jet_30BE_only",16,-0.5,15.5);
  TH1F* h_n_inc_jet_30F_only  = new TH1F("n_inc_jet_30F_only","n_inc_jet_30F_only",16,-0.5,15.5);
  TH1F* h_n_inc_jet_30BE   = new TH1F("n_inc_jet_30BE","n_inc_jet_30BE",8,-0.5,7.5);
  TH1F* h_n_inc_jet_30F  = new TH1F("n_inc_jet_30F","n_inc_jet_30F",16,-0.5,15.5);
  TH1F* h_n_inc_jet_30B  = new TH1F("n_inc_jet_30B","n_inc_jet_30B",16,-0.5,15.5);
 TH1F* h_n_inc_jet_30B_only  = new TH1F("n_inc_jet_30B_only","n_inc_jet_30B_only",16,-0.5,15.5);

  TH1F* h_n_inc_jet_50  = new TH1F("n_inc_jet_50","n_inc_jet_50",16,-0.5,15.5);
  TH1F* h_n_inc_jet_50BE_only  = new TH1F("n_inc_jet_50BE_only","n_inc_jet_50BE_only",16,-0.5,15.5);
  TH1F* h_n_inc_jet_50F_only  = new TH1F("n_inc_jet_50F_only","n_inc_jet_50F_only",16,-0.5,15.5);
  TH1F* h_n_inc_jet_50BE   = new TH1F("n_inc_jet_50BE","n_inc_jet_50BE",16,-0.5,15.5);
  TH1F* h_n_inc_jet_50F  = new TH1F("n_inc_jet_50F","n_inc_jet_50F",16,-0.5,15.5);
  TH1F* h_n_inc_jet_50B  = new TH1F("n_inc_jet_50B","n_inc_jet_50B",16,-0.5,15.5);
 TH1F* h_n_inc_jet_50B_only  = new TH1F("n_inc_jet_50B_only","n_inc_jet_50B_only",16,-0.5,15.5);

  TH1F* h_diElecpt_1_30_BEF  = new TH1F("diElecpt_1_30_BEF","diElecpt_1_30_BEF",80,0,400);
  TH1F* h_diElecpt_2_30_BEF  = new TH1F("diElecpt_2_30_BEF","diElecpt_2_30_BEF",80,0,400);
  TH1F* h_diElecpt_3_30_BEF  = new TH1F("diElecpt_3_30_BEF","diElecpt_3_30_BEF",80,0,400);
  TH1F* h_diElecpt_4_30_BEF  = new TH1F("diElecpt_4_30_BEF","diElecpt_4_30_BEF",80,0,400);

  TH1F* h_diElecrapidity_1_30_BEF  = new TH1F("diElecrapidity_1_30_BEF","diElecrapidity_1_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_2_30_BEF  = new TH1F("diElecrapidity_2_30_BEF","diElecrapidity_2_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_3_30_BEF  = new TH1F("diElecrapidity_3_30_BEF","diElecrapidity_3_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_4_30_BEF  = new TH1F("diElecrapidity_4_30_BEF","diElecrapidity_4_30_BEF",30,-3,3);

  TH1F* h_diElecphi_1_30_BEF  = new TH1F("diElecphi_1_30_BEF","diElecphi_1_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_2_30_BEF  = new TH1F("diElecphi_2_30_BEF","diElecphi_2_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_3_30_BEF  = new TH1F("diElecphi_3_30_BEF","diElecphi_3_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_4_30_BEF  = new TH1F("diElecphi_4_30_BEF","diElecphi_4_30_BEF",12,-3.2,3.2);

  TH1F* h_diElecM_1_30_BEF  = new TH1F("diElecM_1_30_BEF","diElecM_1_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_2_30_BEF  = new TH1F("diElecM_2_30_BEF","diElecM_2_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_3_30_BEF  = new TH1F("diElecM_3_30_BEF","diElecM_3_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_4_30_BEF  = new TH1F("diElecM_4_30_BEF","diElecM_4_30_BEF",40, 70.,110.);

  TH1F* h_jetpt_1_30_BEF  = new TH1F("jetpt_1_30_BEF","jetpt_1_30_BEF",80,0,400);
  TH1F* h_jetpt_2_30_BEF  = new TH1F("jetpt_2_30_BEF","jetpt_2_30_BEF",80,0,400);
  TH1F* h_jetpt_3_30_BEF  = new TH1F("jetpt_3_30_BEF","jetpt_3_30_BEF",80,0,400);
  TH1F* h_jetpt_4_30_BEF  = new TH1F("jetpt_4_30_BEF","jetpt_4_30_BEF",80,0,400);

  TH1F* h_diElecpt_e0_30_BEF  = new TH1F("diElecpt_e0_30_BEF","diElecpt_e0_30_BEF",80,0,400);
  TH1F* h_diElecpt_e1_30_BEF  = new TH1F("diElecpt_e1_30_BEF","diElecpt_e1_30_BEF",80,0,400);
  TH1F* h_diElecpt_e2_30_BEF  = new TH1F("diElecpt_e2_30_BEF","diElecpt_e2_30_BEF",80,0,400);
  TH1F* h_diElecpt_e3_30_BEF  = new TH1F("diElecpt_e3_30_BEF","diElecpt_e3_30_BEF",80,0,400);
  TH1F* h_diElecpt_e4_30_BEF  = new TH1F("diElecpt_e4_30_BEF","diElecpt_e4_30_BEF",80,0,400);

  TH1F* h_diElecrapidity_e0_30_BEF  = new TH1F("diElecrapidity_e0_30_BEF","diElecrapidity_e0_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e1_30_BEF  = new TH1F("diElecrapidity_e1_30_BEF","diElecrapidity_e1_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e2_30_BEF  = new TH1F("diElecrapidity_e2_30_BEF","diElecrapidity_e2_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e3_30_BEF  = new TH1F("diElecrapidity_e3_30_BEF","diElecrapidity_e3_30_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e4_30_BEF  = new TH1F("diElecrapidity_e4_30_BEF","diElecrapidity_e4_30_BEF",30,-3,3);

  TH1F* h_diElecphi_e0_30_BEF  = new TH1F("diElecphi_e0_30_BEF","diElecphi_e0_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e1_30_BEF  = new TH1F("diElecphi_e1_30_BEF","diElecphi_e1_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e2_30_BEF  = new TH1F("diElecphi_e2_30_BEF","diElecphi_e2_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e3_30_BEF  = new TH1F("diElecphi_e3_30_BEF","diElecphi_e3_30_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e4_30_BEF  = new TH1F("diElecphi_e4_30_BEF","diElecphi_e4_30_BEF",12,-3.2,3.2);

  TH1F* h_diElecM_e0_30_BEF  = new TH1F("diElecM_e0_30_BEF","diElecM_e0_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_e1_30_BEF  = new TH1F("diElecM_e1_30_BEF","diElecM_e1_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_e2_30_BEF  = new TH1F("diElecM_e2_30_BEF","diElecM_e2_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_e3_30_BEF  = new TH1F("diElecM_e3_30_BEF","diElecM_e3_30_BEF",40, 70.,110.);
  TH1F* h_diElecM_e4_30_BEF  = new TH1F("diElecM_e4_30_BEF","diElecM_e4_30_BEF",40, 70.,110.);

  TH1F* h_jetpt_e1_30_BEF  = new TH1F("jetpt_e1_30_BEF","jetpt_e1_30_BEF",80,0,400);
  TH1F* h_jetpt_e2_30_BEF  = new TH1F("jetpt_e2_30_BEF","jetpt_e2_30_BEF",80,0,400);
  TH1F* h_jetpt_e3_30_BEF  = new TH1F("jetpt_e3_30_BEF","jetpt_e3_30_BEF",80,0,400);
  TH1F* h_jetpt_e4_30_BEF  = new TH1F("jetpt_e4_30_BEF","jetpt_e4_30_BEF",80,0,400);

  TH1F* h_diElecpt_1_50_BEF  = new TH1F("diElecpt_1_50_BEF","diElecpt_1_50_BEF",80,0,400);
  TH1F* h_diElecpt_2_50_BEF  = new TH1F("diElecpt_2_50_BEF","diElecpt_2_50_BEF",80,0,400);
  TH1F* h_diElecpt_3_50_BEF  = new TH1F("diElecpt_3_50_BEF","diElecpt_3_50_BEF",80,0,400);
  TH1F* h_diElecpt_4_50_BEF  = new TH1F("diElecpt_4_50_BEF","diElecpt_4_50_BEF",80,0,400);

  TH1F* h_diElecrapidity_1_50_BEF  = new TH1F("diElecrapidity_1_50_BEF","diElecrapidity_1_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_2_50_BEF  = new TH1F("diElecrapidity_2_50_BEF","diElecrapidity_2_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_3_50_BEF  = new TH1F("diElecrapidity_3_50_BEF","diElecrapidity_3_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_4_50_BEF  = new TH1F("diElecrapidity_4_50_BEF","diElecrapidity_4_50_BEF",30,-3,3);

  TH1F* h_diElecphi_1_50_BEF  = new TH1F("diElecphi_1_50_BEF","diElecphi_1_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_2_50_BEF  = new TH1F("diElecphi_2_50_BEF","diElecphi_2_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_3_50_BEF  = new TH1F("diElecphi_3_50_BEF","diElecphi_3_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_4_50_BEF  = new TH1F("diElecphi_4_50_BEF","diElecphi_4_50_BEF",12,-3.2,3.2);

  TH1F* h_diElecM_1_50_BEF  = new TH1F("diElecM_1_50_BEF","diElecM_1_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_2_50_BEF  = new TH1F("diElecM_2_50_BEF","diElecM_2_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_3_50_BEF  = new TH1F("diElecM_3_50_BEF","diElecM_3_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_4_50_BEF  = new TH1F("diElecM_4_50_BEF","diElecM_4_50_BEF",40, 70.,110.);

  TH1F* h_jetpt_1_50_BEF  = new TH1F("jetpt_1_50_BEF","jetpt_1_50_BEF",80,0,400);
  TH1F* h_jetpt_2_50_BEF  = new TH1F("jetpt_2_50_BEF","jetpt_2_50_BEF",80,0,400);
  TH1F* h_jetpt_3_50_BEF  = new TH1F("jetpt_3_50_BEF","jetpt_3_50_BEF",80,0,400);
  TH1F* h_jetpt_4_50_BEF  = new TH1F("jetpt_4_50_BEF","jetpt_4_50_BEF",80,0,400);

  TH1F* h_diElecpt_e0_50_BEF  = new TH1F("diElecpt_e0_50_BEF","diElecpt_e0_50_BEF",80,0,400);
  TH1F* h_diElecpt_e1_50_BEF  = new TH1F("diElecpt_e1_50_BEF","diElecpt_e1_50_BEF",80,0,400);
  TH1F* h_diElecpt_e2_50_BEF  = new TH1F("diElecpt_e2_50_BEF","diElecpt_e2_50_BEF",80,0,400);
  TH1F* h_diElecpt_e3_50_BEF  = new TH1F("diElecpt_e3_50_BEF","diElecpt_e3_50_BEF",80,0,400);
  TH1F* h_diElecpt_e4_50_BEF  = new TH1F("diElecpt_e4_50_BEF","diElecpt_e4_50_BEF",80,0,400);

  TH1F* h_diElecrapidity_e0_50_BEF  = new TH1F("diElecrapidity_e0_50_BEF","diElecrapidity_e0_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e1_50_BEF  = new TH1F("diElecrapidity_e1_50_BEF","diElecrapidity_e1_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e2_50_BEF  = new TH1F("diElecrapidity_e2_50_BEF","diElecrapidity_e2_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e3_50_BEF  = new TH1F("diElecrapidity_e3_50_BEF","diElecrapidity_e3_50_BEF",30,-3,3);
  TH1F* h_diElecrapidity_e4_50_BEF  = new TH1F("diElecrapidity_e4_50_BEF","diElecrapidity_e4_50_BEF",30,-3,3);

  TH1F* h_diElecphi_e0_50_BEF  = new TH1F("diElecphi_e0_50_BEF","diElecphi_e0_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e1_50_BEF  = new TH1F("diElecphi_e1_50_BEF","diElecphi_e1_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e2_50_BEF  = new TH1F("diElecphi_e2_50_BEF","diElecphi_e2_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e3_50_BEF  = new TH1F("diElecphi_e3_50_BEF","diElecphi_e3_50_BEF",12,-3.2,3.2);
  TH1F* h_diElecphi_e4_50_BEF  = new TH1F("diElecphi_e4_50_BEF","diElecphi_e4_50_BEF",12,-3.2,3.2);

  TH1F* h_diElecM_e0_50_BEF  = new TH1F("diElecM_e0_50_BEF","diElecM_e0_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_e1_50_BEF  = new TH1F("diElecM_e1_50_BEF","diElecM_e1_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_e2_50_BEF  = new TH1F("diElecM_e2_50_BEF","diElecM_e2_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_e3_50_BEF  = new TH1F("diElecM_e3_50_BEF","diElecM_e3_50_BEF",40, 70.,110.);
  TH1F* h_diElecM_e4_50_BEF  = new TH1F("diElecM_e4_50_BEF","diElecM_e4_50_BEF",40, 70.,110.);

  TH1F* h_jetpt_e1_50_BEF  = new TH1F("jetpt_e1_50_BEF","jetpt_e1_50_BEF",80,0,400);
  TH1F* h_jetpt_e2_50_BEF  = new TH1F("jetpt_e2_50_BEF","jetpt_e2_50_BEF",80,0,400);
  TH1F* h_jetpt_e3_50_BEF  = new TH1F("jetpt_e3_50_BEF","jetpt_e3_50_BEF",80,0,400);
  TH1F* h_jetpt_e4_50_BEF  = new TH1F("jetpt_e4_50_BEF","jetpt_e4_50_BEF",80,0,400);

  TH1F* h_diElecpt_1_30_F  = new TH1F("diElecpt_1_30_F","diElecpt_1_30_F",80,0,400);
  TH1F* h_diElecpt_2_30_F  = new TH1F("diElecpt_2_30_F","diElecpt_2_30_F",80,0,400);
  TH1F* h_diElecpt_3_30_F  = new TH1F("diElecpt_3_30_F","diElecpt_3_30_F",80,0,400);
  TH1F* h_diElecpt_4_30_F  = new TH1F("diElecpt_4_30_F","diElecpt_4_30_F",80,0,400);

  TH1F* h_diElecrapidity_1_30_F  = new TH1F("diElecrapidity_1_30_F","diElecrapidity_1_30_F",30,-3,3);
  TH1F* h_diElecrapidity_2_30_F  = new TH1F("diElecrapidity_2_30_F","diElecrapidity_2_30_F",30,-3,3);
  TH1F* h_diElecrapidity_3_30_F  = new TH1F("diElecrapidity_3_30_F","diElecrapidity_3_30_F",30,-3,3);
  TH1F* h_diElecrapidity_4_30_F  = new TH1F("diElecrapidity_4_30_F","diElecrapidity_4_30_F",30,-3,3);

  TH1F* h_diElecphi_1_30_F  = new TH1F("diElecphi_1_30_F","diElecphi_1_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_2_30_F  = new TH1F("diElecphi_2_30_F","diElecphi_2_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_3_30_F  = new TH1F("diElecphi_3_30_F","diElecphi_3_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_4_30_F  = new TH1F("diElecphi_4_30_F","diElecphi_4_30_F",12,-3.2,3.2);

  TH1F* h_diElecM_1_30_F  = new TH1F("diElecM_1_30_F","diElecM_1_30_F",40, 70.,110.);
  TH1F* h_diElecM_2_30_F  = new TH1F("diElecM_2_30_F","diElecM_2_30_F",40, 70.,110.);
  TH1F* h_diElecM_3_30_F  = new TH1F("diElecM_3_30_F","diElecM_3_30_F",40, 70.,110.);
  TH1F* h_diElecM_4_30_F  = new TH1F("diElecM_4_30_F","diElecM_4_30_F",40, 70.,110.);

  TH1F* h_jetpt_1_30_F  = new TH1F("jetpt_1_30_F","jetpt_1_30_F",80,0,400);
  TH1F* h_jetpt_2_30_F  = new TH1F("jetpt_2_30_F","jetpt_2_30_F",80,0,400);
  TH1F* h_jetpt_3_30_F  = new TH1F("jetpt_3_30_F","jetpt_3_30_F",80,0,400);
  TH1F* h_jetpt_4_30_F  = new TH1F("jetpt_4_30_F","jetpt_4_30_F",80,0,400);

  TH1F* h_diElecpt_e0_30_F  = new TH1F("diElecpt_e0_30_F","diElecpt_e0_30_F",80,0,400);
  TH1F* h_diElecpt_e1_30_F  = new TH1F("diElecpt_e1_30_F","diElecpt_e1_30_F",80,0,400);
  TH1F* h_diElecpt_e2_30_F  = new TH1F("diElecpt_e2_30_F","diElecpt_e2_30_F",80,0,400);
  TH1F* h_diElecpt_e3_30_F  = new TH1F("diElecpt_e3_30_F","diElecpt_e3_30_F",80,0,400);
  TH1F* h_diElecpt_e4_30_F  = new TH1F("diElecpt_e4_30_F","diElecpt_e4_30_F",80,0,400);

  TH1F* h_diElecrapidity_e0_30_F  = new TH1F("diElecrapidity_e0_30_F","diElecrapidity_e0_30_F",30,-3,3);
  TH1F* h_diElecrapidity_e1_30_F  = new TH1F("diElecrapidity_e1_30_F","diElecrapidity_e1_30_F",30,-3,3);
  TH1F* h_diElecrapidity_e2_30_F  = new TH1F("diElecrapidity_e2_30_F","diElecrapidity_e2_30_F",30,-3,3);
  TH1F* h_diElecrapidity_e3_30_F  = new TH1F("diElecrapidity_e3_30_F","diElecrapidity_e3_30_F",30,-3,3);
  TH1F* h_diElecrapidity_e4_30_F  = new TH1F("diElecrapidity_e4_30_F","diElecrapidity_e4_30_F",30,-3,3);

  TH1F* h_diElecphi_e0_30_F  = new TH1F("diElecphi_e0_30_F","diElecphi_e0_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_e1_30_F  = new TH1F("diElecphi_e1_30_F","diElecphi_e1_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_e2_30_F  = new TH1F("diElecphi_e2_30_F","diElecphi_e2_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_e3_30_F  = new TH1F("diElecphi_e3_30_F","diElecphi_e3_30_F",12,-3.2,3.2);
  TH1F* h_diElecphi_e4_30_F  = new TH1F("diElecphi_e4_30_F","diElecphi_e4_30_F",12,-3.2,3.2);

  TH1F* h_diElecM_e0_30_F  = new TH1F("diElecM_e0_30_F","diElecM_e0_30_F",40, 70.,110.);
  TH1F* h_diElecM_e1_30_F  = new TH1F("diElecM_e1_30_F","diElecM_e1_30_F",40, 70.,110.);
  TH1F* h_diElecM_e2_30_F  = new TH1F("diElecM_e2_30_F","diElecM_e2_30_F",40, 70.,110.);
  TH1F* h_diElecM_e3_30_F  = new TH1F("diElecM_e3_30_F","diElecM_e3_30_F",40, 70.,110.);
  TH1F* h_diElecM_e4_30_F  = new TH1F("diElecM_e4_30_F","diElecM_e4_30_F",40, 70.,110.);

  TH1F* h_jetpt_e1_30_F  = new TH1F("jetpt_e1_30_F","jetpt_e1_30_F",80,0,400);
  TH1F* h_jetpt_e2_30_F  = new TH1F("jetpt_e2_30_F","jetpt_e2_30_F",80,0,400);
  TH1F* h_jetpt_e3_30_F  = new TH1F("jetpt_e3_30_F","jetpt_e3_30_F",80,0,400);
  TH1F* h_jetpt_e4_30_F  = new TH1F("jetpt_e4_30_F","jetpt_e4_30_F",80,0,400);

  TH1F* h_diElecpt_1_50_F  = new TH1F("diElecpt_1_50_F","diElecpt_1_50_F",80,0,400);
  TH1F* h_diElecpt_2_50_F  = new TH1F("diElecpt_2_50_F","diElecpt_2_50_F",80,0,400);
  TH1F* h_diElecpt_3_50_F  = new TH1F("diElecpt_3_50_F","diElecpt_3_50_F",80,0,400);
  TH1F* h_diElecpt_4_50_F  = new TH1F("diElecpt_4_50_F","diElecpt_4_50_F",80,0,400);

  TH1F* h_diElecrapidity_1_50_F  = new TH1F("diElecrapidity_1_50_F","diElecrapidity_1_50_F",30,-3,3);
  h_diElecrapidity_1_50_F->Sumw2();
  TH1F* h_diElecrapidity_2_50_F  = new TH1F("diElecrapidity_2_50_F","diElecrapidity_2_50_F",30,-3,3);
  h_diElecrapidity_2_50_F->Sumw2();
  TH1F* h_diElecrapidity_3_50_F  = new TH1F("diElecrapidity_3_50_F","diElecrapidity_3_50_F",30,-3,3);
  h_diElecrapidity_3_50_F->Sumw2();
  TH1F* h_diElecrapidity_4_50_F  = new TH1F("diElecrapidity_4_50_F","diElecrapidity_4_50_F",30,-3,3);
  h_diElecrapidity_4_50_F->Sumw2();

  TH1F* h_diElecphi_1_50_F  = new TH1F("diElecphi_1_50_F","diElecphi_1_50_F",12,-3.2,3.2);
  h_diElecphi_1_50_F->Sumw2();
  TH1F* h_diElecphi_2_50_F  = new TH1F("diElecphi_2_50_F","diElecphi_2_50_F",12,-3.2,3.2);
  h_diElecphi_2_50_F->Sumw2();
  TH1F* h_diElecphi_3_50_F  = new TH1F("diElecphi_3_50_F","diElecphi_3_50_F",12,-3.2,3.2);
  h_diElecphi_3_50_F->Sumw2();
  TH1F* h_diElecphi_4_50_F  = new TH1F("diElecphi_4_50_F","diElecphi_4_50_F",12,-3.2,3.2);
  h_diElecphi_4_50_F->Sumw2();

  TH1F* h_diElecM_1_50_F  = new TH1F("diElecM_1_50_F","diElecM_1_50_F",40, 70.,110.);
  h_diElecM_1_50_F->Sumw2();
  TH1F* h_diElecM_2_50_F  = new TH1F("diElecM_2_50_F","diElecM_2_50_F",40, 70.,110.);
  h_diElecM_2_50_F->Sumw2();
  TH1F* h_diElecM_3_50_F  = new TH1F("diElecM_3_50_F","diElecM_3_50_F",40, 70.,110.);
  h_diElecM_3_50_F->Sumw2();
  TH1F* h_diElecM_4_50_F  = new TH1F("diElecM_4_50_F","diElecM_4_50_F",40, 70.,110.);
  h_diElecM_4_50_F->Sumw2();

  TH1F* h_jetpt_1_50_F  = new TH1F("jetpt_1_50_F","jetpt_1_50_F",80,0,400);
  h_jetpt_1_50_F->Sumw2();
  TH1F* h_jetpt_2_50_F  = new TH1F("jetpt_2_50_F","jetpt_2_50_F",80,0,400);
  h_jetpt_2_50_F->Sumw2();
  TH1F* h_jetpt_3_50_F  = new TH1F("jetpt_3_50_F","jetpt_3_50_F",80,0,400);
  h_jetpt_3_50_F->Sumw2();
  TH1F* h_jetpt_4_50_F  = new TH1F("jetpt_4_50_F","jetpt_4_50_F",80,0,400);
  h_jetpt_4_50_F->Sumw2();

  TH1F* h_diElecpt_e0_50_F  = new TH1F("diElecpt_e0_50_F","diElecpt_e0_50_F",80,0,400);
  h_diElecpt_e0_50_F->Sumw2();
  TH1F* h_diElecpt_e1_50_F  = new TH1F("diElecpt_e1_50_F","diElecpt_e1_50_F",80,0,400);
  h_diElecpt_e1_50_F->Sumw2();
  TH1F* h_diElecpt_e2_50_F  = new TH1F("diElecpt_e2_50_F","diElecpt_e2_50_F",80,0,400);
  h_diElecpt_e2_50_F->Sumw2();
  TH1F* h_diElecpt_e3_50_F  = new TH1F("diElecpt_e3_50_F","diElecpt_e3_50_F",80,0,400);
  h_diElecpt_e3_50_F->Sumw2();
  TH1F* h_diElecpt_e4_50_F  = new TH1F("diElecpt_e4_50_F","diElecpt_e4_50_F",80,0,400);
  h_diElecpt_e4_50_F->Sumw2();

  TH1F* h_diElecrapidity_e0_50_F  = new TH1F("diElecrapidity_e0_50_F","diElecrapidity_e0_50_F",30,-3,3);
  h_diElecrapidity_e0_50_F->Sumw2();
  TH1F* h_diElecrapidity_e1_50_F  = new TH1F("diElecrapidity_e1_50_F","diElecrapidity_e1_50_F",30,-3,3);
  h_diElecrapidity_e1_50_F->Sumw2();
  TH1F* h_diElecrapidity_e2_50_F  = new TH1F("diElecrapidity_e2_50_F","diElecrapidity_e2_50_F",30,-3,3);
  h_diElecrapidity_e2_50_F->Sumw2();
  TH1F* h_diElecrapidity_e3_50_F  = new TH1F("diElecrapidity_e3_50_F","diElecrapidity_e3_50_F",30,-3,3);
  h_diElecrapidity_e3_50_F->Sumw2();
  TH1F* h_diElecrapidity_e4_50_F  = new TH1F("diElecrapidity_e4_50_F","diElecrapidity_e4_50_F",30,-3,3);
  h_diElecrapidity_e4_50_F->Sumw2();

  TH1F* h_diElecphi_e0_50_F  = new TH1F("diElecphi_e0_50_F","diElecphi_e0_50_F",12,-3.2,3.2);
  h_diElecphi_e0_50_F->Sumw2();
  TH1F* h_diElecphi_e1_50_F  = new TH1F("diElecphi_e1_50_F","diElecphi_e1_50_F",12,-3.2,3.2);
  h_diElecphi_e1_50_F->Sumw2();
  TH1F* h_diElecphi_e2_50_F  = new TH1F("diElecphi_e2_50_F","diElecphi_e2_50_F",12,-3.2,3.2);
  h_diElecphi_e2_50_F->Sumw2();
  TH1F* h_diElecphi_e3_50_F  = new TH1F("diElecphi_e3_50_F","diElecphi_e3_50_F",12,-3.2,3.2);
  h_diElecphi_e3_50_F->Sumw2();
  TH1F* h_diElecphi_e4_50_F  = new TH1F("diElecphi_e4_50_F","diElecphi_e4_50_F",12,-3.2,3.2);
  h_diElecphi_e4_50_F->Sumw2();

  TH1F* h_diElecM_e0_50_F  = new TH1F("diElecM_e0_50_F","diElecM_e0_50_F",40, 70.,110.);
  h_diElecM_e0_50_F->Sumw2();
  TH1F* h_diElecM_e1_50_F  = new TH1F("diElecM_e1_50_F","diElecM_e1_50_F",40, 70.,110.);
  h_diElecM_e1_50_F->Sumw2();
  TH1F* h_diElecM_e2_50_F  = new TH1F("diElecM_e2_50_F","diElecM_e2_50_F",40, 70.,110.);
  h_diElecM_e2_50_F->Sumw2();
  TH1F* h_diElecM_e3_50_F  = new TH1F("diElecM_e3_50_F","diElecM_e3_50_F",40, 70.,110.);
  h_diElecM_e3_50_F->Sumw2();
  TH1F* h_diElecM_e4_50_F  = new TH1F("diElecM_e4_50_F","diElecM_e4_50_F",40, 70.,110.);
  h_diElecM_e4_50_F->Sumw2();

  TH1F* h_jetpt_e1_50_F  = new TH1F("jetpt_e1_50_F","jetpt_e1_50_F",80,0,400);
  h_jetpt_e1_50_F->Sumw2();
  TH1F* h_jetpt_e2_50_F  = new TH1F("jetpt_e2_50_F","jetpt_e2_50_F",80,0,400);
  h_jetpt_e2_50_F->Sumw2();
  TH1F* h_jetpt_e3_50_F  = new TH1F("jetpt_e3_50_F","jetpt_e3_50_F",80,0,400);
  h_jetpt_e3_50_F->Sumw2();
  TH1F* h_jetpt_e4_50_F  = new TH1F("jetpt_e4_50_F","jetpt_e4_50_F",80,0,400);
  h_jetpt_e4_50_F->Sumw2();

  TH1F* h_diElecpt_1_30_BE  = new TH1F("diElecpt_1_30_BE","diElecpt_1_30_BE",80,0,400);
  h_diElecpt_1_30_BE->Sumw2();
  TH1F* h_diElecpt_2_30_BE  = new TH1F("diElecpt_2_30_BE","diElecpt_2_30_BE",80,0,400);
  h_diElecpt_2_30_BE->Sumw2();
  TH1F* h_diElecpt_3_30_BE  = new TH1F("diElecpt_3_30_BE","diElecpt_3_30_BE",80,0,400);
  h_diElecpt_3_30_BE->Sumw2();
  TH1F* h_diElecpt_4_30_BE  = new TH1F("diElecpt_4_30_BE","diElecpt_4_30_BE",80,0,400);
  h_diElecpt_4_30_BE->Sumw2();

  TH1F* h_diElecrapidity_1_30_BE  = new TH1F("diElecrapidity_1_30_BE","diElecrapidity_1_30_BE",30,-3,3);
  h_diElecrapidity_1_30_BE->Sumw2();
  TH1F* h_diElecrapidity_2_30_BE  = new TH1F("diElecrapidity_2_30_BE","diElecrapidity_2_30_BE",30,-3,3);
  h_diElecrapidity_2_30_BE->Sumw2();
  TH1F* h_diElecrapidity_3_30_BE  = new TH1F("diElecrapidity_3_30_BE","diElecrapidity_3_30_BE",30,-3,3);
  h_diElecrapidity_3_30_BE->Sumw2();
  TH1F* h_diElecrapidity_4_30_BE  = new TH1F("diElecrapidity_4_30_BE","diElecrapidity_4_30_BE",30,-3,3);
  h_diElecrapidity_4_30_BE->Sumw2();

  TH1F* h_diElecphi_1_30_BE  = new TH1F("diElecphi_1_30_BE","diElecphi_1_30_BE",12,-3.2,3.2);
  h_diElecphi_1_30_BE->Sumw2();
  TH1F* h_diElecphi_2_30_BE  = new TH1F("diElecphi_2_30_BE","diElecphi_2_30_BE",12,-3.2,3.2);
  h_diElecphi_2_30_BE->Sumw2();
  TH1F* h_diElecphi_3_30_BE  = new TH1F("diElecphi_3_30_BE","diElecphi_3_30_BE",12,-3.2,3.2);
  h_diElecphi_3_30_BE->Sumw2();
  TH1F* h_diElecphi_4_30_BE  = new TH1F("diElecphi_4_30_BE","diElecphi_4_30_BE",12,-3.2,3.2);
  h_diElecphi_4_30_BE->Sumw2();

  TH1F* h_diElecM_1_30_BE  = new TH1F("diElecM_1_30_BE","diElecM_1_30_BE",40, 70.,110.);
  h_diElecM_1_30_BE->Sumw2();
  TH1F* h_diElecM_2_30_BE  = new TH1F("diElecM_2_30_BE","diElecM_2_30_BE",40, 70.,110.);
  h_diElecM_2_30_BE->Sumw2();
  TH1F* h_diElecM_3_30_BE  = new TH1F("diElecM_3_30_BE","diElecM_3_30_BE",40, 70.,110.);
  h_diElecM_3_30_BE->Sumw2();
  TH1F* h_diElecM_4_30_BE  = new TH1F("diElecM_4_30_BE","diElecM_4_30_BE",40, 70.,110.);
  h_diElecM_4_30_BE->Sumw2();

  TH1F* h_jetpt_1_30_BE  = new TH1F("jetpt_1_30_BE","jetpt_1_30_BE",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  h_jetpt_1_30_BE->Sumw2();
  TH1F* h_jetpt_2_30_BE  = new TH1F("jetpt_2_30_BE","jetpt_2_30_BE",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_jetpt_2_30_BE->Sumw2();
  TH1F* h_jetpt_3_30_BE  = new TH1F("jetpt_3_30_BE","jetpt_3_30_BE",80,0,400);
  h_jetpt_3_30_BE->Sumw2();
  TH1F* h_jetpt_4_30_BE  = new TH1F("jetpt_4_30_BE","jetpt_4_30_BE",80,0,400);
  h_jetpt_4_30_BE->Sumw2();

  TH1F* h_diElecpt_e0_30_BE  = new TH1F("diElecpt_e0_30_BE","diElecpt_e0_30_BE",80,0,400);
  h_diElecpt_e0_30_BE->Sumw2();
  TH1F* h_diElecpt_e1_30_BE  = new TH1F("diElecpt_e1_30_BE","diElecpt_e1_30_BE",80,0,400);
  h_diElecpt_e1_30_BE->Sumw2();
  TH1F* h_diElecpt_e2_30_BE  = new TH1F("diElecpt_e2_30_BE","diElecpt_e2_30_BE",80,0,400);
  h_diElecpt_e2_30_BE->Sumw2();
  TH1F* h_diElecpt_e3_30_BE  = new TH1F("diElecpt_e3_30_BE","diElecpt_e3_30_BE",80,0,400);
  h_diElecpt_e3_30_BE->Sumw2();
  TH1F* h_diElecpt_e4_30_BE  = new TH1F("diElecpt_e4_30_BE","diElecpt_e4_30_BE",80,0,400);
  h_diElecpt_e4_30_BE->Sumw2();

  TH1F* h_diElecrapidity_e0_30_BE  = new TH1F("diElecrapidity_e0_30_BE","diElecrapidity_e0_30_BE",30,-3,3);
  h_diElecrapidity_e0_30_BE->Sumw2();
  TH1F* h_diElecrapidity_e1_30_BE  = new TH1F("diElecrapidity_e1_30_BE","diElecrapidity_e1_30_BE",30,-3,3);
  h_diElecrapidity_e1_30_BE->Sumw2();
  TH1F* h_diElecrapidity_e2_30_BE  = new TH1F("diElecrapidity_e2_30_BE","diElecrapidity_e2_30_BE",30,-3,3);
  h_diElecrapidity_e2_30_BE->Sumw2();
  TH1F* h_diElecrapidity_e3_30_BE  = new TH1F("diElecrapidity_e3_30_BE","diElecrapidity_e3_30_BE",30,-3,3);
  h_diElecrapidity_e3_30_BE->Sumw2();
  TH1F* h_diElecrapidity_e4_30_BE  = new TH1F("diElecrapidity_e4_30_BE","diElecrapidity_e4_30_BE",30,-3,3);
  h_diElecrapidity_e4_30_BE->Sumw2();

  TH1F* h_diElecphi_e0_30_BE  = new TH1F("diElecphi_e0_30_BE","diElecphi_e0_30_BE",12,-3.2,3.2);
  h_diElecphi_e0_30_BE->Sumw2();
  TH1F* h_diElecphi_e1_30_BE  = new TH1F("diElecphi_e1_30_BE","diElecphi_e1_30_BE",12,-3.2,3.2);
  h_diElecphi_e1_30_BE->Sumw2();
  TH1F* h_diElecphi_e2_30_BE  = new TH1F("diElecphi_e2_30_BE","diElecphi_e2_30_BE",12,-3.2,3.2);
  h_diElecphi_e2_30_BE->Sumw2();
  TH1F* h_diElecphi_e3_30_BE  = new TH1F("diElecphi_e3_30_BE","diElecphi_e3_30_BE",12,-3.2,3.2);
  h_diElecphi_e3_30_BE->Sumw2();
  TH1F* h_diElecphi_e4_30_BE  = new TH1F("diElecphi_e4_30_BE","diElecphi_e4_30_BE",12,-3.2,3.2);
  h_diElecphi_e4_30_BE->Sumw2();

  TH1F* h_diElecM_e0_30_BE  = new TH1F("diElecM_e0_30_BE","diElecM_e0_30_BE",40, 70.,110.);
  h_diElecM_e0_30_BE->Sumw2();
  TH1F* h_diElecM_e1_30_BE  = new TH1F("diElecM_e1_30_BE","diElecM_e1_30_BE",40, 70.,110.);
  h_diElecM_e1_30_BE->Sumw2();
  TH1F* h_diElecM_e2_30_BE  = new TH1F("diElecM_e2_30_BE","diElecM_e2_30_BE",40, 70.,110.);
  h_diElecM_e2_30_BE->Sumw2();
  TH1F* h_diElecM_e3_30_BE  = new TH1F("diElecM_e3_30_BE","diElecM_e3_30_BE",40, 70.,110.);
  h_diElecM_e3_30_BE->Sumw2();
  TH1F* h_diElecM_e4_30_BE  = new TH1F("diElecM_e4_30_BE","diElecM_e4_30_BE",40, 70.,110.);
  h_diElecM_e4_30_BE->Sumw2();

  TH1F* h_jetpt_e1_30_BE  = new TH1F("jetpt_e1_30_BE","jetpt_e1_30_BE",80,0,400);
  h_jetpt_e1_30_BE->Sumw2();
  TH1F* h_jetpt_e2_30_BE  = new TH1F("jetpt_e2_30_BE","jetpt_e2_30_BE",80,0,400);
  h_jetpt_e2_30_BE->Sumw2();
  TH1F* h_jetpt_e3_30_BE  = new TH1F("jetpt_e3_30_BE","jetpt_e3_30_BE",80,0,400);
  h_jetpt_e3_30_BE->Sumw2();
  TH1F* h_jetpt_e4_30_BE  = new TH1F("jetpt_e4_30_BE","jetpt_e4_30_BE",80,0,400);
  h_jetpt_e4_30_BE->Sumw2();
  TH1F*h_deltar = new TH1F("deltar","deltar",200,0,5);
  h_deltar->Sumw2();

  TH1F* h_diElecpt_1_30_B  = new TH1F("diElecpt_1_30_B","diElecpt_1_30_B",80,0,400);
  h_diElecpt_1_30_B->Sumw2();
  TH1F* h_diElecpt_2_30_B  = new TH1F("diElecpt_2_30_B","diElecpt_2_30_B",80,0,400);
  h_diElecpt_2_30_B->Sumw2();
  TH1F* h_diElecpt_3_30_B  = new TH1F("diElecpt_3_30_B","diElecpt_3_30_B",80,0,400);
  h_diElecpt_3_30_B->Sumw2();
  TH1F* h_diElecpt_4_30_B  = new TH1F("diElecpt_4_30_B","diElecpt_4_30_B",80,0,400);
  h_diElecpt_4_30_B->Sumw2();

  TH1F* h_diElecrapidity_1_30_B  = new TH1F("diElecrapidity_1_30_B","diElecrapidity_1_30_B",30,-3,3);
  h_diElecrapidity_1_30_B->Sumw2();
  TH1F* h_diElecrapidity_2_30_B  = new TH1F("diElecrapidity_2_30_B","diElecrapidity_2_30_B",30,-3,3);
  h_diElecrapidity_2_30_B->Sumw2();
  TH1F* h_diElecrapidity_3_30_B  = new TH1F("diElecrapidity_3_30_B","diElecrapidity_3_30_B",30,-3,3);
  h_diElecrapidity_3_30_B->Sumw2();
  TH1F* h_diElecrapidity_4_30_B  = new TH1F("diElecrapidity_4_30_B","diElecrapidity_4_30_B",30,-3,3);
  h_diElecrapidity_4_30_B->Sumw2();

  TH1F* h_diElecphi_1_30_B  = new TH1F("diElecphi_1_30_B","diElecphi_1_30_B",12,-3.2,3.2);
  h_diElecphi_1_30_B->Sumw2();
  TH1F* h_diElecphi_2_30_B  = new TH1F("diElecphi_2_30_B","diElecphi_2_30_B",12,-3.2,3.2);
  h_diElecphi_2_30_B->Sumw2();
  TH1F* h_diElecphi_3_30_B  = new TH1F("diElecphi_3_30_B","diElecphi_3_30_B",12,-3.2,3.2);
  h_diElecphi_3_30_B->Sumw2();
  TH1F* h_diElecphi_4_30_B  = new TH1F("diElecphi_4_30_B","diElecphi_4_30_B",12,-3.2,3.2);
  h_diElecphi_4_30_B->Sumw2();

  TH1F* h_diElecM_1_30_B  = new TH1F("diElecM_1_30_B","diElecM_1_30_B",40, 70.,110.);
  h_diElecM_1_30_B->Sumw2();
  TH1F* h_diElecM_2_30_B  = new TH1F("diElecM_2_30_B","diElecM_2_30_B",40, 70.,110.);
  h_diElecM_2_30_B->Sumw2();
  TH1F* h_diElecM_3_30_B  = new TH1F("diElecM_3_30_B","diElecM_3_30_B",40, 70.,110.);
  h_diElecM_3_30_B->Sumw2();
  TH1F* h_diElecM_4_30_B  = new TH1F("diElecM_4_30_B","diElecM_4_30_B",40, 70.,110.);
  h_diElecM_4_30_B->Sumw2();

  TH1F* h_jetpt_1_30_B  = new TH1F("jetpt_1_30_B","jetpt_1_30_B",80,0,400);
  h_jetpt_1_30_B->Sumw2();
  TH1F* h_jetpt_2_30_B  = new TH1F("jetpt_2_30_B","jetpt_2_30_B",80,0,400);
  h_jetpt_2_30_B->Sumw2();
  TH1F* h_jetpt_3_30_B  = new TH1F("jetpt_3_30_B","jetpt_3_30_B",80,0,400);
  h_jetpt_3_30_B->Sumw2();
  TH1F* h_jetpt_4_30_B  = new TH1F("jetpt_4_30_B","jetpt_4_30_B",80,0,400);
  h_jetpt_4_30_B->Sumw2();

  TH1F* h_diElecpt_e0_30_B  = new TH1F("diElecpt_e0_30_B","diElecpt_e0_30_B",80,0,400);
  h_diElecpt_e0_30_B->Sumw2();
  TH1F* h_diElecpt_e1_30_B  = new TH1F("diElecpt_e1_30_B","diElecpt_e1_30_B",80,0,400);
  h_diElecpt_e1_30_B->Sumw2();
  TH1F* h_diElecpt_e2_30_B  = new TH1F("diElecpt_e2_30_B","diElecpt_e2_30_B",80,0,400);
  h_diElecpt_e2_30_B->Sumw2();
  TH1F* h_diElecpt_e3_30_B  = new TH1F("diElecpt_e3_30_B","diElecpt_e3_30_B",80,0,400);
  h_diElecpt_e3_30_B->Sumw2();
  TH1F* h_diElecpt_e4_30_B  = new TH1F("diElecpt_e4_30_B","diElecpt_e4_30_B",80,0,400);
  h_diElecpt_e4_30_B->Sumw2();

  TH1F* h_diElecrapidity_e0_30_B  = new TH1F("diElecrapidity_e0_30_B","diElecrapidity_e0_30_B",30,-3,3);
  h_diElecrapidity_e0_30_B->Sumw2();
  TH1F* h_diElecrapidity_e1_30_B  = new TH1F("diElecrapidity_e1_30_B","diElecrapidity_e1_30_B",30,-3,3);
  h_diElecrapidity_e1_30_B->Sumw2();
  TH1F* h_diElecrapidity_e2_30_B  = new TH1F("diElecrapidity_e2_30_B","diElecrapidity_e2_30_B",30,-3,3);
  h_diElecrapidity_e2_30_B->Sumw2();
  TH1F* h_diElecrapidity_e3_30_B  = new TH1F("diElecrapidity_e3_30_B","diElecrapidity_e3_30_B",30,-3,3);
  h_diElecrapidity_e3_30_B->Sumw2();
  TH1F* h_diElecrapidity_e4_30_B  = new TH1F("diElecrapidity_e4_30_B","diElecrapidity_e4_30_B",30,-3,3);
  h_diElecrapidity_e4_30_B->Sumw2();

  TH1F* h_diElecphi_e0_30_B  = new TH1F("diElecphi_e0_30_B","diElecphi_e0_30_B",12,-3.2,3.2);
  h_diElecphi_e0_30_B->Sumw2();
  TH1F* h_diElecphi_e1_30_B  = new TH1F("diElecphi_e1_30_B","diElecphi_e1_30_B",12,-3.2,3.2);
  h_diElecphi_e1_30_B->Sumw2();
  TH1F* h_diElecphi_e2_30_B  = new TH1F("diElecphi_e2_30_B","diElecphi_e2_30_B",12,-3.2,3.2);
  h_diElecphi_e2_30_B->Sumw2();
  TH1F* h_diElecphi_e3_30_B  = new TH1F("diElecphi_e3_30_B","diElecphi_e3_30_B",12,-3.2,3.2);
  h_diElecphi_e3_30_B->Sumw2();
  TH1F* h_diElecphi_e4_30_B  = new TH1F("diElecphi_e4_30_B","diElecphi_e4_30_B",12,-3.2,3.2);
  h_diElecphi_e4_30_B->Sumw2();

  TH1F* h_diElecM_e0_30_B  = new TH1F("diElecM_e0_30_B","diElecM_e0_30_B",40, 70.,110.);
  h_diElecM_e0_30_B->Sumw2();
  TH1F* h_diElecM_e1_30_B  = new TH1F("diElecM_e1_30_B","diElecM_e1_30_B",40, 70.,110.);
  h_diElecM_e1_30_B->Sumw2();
  TH1F* h_diElecM_e2_30_B  = new TH1F("diElecM_e2_30_B","diElecM_e2_30_B",40, 70.,110.);
  h_diElecM_e2_30_B->Sumw2();
  TH1F* h_diElecM_e3_30_B  = new TH1F("diElecM_e3_30_B","diElecM_e3_30_B",40, 70.,110.);
  h_diElecM_e3_30_B->Sumw2();
  TH1F* h_diElecM_e4_30_B  = new TH1F("diElecM_e4_30_B","diElecM_e4_30_B",40, 70.,110.);
  h_diElecM_e4_30_B->Sumw2();

  TH1F* h_jetpt_e1_30_B  = new TH1F("jetpt_e1_30_B","jetpt_e1_30_B",80,0,400);
  h_jetpt_e1_30_B->Sumw2();
  TH1F* h_jetpt_e2_30_B  = new TH1F("jetpt_e2_30_B","jetpt_e2_30_B",80,0,400);
  h_jetpt_e2_30_B->Sumw2();
  TH1F* h_jetpt_e3_30_B  = new TH1F("jetpt_e3_30_B","jetpt_e3_30_B",80,0,400);
  h_jetpt_e3_30_B->Sumw2();
  TH1F* h_jetpt_e4_30_B  = new TH1F("jetpt_e4_30_B","jetpt_e4_30_B",80,0,400);
  h_jetpt_e4_30_B->Sumw2();

  TH1F* h_diElecpt_1_50_BE  = new TH1F("diElecpt_1_50_BE","diElecpt_1_50_BE",80,0,400);
  h_diElecpt_1_50_BE->Sumw2();
  TH1F* h_diElecpt_2_50_BE  = new TH1F("diElecpt_2_50_BE","diElecpt_2_50_BE",80,0,400);
  h_diElecpt_2_50_BE->Sumw2();
  TH1F* h_diElecpt_3_50_BE  = new TH1F("diElecpt_3_50_BE","diElecpt_3_50_BE",80,0,400);
  h_diElecpt_3_50_BE->Sumw2();
  TH1F* h_diElecpt_4_50_BE  = new TH1F("diElecpt_4_50_BE","diElecpt_4_50_BE",80,0,400);
  h_diElecpt_4_50_BE->Sumw2();

  TH1F* h_diElecrapidity_1_50_BE  = new TH1F("diElecrapidity_1_50_BE","diElecrapidity_1_50_BE",30,-3,3);
  h_diElecrapidity_1_50_BE->Sumw2();
  TH1F* h_diElecrapidity_2_50_BE  = new TH1F("diElecrapidity_2_50_BE","diElecrapidity_2_50_BE",30,-3,3);
  h_diElecrapidity_2_50_BE->Sumw2();
  TH1F* h_diElecrapidity_3_50_BE  = new TH1F("diElecrapidity_3_50_BE","diElecrapidity_3_50_BE",30,-3,3);
  h_diElecrapidity_3_50_BE->Sumw2();
  TH1F* h_diElecrapidity_4_50_BE  = new TH1F("diElecrapidity_4_50_BE","diElecrapidity_4_50_BE",30,-3,3);
  h_diElecrapidity_4_50_BE->Sumw2();

  TH1F* h_diElecphi_1_50_BE  = new TH1F("diElecphi_1_50_BE","diElecphi_1_50_BE",12,-3.2,3.2);
  h_diElecphi_1_50_BE->Sumw2();
  TH1F* h_diElecphi_2_50_BE  = new TH1F("diElecphi_2_50_BE","diElecphi_2_50_BE",12,-3.2,3.2);
  h_diElecphi_2_50_BE->Sumw2();
  TH1F* h_diElecphi_3_50_BE  = new TH1F("diElecphi_3_50_BE","diElecphi_3_50_BE",12,-3.2,3.2);
  h_diElecphi_3_50_BE->Sumw2();
  TH1F* h_diElecphi_4_50_BE  = new TH1F("diElecphi_4_50_BE","diElecphi_4_50_BE",12,-3.2,3.2);
  h_diElecphi_4_50_BE->Sumw2();

  TH1F* h_diElecM_1_50_BE  = new TH1F("diElecM_1_50_BE","diElecM_1_50_BE",40, 70.,110.);
  h_diElecM_1_50_BE->Sumw2();
  TH1F* h_diElecM_2_50_BE  = new TH1F("diElecM_2_50_BE","diElecM_2_50_BE",40, 70.,110.);
  h_diElecM_2_50_BE->Sumw2();
  TH1F* h_diElecM_3_50_BE  = new TH1F("diElecM_3_50_BE","diElecM_3_50_BE",40, 70.,110.);
  h_diElecM_3_50_BE->Sumw2();
  TH1F* h_diElecM_4_50_BE  = new TH1F("diElecM_4_50_BE","diElecM_4_50_BE",40, 70.,110.);
  h_diElecM_4_50_BE->Sumw2();

  TH1F* h_jetpt_1_50_BE  = new TH1F("jetpt_1_50_BE","jetpt_1_50_BE",80,0,400);
  h_jetpt_1_50_BE->Sumw2();
  TH1F* h_jetpt_2_50_BE  = new TH1F("jetpt_2_50_BE","jetpt_2_50_BE",80,0,400);
  h_jetpt_2_50_BE->Sumw2();
  TH1F* h_jetpt_3_50_BE  = new TH1F("jetpt_3_50_BE","jetpt_3_50_BE",80,0,400);
  h_jetpt_3_50_BE->Sumw2();
  TH1F* h_jetpt_4_50_BE  = new TH1F("jetpt_4_50_BE","jetpt_4_50_BE",80,0,400);
  h_jetpt_4_50_BE->Sumw2();

  TH1F* h_diElecpt_e0_50_BE  = new TH1F("diElecpt_e0_50_BE","diElecpt_e0_50_BE",80,0,400);
  h_diElecpt_e0_50_BE->Sumw2();
  TH1F* h_diElecpt_e1_50_BE  = new TH1F("diElecpt_e1_50_BE","diElecpt_e1_50_BE",80,0,400);
  h_diElecpt_e1_50_BE->Sumw2();
  TH1F* h_diElecpt_e2_50_BE  = new TH1F("diElecpt_e2_50_BE","diElecpt_e2_50_BE",80,0,400);
  h_diElecpt_e2_50_BE->Sumw2();
  TH1F* h_diElecpt_e3_50_BE  = new TH1F("diElecpt_e3_50_BE","diElecpt_e3_50_BE",80,0,400);
  h_diElecpt_e3_50_BE->Sumw2();
  TH1F* h_diElecpt_e4_50_BE  = new TH1F("diElecpt_e4_50_BE","diElecpt_e4_50_BE",80,0,400);
  h_diElecpt_e4_50_BE->Sumw2();

  TH1F* h_diElecrapidity_e0_50_BE  = new TH1F("diElecrapidity_e0_50_BE","diElecrapidity_e0_50_BE",30,-3,3);
  h_diElecrapidity_e0_50_BE->Sumw2();
  TH1F* h_diElecrapidity_e1_50_BE  = new TH1F("diElecrapidity_e1_50_BE","diElecrapidity_e1_50_BE",30,-3,3);
  h_diElecrapidity_e1_50_BE->Sumw2();
  TH1F* h_diElecrapidity_e2_50_BE  = new TH1F("diElecrapidity_e2_50_BE","diElecrapidity_e2_50_BE",30,-3,3);
  h_diElecrapidity_e2_50_BE->Sumw2();
  TH1F* h_diElecrapidity_e3_50_BE  = new TH1F("diElecrapidity_e3_50_BE","diElecrapidity_e3_50_BE",30,-3,3);
  h_diElecrapidity_e3_50_BE->Sumw2();
  TH1F* h_diElecrapidity_e4_50_BE  = new TH1F("diElecrapidity_e4_50_BE","diElecrapidity_e4_50_BE",30,-3,3);
  h_diElecrapidity_e4_50_BE->Sumw2();

  TH1F* h_diElecphi_e0_50_BE  = new TH1F("diElecphi_e0_50_BE","diElecphi_e0_50_BE",12,-3.2,3.2);
  h_diElecphi_e0_50_BE->Sumw2();
  TH1F* h_diElecphi_e1_50_BE  = new TH1F("diElecphi_e1_50_BE","diElecphi_e1_50_BE",12,-3.2,3.2);
  h_diElecphi_e1_50_BE->Sumw2();
  TH1F* h_diElecphi_e2_50_BE  = new TH1F("diElecphi_e2_50_BE","diElecphi_e2_50_BE",12,-3.2,3.2);
  h_diElecphi_e2_50_BE->Sumw2();
  TH1F* h_diElecphi_e3_50_BE  = new TH1F("diElecphi_e3_50_BE","diElecphi_e3_50_BE",12,-3.2,3.2);
  h_diElecphi_e3_50_BE->Sumw2();
  TH1F* h_diElecphi_e4_50_BE  = new TH1F("diElecphi_e4_50_BE","diElecphi_e4_50_BE",12,-3.2,3.2);
  h_diElecphi_e4_50_BE->Sumw2();

  TH1F* h_diElecM_e0_50_BE  = new TH1F("diElecM_e0_50_BE","diElecM_e0_50_BE",40, 70.,110.);
  h_diElecM_e0_50_BE->Sumw2();
  TH1F* h_diElecM_e1_50_BE  = new TH1F("diElecM_e1_50_BE","diElecM_e1_50_BE",40, 70.,110.);
  h_diElecM_e1_50_BE->Sumw2();
  TH1F* h_diElecM_e2_50_BE  = new TH1F("diElecM_e2_50_BE","diElecM_e2_50_BE",40, 70.,110.);
  h_diElecM_e2_50_BE->Sumw2();
  TH1F* h_diElecM_e3_50_BE  = new TH1F("diElecM_e3_50_BE","diElecM_e3_50_BE",40, 70.,110.);
  h_diElecM_e3_50_BE->Sumw2();
  TH1F* h_diElecM_e4_50_BE  = new TH1F("diElecM_e4_50_BE","diElecM_e4_50_BE",40, 70.,110.);
  h_diElecM_e4_50_BE->Sumw2();

  TH1F* h_jetpt_e1_50_BE  = new TH1F("jetpt_e1_50_BE","jetpt_e1_50_BE",80,0,400);
  h_jetpt_e1_50_BE->Sumw2();
  TH1F* h_jetpt_e2_50_BE  = new TH1F("jetpt_e2_50_BE","jetpt_e2_50_BE",80,0,400);
  h_jetpt_e2_50_BE->Sumw2();
  TH1F* h_jetpt_e3_50_BE  = new TH1F("jetpt_e3_50_BE","jetpt_e3_50_BE",80,0,400);
  h_jetpt_e3_50_BE->Sumw2();
  TH1F* h_jetpt_e4_50_BE  = new TH1F("jetpt_e4_50_BE","jetpt_e4_50_BE",80,0,400);
  h_jetpt_e4_50_BE->Sumw2();

  TH1F* h_diElecpt_1_50_B  = new TH1F("diElecpt_1_50_B","diElecpt_1_50_B",80,0,400);
  h_diElecpt_1_50_B->Sumw2();
  TH1F* h_diElecpt_2_50_B  = new TH1F("diElecpt_2_50_B","diElecpt_2_50_B",80,0,400);
  h_diElecpt_2_50_B->Sumw2();
  TH1F* h_diElecpt_3_50_B  = new TH1F("diElecpt_3_50_B","diElecpt_3_50_B",80,0,400);
  h_diElecpt_3_50_B->Sumw2();
  TH1F* h_diElecpt_4_50_B  = new TH1F("diElecpt_4_50_B","diElecpt_4_50_B",80,0,400);
  h_diElecpt_4_50_B->Sumw2();

  TH1F* h_diElecrapidity_1_50_B  = new TH1F("diElecrapidity_1_50_B","diElecrapidity_1_50_B",50,-3,3);
  h_diElecrapidity_1_50_B->Sumw2();
  TH1F* h_diElecrapidity_2_50_B  = new TH1F("diElecrapidity_2_50_B","diElecrapidity_2_50_B",50,-3,3);
  h_diElecrapidity_2_50_B->Sumw2();
  TH1F* h_diElecrapidity_3_50_B  = new TH1F("diElecrapidity_3_50_B","diElecrapidity_3_50_B",50,-3,3);
  h_diElecrapidity_3_50_B->Sumw2();
  TH1F* h_diElecrapidity_4_50_B  = new TH1F("diElecrapidity_4_50_B","diElecrapidity_4_50_B",50,-3,3);
  h_diElecrapidity_4_50_B->Sumw2();

  TH1F* h_diElecphi_1_50_B  = new TH1F("diElecphi_1_50_B","diElecphi_1_50_B",12,-3.2,3.2);
  h_diElecphi_1_50_B->Sumw2();
  TH1F* h_diElecphi_2_50_B  = new TH1F("diElecphi_2_50_B","diElecphi_2_50_B",12,-3.2,3.2);
  h_diElecphi_2_50_B->Sumw2();
  TH1F* h_diElecphi_3_50_B  = new TH1F("diElecphi_3_50_B","diElecphi_3_50_B",12,-3.2,3.2);
  h_diElecphi_3_50_B->Sumw2();
  TH1F* h_diElecphi_4_50_B  = new TH1F("diElecphi_4_50_B","diElecphi_4_50_B",12,-3.2,3.2);
  h_diElecphi_4_50_B->Sumw2();

  TH1F* h_diElecM_1_50_B  = new TH1F("diElecM_1_50_B","diElecM_1_50_B",40, 70.,110.);
  h_diElecM_1_50_B->Sumw2();
  TH1F* h_diElecM_2_50_B  = new TH1F("diElecM_2_50_B","diElecM_2_50_B",40, 70.,110.);
  h_diElecM_2_50_B->Sumw2();
  TH1F* h_diElecM_3_50_B  = new TH1F("diElecM_3_50_B","diElecM_3_50_B",40, 70.,110.);
  h_diElecM_3_50_B->Sumw2();
  TH1F* h_diElecM_4_50_B  = new TH1F("diElecM_4_50_B","diElecM_4_50_B",40, 70.,110.);
  h_diElecM_4_50_B->Sumw2();

  TH1F* h_jetpt_1_50_B  = new TH1F("jetpt_1_50_B","jetpt_1_50_B",80,0,400);
  h_jetpt_1_50_B->Sumw2();
  TH1F* h_jetpt_2_50_B  = new TH1F("jetpt_2_50_B","jetpt_2_50_B",80,0,400);
  h_jetpt_2_50_B->Sumw2();
  TH1F* h_jetpt_3_50_B  = new TH1F("jetpt_3_50_B","jetpt_3_50_B",80,0,400);
  h_jetpt_3_50_B->Sumw2();
  TH1F* h_jetpt_4_50_B  = new TH1F("jetpt_4_50_B","jetpt_4_50_B",80,0,400);
  h_jetpt_4_50_B->Sumw2();

  TH1F* h_diElecpt_e0_50_B  = new TH1F("diElecpt_e0_50_B","diElecpt_e0_50_B",80,0,400);
  h_diElecpt_e0_50_B->Sumw2();
  TH1F* h_diElecpt_e1_50_B  = new TH1F("diElecpt_e1_50_B","diElecpt_e1_50_B",80,0,400);
  h_diElecpt_e1_50_B->Sumw2();
  TH1F* h_diElecpt_e2_50_B  = new TH1F("diElecpt_e2_50_B","diElecpt_e2_50_B",80,0,400);
  h_diElecpt_e2_50_B->Sumw2();
  TH1F* h_diElecpt_e3_50_B  = new TH1F("diElecpt_e3_50_B","diElecpt_e3_50_B",80,0,400);
  h_diElecpt_e3_50_B->Sumw2();
  TH1F* h_diElecpt_e4_50_B  = new TH1F("diElecpt_e4_50_B","diElecpt_e4_50_B",80,0,400);
  h_diElecpt_e4_50_B->Sumw2();

  TH1F* h_diElecrapidity_e0_50_B  = new TH1F("diElecrapidity_e0_50_B","diElecrapidity_e0_50_B",50,-3,3);
  h_diElecrapidity_e0_50_B->Sumw2();
  TH1F* h_diElecrapidity_e1_50_B  = new TH1F("diElecrapidity_e1_50_B","diElecrapidity_e1_50_B",50,-3,3);
  h_diElecrapidity_e1_50_B->Sumw2();
  TH1F* h_diElecrapidity_e2_50_B  = new TH1F("diElecrapidity_e2_50_B","diElecrapidity_e2_50_B",50,-3,3);
  h_diElecrapidity_e2_50_B->Sumw2();
  TH1F* h_diElecrapidity_e3_50_B  = new TH1F("diElecrapidity_e3_50_B","diElecrapidity_e3_50_B",50,-3,3);
  h_diElecrapidity_e3_50_B->Sumw2();
  TH1F* h_diElecrapidity_e4_50_B  = new TH1F("diElecrapidity_e4_50_B","diElecrapidity_e4_50_B",50,-3,3);
  h_diElecrapidity_e4_50_B->Sumw2();

  TH1F* h_diElecphi_e0_50_B  = new TH1F("diElecphi_e0_50_B","diElecphi_e0_50_B",12,-3.2,3.2);
  h_diElecphi_e0_50_B->Sumw2();
  TH1F* h_diElecphi_e1_50_B  = new TH1F("diElecphi_e1_50_B","diElecphi_e1_50_B",12,-3.2,3.2);
  h_diElecphi_e1_50_B->Sumw2();
  TH1F* h_diElecphi_e2_50_B  = new TH1F("diElecphi_e2_50_B","diElecphi_e2_50_B",12,-3.2,3.2);
  h_diElecphi_e2_50_B->Sumw2();
  TH1F* h_diElecphi_e3_50_B  = new TH1F("diElecphi_e3_50_B","diElecphi_e3_50_B",12,-3.2,3.2);
  h_diElecphi_e3_50_B->Sumw2();
  TH1F* h_diElecphi_e4_50_B  = new TH1F("diElecphi_e4_50_B","diElecphi_e4_50_B",12,-3.2,3.2);
  h_diElecphi_e4_50_B->Sumw2();

  TH1F* h_diElecM_e0_50_B  = new TH1F("diElecM_e0_50_B","diElecM_e0_50_B",40, 70.,110.);
  h_diElecM_e0_50_B->Sumw2();
  TH1F* h_diElecM_e1_50_B  = new TH1F("diElecM_e1_50_B","diElecM_e1_50_B",40, 70.,110.);
  h_diElecM_e1_50_B->Sumw2();
  TH1F* h_diElecM_e2_50_B  = new TH1F("diElecM_e2_50_B","diElecM_e2_50_B",40, 70.,110.);
  h_diElecM_e2_50_B->Sumw2();
  TH1F* h_diElecM_e3_50_B  = new TH1F("diElecM_e3_50_B","diElecM_e3_50_B",40, 70.,110.);
  h_diElecM_e3_50_B->Sumw2();
  TH1F* h_diElecM_e4_50_B  = new TH1F("diElecM_e4_50_B","diElecM_e4_50_B",40, 70.,110.);
  h_diElecM_e4_50_B->Sumw2();

  TH1F* h_jetpt_e1_50_B  = new TH1F("jetpt_e1_50_B","jetpt_e1_50_B",80,0,400);
  h_jetpt_e1_50_B->Sumw2();
  TH1F* h_jetpt_e2_50_B  = new TH1F("jetpt_e2_50_B","jetpt_e2_50_B",80,0,400);
  h_jetpt_e2_50_B->Sumw2();
  TH1F* h_jetpt_e3_50_B  = new TH1F("jetpt_e3_50_B","jetpt_e3_50_B",80,0,400);
  h_jetpt_e3_50_B->Sumw2();
  TH1F* h_jetpt_e4_50_B  = new TH1F("jetpt_e4_50_B","jetpt_e4_50_B",80,0,400);
  h_jetpt_e4_50_B->Sumw2();

  TH1F* h_inc_jetpt_1_30_BEF  = new TH1F("inc_jetpt_1_30_BEF","inc_jetpt_1_30_BEF",80,0,400);
  TH1F* h_inc_jetpt_2_30_BEF  = new TH1F("inc_jetpt_2_30_BEF","inc_jetpt_2_30_BEF",80,0,400);
  TH1F* h_inc_jetpt_3_30_BEF  = new TH1F("inc_jetpt_3_30_BEF","inc_jetpt_3_30_BEF",80,0,400);
  TH1F* h_inc_jetpt_4_30_BEF  = new TH1F("inc_jetpt_4_30_BEF","inc_jetpt_4_30_BEF",80,0,400);

  TH1F* h_inc_jetpt_1_50_BEF  = new TH1F("inc_jetpt_1_50_BEF","inc_jetpt_1_50_BEF",80,0,400);
  TH1F* h_inc_jetpt_2_50_BEF  = new TH1F("inc_jetpt_2_50_BEF","inc_jetpt_2_50_BEF",80,0,400);
  TH1F* h_inc_jetpt_3_50_BEF  = new TH1F("inc_jetpt_3_50_BEF","inc_jetpt_3_50_BEF",80,0,400);
  TH1F* h_inc_jetpt_4_50_BEF  = new TH1F("inc_jetpt_4_50_BEF","inc_jetpt_4_50_BEF",80,0,400);

  TH1F* h_inc_jetpt_1_30_BE  = new TH1F("inc_jetpt_1_30_BE","inc_jetpt_1_30_BE",80,0,400);
  TH1F* h_inc_jetpt_2_30_BE  = new TH1F("inc_jetpt_2_30_BE","inc_jetpt_2_30_BE",80,0,400);
  TH1F* h_inc_jetpt_3_30_BE  = new TH1F("inc_jetpt_3_30_BE","inc_jetpt_3_30_BE",80,0,400);
  TH1F* h_inc_jetpt_4_30_BE  = new TH1F("inc_jetpt_4_30_BE","inc_jetpt_4_30_BE",80,0,400);

  TH1F* h_inc_jetpt_1_50_BE  = new TH1F("inc_jetpt_1_50_BE","inc_jetpt_1_50_BE",80,0,400);
  TH1F* h_inc_jetpt_2_50_BE  = new TH1F("inc_jetpt_2_50_BE","inc_jetpt_2_50_BE",80,0,400);
  TH1F* h_inc_jetpt_3_50_BE  = new TH1F("inc_jetpt_3_50_BE","inc_jetpt_3_50_BE",80,0,400);
  TH1F* h_inc_jetpt_4_50_BE  = new TH1F("inc_jetpt_4_50_BE","inc_jetpt_4_50_BE",80,0,400);

  TH1F* h_inc_jetpt_1_30_F  = new TH1F("inc_jetpt_1_30_F","inc_jetpt_1_30_F",80,0,400);
  TH1F* h_inc_jetpt_2_30_F  = new TH1F("inc_jetpt_2_30_F","inc_jetpt_2_30_F",80,0,400);
  TH1F* h_inc_jetpt_3_30_F  = new TH1F("inc_jetpt_3_30_F","inc_jetpt_3_30_F",80,0,400);
  TH1F* h_inc_jetpt_4_30_F  = new TH1F("inc_jetpt_4_30_F","inc_jetpt_4_30_F",80,0,400);

  TH1F* h_inc_jetpt_1_50_F  = new TH1F("inc_jetpt_1_50_F","inc_jetpt_1_50_F",80,0,400);
  TH1F* h_inc_jetpt_2_50_F  = new TH1F("inc_jetpt_2_50_F","inc_jetpt_2_50_F",80,0,400);
  TH1F* h_inc_jetpt_3_50_F  = new TH1F("inc_jetpt_3_50_F","inc_jetpt_3_50_F",80,0,400);
  TH1F* h_inc_jetpt_4_50_F  = new TH1F("inc_jetpt_4_50_F","inc_jetpt_4_50_F",80,0,400);

//TBA MACRO 25 03
  TH1F* h_inc_jeteta_1_30_BEF  = new TH1F("inc_jeteta_1_30_BEF","inc_jeteta_1_30_BEF",50,-5,5);
  TH1F* h_inc_jeteta_2_30_BEF  = new TH1F("inc_jeteta_2_30_BEF","inc_jeteta_2_30_BEF",50,-5,5);
  TH1F* h_inc_jeteta_3_30_BEF  = new TH1F("inc_jeteta_3_30_BEF","inc_jeteta_3_30_BEF",50,-5,5);
  TH1F* h_inc_jeteta_4_30_BEF  = new TH1F("inc_jeteta_4_30_BEF","inc_jeteta_4_30_BEF",50,-5,5);

  TH1F* h_inc_jeteta_1_50_BEF  = new TH1F("inc_jeteta_1_50_BEF","inc_jeteta_1_50_BEF",50,-5,5);
  TH1F* h_inc_jeteta_2_50_BEF  = new TH1F("inc_jeteta_2_50_BEF","inc_jeteta_2_50_BEF",50,-5,5);
  TH1F* h_inc_jeteta_3_50_BEF  = new TH1F("inc_jeteta_3_50_BEF","inc_jeteta_3_50_BEF",50,-5,5);
  TH1F* h_inc_jeteta_4_50_BEF  = new TH1F("inc_jeteta_4_50_BEF","inc_jeteta_4_50_BEF",50,-5,5);

  TH1F* h_inc_jeteta_1_30_BE  = new TH1F("inc_jeteta_1_30_BE","inc_jeteta_1_30_BE",50,-5,5);
  TH1F* h_inc_jeteta_2_30_BE  = new TH1F("inc_jeteta_2_30_BE","inc_jeteta_2_30_BE",50,-5,5);
  TH1F* h_inc_jeteta_3_30_BE  = new TH1F("inc_jeteta_3_30_BE","inc_jeteta_3_30_BE",50,-5,5);
  TH1F* h_inc_jeteta_4_30_BE  = new TH1F("inc_jeteta_4_30_BE","inc_jeteta_4_30_BE",50,-5,5);

  TH1F* h_inc_jeteta_1_50_BE  = new TH1F("inc_jeteta_1_50_BE","inc_jeteta_1_50_BE",50,-5,5);
  TH1F* h_inc_jeteta_2_50_BE  = new TH1F("inc_jeteta_2_50_BE","inc_jeteta_2_50_BE",50,-5,5);
  TH1F* h_inc_jeteta_3_50_BE  = new TH1F("inc_jeteta_3_50_BE","inc_jeteta_3_50_BE",50,-5,5);
  TH1F* h_inc_jeteta_4_50_BE  = new TH1F("inc_jeteta_4_50_BE","inc_jeteta_4_50_BE",50,-5,5);

  TH1F* h_inc_jeteta_1_30_F  = new TH1F("inc_jeteta_1_30_F","inc_jeteta_1_30_F",50,-5,5);
  TH1F* h_inc_jeteta_2_30_F  = new TH1F("inc_jeteta_2_30_F","inc_jeteta_2_30_F",50,-5,5);
  TH1F* h_inc_jeteta_3_30_F  = new TH1F("inc_jeteta_3_30_F","inc_jeteta_3_30_F",50,-5,5);
  TH1F* h_inc_jeteta_4_30_F  = new TH1F("inc_jeteta_4_30_F","inc_jeteta_4_30_F",50,-5,5);

  TH1F* h_inc_jeteta_1_50_F  = new TH1F("inc_jeteta_1_50_F","inc_jeteta_1_50_F",50,-5,5);
  TH1F* h_inc_jeteta_2_50_F  = new TH1F("inc_jeteta_2_50_F","inc_jeteta_2_50_F",50,-5,5);
  TH1F* h_inc_jeteta_3_50_F  = new TH1F("inc_jeteta_3_50_F","inc_jeteta_3_50_F",50,-5,5);
  TH1F* h_inc_jeteta_4_50_F  = new TH1F("inc_jeteta_4_50_F","inc_jeteta_4_50_F",50,-5,5);


  TH1F* h_jeteta_1_30  = new TH1F("jeteta_1_30","jeteta_1_30",50,-5,5);
  TH1F* h_jeteta_2_30  = new TH1F("jeteta_2_30","jeteta_2_30",50,-5,5);
  TH1F* h_jeteta_1_30_BE  = new TH1F("jeteta_1_30_BE","jeteta_1_30_BE",24,-2.4,2.4);
  TH1F* h_jeteta_2_30_BE  = new TH1F("jeteta_2_30_BE","jeteta_2_30_BE",24,-2.4,2.4);

  TH1F* h_jeteta_1_50  = new TH1F("jeteta_1_50","jeteta_1_50",50,-5,5);
  TH1F* h_jeteta_2_50  = new TH1F("jeteta_2_50","jeteta_2_50",50,-5,5);

  TH1F* h_c_jetpt_1_30  = new TH1F("c_jetpt_1_30","c_jetpt_1_30",80,0,400);
  TH1F* h_f_jetpt_1_30  = new TH1F("f_jetpt_1_30","f_jetpt_1_30",80,0,400);

  TH1F* h_c_jetpt_2_30  = new TH1F("c_jetpt_2_30","c_jetpt_2_30",80,0,400);
  TH1F* h_f_jetpt_2_30  = new TH1F("f_jetpt_2_30","f_jetpt_2_30",80,0,400);

  TH1F* h_c_jetpt_1_50  = new TH1F("c_jetpt_1_50","c_jetpt_1_50",80,0,400);
  TH1F* h_f_jetpt_1_50  = new TH1F("f_jetpt_1_50","f_jetpt_1_50",80,0,400);

  TH1F* h_c_jetpt_2_50  = new TH1F("c_jetpt_2_50","c_jetpt_2_50",80,0,400);
  TH1F* h_f_jetpt_2_50  = new TH1F("f_jetpt_2_50","f_jetpt_2_50",80,0,400);

  TH1F*h_dYj1j2_30= new TH1F("dYj1j2_30","dYj1j2_30",30,0,5);
  TH1F*h_dphij1j2_30= new TH1F("dphij1j2_30","dphij1j2_30",30,0,3.2);
  TH1F*h_dRj1j2_30= new TH1F("dRj1j2_30","dRj1j2_30",30,0,5);
  TH1F*h_Mj1j2_30= new TH1F("Mj1j2_30","Mj1j2_30",30,0,300);

  TH1F*h_dYj1j2_50= new TH1F("dYj1j2_50","dYj1j2_50",30,0,5);
  TH1F*h_dphij1j2_50= new TH1F("dphij1j2_50","dphij1j2_50",30,0,3.2);
  TH1F*h_dRj1j2_50= new TH1F("dRj1j2_50","dRj1j2_50",30,0,5);
  TH1F*h_Mj1j2_50= new TH1F("Mj1j2_50","Mj1j2_50",30,0,300);

  TH1F*h_dYj1j2_30_F= new TH1F("dYj1j2_30_F","dYj1j2_30_F",30,0,5);
  TH1F*h_dphij1j2_30_F= new TH1F("dphij1j2_30_F","dphij1j2_30_F",30,0,3.2);
  TH1F*h_dRj1j2_30_F= new TH1F("dRj1j2_30_F","dRj1j2_30_F",30,0,5);
  TH1F*h_Mj1j2_30_F= new TH1F("Mj1j2_30_F","Mj1j2_30_F",30,0,300);

  TH1F*h_dYj1j2_50_F= new TH1F("dYj1j2_50_F","dYj1j2_50_F",30,0,5);
  TH1F*h_dphij1j2_50_F= new TH1F("dphij1j2_50_F","dphij1j2_50_F",30,0,3.2);
  TH1F*h_dRj1j2_50_F= new TH1F("dRj1j2_50_F","dRj1j2_50_F",30,0,5);
  TH1F*h_Mj1j2_50_F= new TH1F("Mj1j2_50_F","Mj1j2_50_F",30,0,300);

  TH1F*  h_beta_30F = new TH1F("beta_30F","beta_30F",40,0,1);
  TH1F*  h_betastar_30F = new TH1F("betastar_30F","betastar_30F",40,0,1);
  TH1F*  h_jet_mva_30F = new TH1F("jet_mva_30F","jet_mva_30F",80,-1,1);
  TH2F*  h_jet_mva_beta_30F = new TH2F("jet_mva_beta_30F","jet_mva_beta_30F",80,-1,1,40,0,1);
  TH2F*  h_jet_mva_betastar_30F = new TH2F("jet_mva_betastar_30F","jet_mva_betastar_30F",80,-1,1,40,0,1);
  TH1F*  h_beta_50F = new TH1F("beta_50F","beta_50F",40,0,1);
  TH1F*  h_betastar_50F = new TH1F("betastar_50F","betastar_50F",40,0,1);
  TH1F*  h_jet_mva_50F = new TH1F("jet_mva_50F","jet_mva_50F",80,-1,1);
  TH2F*  h_jet_mva_beta_50F = new TH2F("jet_mva_beta_50F","jet_mva_beta_50F",80,-1,1,40,0,1);
  TH2F*  h_jet_mva_betastar_50F = new TH2F("jet_mva_betastar_50F","jet_mva_betastar_50F",80,-1,1,40,0,1);

  TH1F*  h_beta_30BEF = new TH1F("beta_30BEF","beta_30BEF",40,0,1);
  h_beta_30BEF->Sumw2();
  TH1F*  h_betastar_30BEF = new TH1F("betastar_30BEF","betastar_30BEF",40,0,1);
  h_betastar_30BEF->Sumw2();
  TH1F*  h_jet_mva_30BEF = new TH1F("jet_mva_30BEF","jet_mva_30BEF",80,-1,1);
  h_jet_mva_30BEF->Sumw2();
  TH2F*  h_jet_mva_beta_30BEF = new TH2F("jet_mva_beta_30BEF","jet_mva_beta_30BEF",80,-1,1,40,0,1);
  h_jet_mva_beta_30BEF->Sumw2();
  TH2F*  h_jet_mva_betastar_30BEF = new TH2F("jet_mva_betastar_30BEF","jet_mva_betastar_30BEF",80,-1,1,40,0,1);
  h_jet_mva_betastar_30BEF->Sumw2();
  TH1F*  h_beta_50BEF = new TH1F("beta_50BEF","beta_50BEF",40,0,1);
  h_beta_50BEF->Sumw2();
  TH1F*  h_betastar_50BEF = new TH1F("betastar_50BEF","betastar_50BEF",40,0,1);
  h_betastar_50BEF->Sumw2();
  TH1F*  h_jet_mva_50BEF = new TH1F("jet_mva_50BEF","jet_mva_50BEF",80,-1,1);
  h_jet_mva_50BEF->Sumw2();
  TH2F*  h_jet_mva_beta_50BEF = new TH2F("jet_mva_beta_50BEF","jet_mva_beta_50BEF",80,-1,1,40,0,1);
  h_jet_mva_beta_50BEF->Sumw2();
  TH2F*  h_jet_mva_betastar_50BEF = new TH2F("jet_mva_betastar_50BEF","jet_mva_betastar_50BEF",80,-1,1,40,0,1);
  h_jet_mva_betastar_50BEF->Sumw2();



//END TBA///////

  TH1F*  h_beta_30BE = new TH1F("beta_30BE","beta_30BE",40,0,1);
  h_beta_30BE->Sumw2();
  TH1F*  h_betastar_30BE = new TH1F("betastar_30BE","betastar_30BE",40,0,1);
  h_betastar_30BE->Sumw2();
  TH1F*  h_jet_mva_30BE = new TH1F("jet_mva_30BE","jet_mva_30BE",80,-1,1);
  h_jet_mva_30BE->Sumw2();
  TH2F*  h_jet_mva_beta_30BE = new TH2F("jet_mva_beta_30BE","jet_mva_beta_30BE",80,-1,1,40,0,1);
  h_jet_mva_beta_30BE->Sumw2();
  TH2F*  h_jet_mva_betastar_30BE = new TH2F("jet_mva_betastar_30BE","jet_mva_betastar_30BE",80,-1,1,40,0,1);
  h_jet_mva_betastar_30BE->Sumw2();
  TH1F*  h_beta_50BE = new TH1F("beta_50BE","beta_50BE",40,0,1);
  h_beta_50BE->Sumw2();
  TH1F*  h_betastar_50BE = new TH1F("betastar_50BE","betastar_50BE",40,0,1);
  h_betastar_50BE->Sumw2();
  TH1F*  h_jet_mva_50BE = new TH1F("jet_mva_50BE","jet_mva_50BE",80,-1,1);
  h_jet_mva_50BE->Sumw2();
  TH2F*  h_jet_mva_beta_50BE = new TH2F("jet_mva_beta_50BE","jet_mva_beta_50BE",80,-1,1,40,0,1);
  h_jet_mva_beta_50BE->Sumw2();
  TH2F*  h_jet_mva_betastar_50BE = new TH2F("jet_mva_betastar_50BE","jet_mva_betastar_50BE",80,-1,1,40,0,1);
  h_jet_mva_betastar_50BE->Sumw2();

  TH1F*  h_beta_30B = new TH1F("beta_30B","beta_30B",40,0,1);
  h_beta_30B->Sumw2();
  TH1F*  h_betastar_30B = new TH1F("betastar_30B","betastar_30B",40,0,1);
  h_betastar_30B->Sumw2();
  TH1F*  h_jet_mva_30B = new TH1F("jet_mva_30B","jet_mva_30B",80,-1,1);
  h_jet_mva_30B->Sumw2();
  TH2F*  h_jet_mva_beta_30B = new TH2F("jet_mva_beta_30B","jet_mva_beta_30B",80,-1,1,40,0,1);
  h_jet_mva_beta_30B->Sumw2();
  TH2F*  h_jet_mva_betastar_30B = new TH2F("jet_mva_betastar_30B","jet_mva_betastar_30B",80,-1,1,40,0,1);
  h_jet_mva_betastar_30B->Sumw2();
  TH1F*  h_beta_50B = new TH1F("beta_50B","beta_50B",40,0,1);
  h_beta_50B->Sumw2();
  TH1F*  h_betastar_50B = new TH1F("betastar_50B","betastar_50B",40,0,1);
  h_betastar_50B->Sumw2();
  TH1F*  h_jet_mva_50B = new TH1F("jet_mva_50B","jet_mva_50B",80,-1,1);
  h_jet_mva_50B->Sumw2();
  TH2F*  h_jet_mva_beta_50B = new TH2F("jet_mva_beta_50B","jet_mva_beta_50B",80,-1,1,40,0,1);
  h_jet_mva_beta_50B->Sumw2();
  TH2F*  h_jet_mva_betastar_50B = new TH2F("jet_mva_betastar_50B","jet_mva_betastar_50B",80,-1,1,40,0,1);
  h_jet_mva_betastar_50B->Sumw2();

  TH1F *h_jet_pt_y[100];
  TH1F *h_jet_y_pt[100];
  TH1F *h_jet_pt_y_z0[100];
  TH1F *h_jet_pt_y_z1[100];
  TH1F *h_jet_y_pt_z0[100];
  TH1F *h_jet_y_pt_z1[100];

  TH1F *h_jet_pt_y_Gen[100];
  TH1F *h_jet_y_pt_Gen[100];
  TH1F *h_jet_pt_y_z0_Gen[100];
  TH1F *h_jet_pt_y_z1_Gen[100];
  TH1F *h_jet_y_pt_z0_Gen[100];
  TH1F *h_jet_y_pt_z1_Gen[100];

  TH1F *h_ljet_pt_y[100];
  TH1F *h_ljet_y_pt[100];
  TH1F *h_ljet_pt_y_z0[100];
  TH1F *h_ljet_pt_y_z1[100];
  TH1F *h_ljet_y_pt_z0[100];
  TH1F *h_ljet_y_pt_z1[100];

  TH1F *h_ljet_pt_y_Gen[100];
  TH1F *h_ljet_y_pt_Gen[100];
  TH1F *h_ljet_pt_y_z0_Gen[100];
  TH1F *h_ljet_pt_y_z1_Gen[100];
  TH1F *h_ljet_y_pt_z0_Gen[100];
  TH1F *h_ljet_y_pt_z1_Gen[100];


  TH2* h_ljet_pt_y_Gen_VS_reco[100];
  TH2* h_ljet_y_pt_Gen_VS_reco[100];
  TH2* h_ljet_pt_y_z0_Gen_VS_reco[100];
  TH2* h_ljet_pt_y_z1_Gen_VS_reco[100];
  TH2* h_ljet_y_pt_z0_Gen_VS_reco[100];
  TH2* h_ljet_y_pt_z1_Gen_VS_reco[100];



  TH1F *h_z_pt_y[100][100];
  TH1F *h_z_y_pt[100][100];
  TH1F *h_z_pt_y_Gen[100][100];
  TH1F *h_z_y_pt_Gen[100][100];
  TH2* h_z_pt_y_Gen_VS_reco[100][100];
  TH2* h_z_y_pt_Gen_VS_reco[100][100];

  char name1[100];
  char name2[100];
  char name3[100];
  char name4[100];
  char name5[100];
  char name6[100];
  char name7[100];
  char name8[100];
  char name9[100];
  char name10[100];
  char name11[100];
  char name12[100];
  char name13[100];
  char name14[100];
  char name15[100];
  char name16[100];
  char name17[100];
  char name18[100];
  char name19[100];
  char name20[100];
  char name21[100];
  char name22[100];
  char name23[100];
  char name24[100];
  char name25[100];
  char name26[100];
  char name27[100];
  char name28[100];
  char name29[100];
  char name30[100];
  char name31[100];
  char name32[100];
  char name33[100];
  char name34[100];
  char name35[100];
  char name36[100];

  double j_pT_range[9]={30.,50.,70.,100.,130.,170.,220.,350.,1000.};
  double j_Y_range[9]={0.,0.5,1.0,1.5,2.0,2.5,3.0,3.2,4.7};

  double Z_pT_range[6]={20.,30.,50.,100.,200.,1000.};
  double Z_Y_range[6]={0.,0.5,1.0,1.5,2.0,2.5};
 //   Z_y_range[5]={0,0.9,1.5,2.1,2.5};//
 //   threed_j_y_range[4]={0,1.5,2.4,4.7};//
 // pt_range[6]={20.,30.,50.,100.,200.,1000.};//
for(int i = 0; i<5;i++){//Z rapidity range
  for(int j = 0; j<3;j++){//jet rapidity range
//Z PT vrt jet 
    sprintf(name3,"Z_pt_y_%i_%i",i,j);
    h_z_pt_y[i][j] = new TH1F (name3,name3, 5, Z_pT_range);
    sprintf(name4,"Z_y_pt_%i_%i",i,j);
    h_z_y_pt[i][j] = new TH1F (name4,name4,5, Z_Y_range);
  
    sprintf(name21,"Z_pt_y_Gen_%i_%i",i,j);
    h_z_pt_y_Gen[i][j] = new TH1F (name21,name21, 5, Z_pT_range);
    sprintf(name22,"Z_y_pt_Gen_%i_%i",i,j);
    h_z_y_pt_Gen[i][j] = new TH1F (name22,name22, 5, Z_Y_range);

    sprintf(name23,"Z_pt_y_Gen_VS_reco_%i_%i",i,j);
    h_z_pt_y_Gen_VS_reco[i][j] = new TH2F (name23,name23, 5, Z_pT_range, 5, Z_pT_range);
    sprintf(name24,"Z_y_pt_Gen_VS_reco_%i_%i",i,j);
    h_z_y_pt_Gen_VS_reco[i][j] = new TH2F (name24,name24, 5, Z_Y_range, 5, Z_Y_range);
     
  }
}
for(int i = 0; i<9;i++){//j rapidity range

  sprintf(name1,"jet_pt_y_%i",i);
  h_jet_pt_y[i] = new TH1F (name1,name1, 8, j_pT_range);
  sprintf(name2,"jet_y_pt_%i",i);
  h_jet_y_pt[i] = new TH1F (name2,name2, 8, j_Y_range);
  sprintf(name5,"jet_pt_y_z0_%i",i);
  h_jet_pt_y_z0[i] = new TH1F (name5,name5, 8, j_pT_range);
  sprintf(name6,"jet_pt_y_z1_%i",i);
  h_jet_pt_y_z1[i] = new TH1F (name6,name6, 8, j_pT_range);
  sprintf(name7,"jet_y_pt_z0_%i",i);
  h_jet_y_pt_z0[i] = new TH1F (name7,name7, 8, j_Y_range);
  sprintf(name8,"jet_y_pt_z1_%i",i);
  h_jet_y_pt_z1[i] = new TH1F (name8,name8, 8, j_Y_range);

  sprintf(name15,"jet_pt_y_Gen_%i",i);
  h_jet_pt_y_Gen[i] = new TH1F(name15,name15,8,j_pT_range);
  sprintf(name16,"jet_y_pt_Gen_%i",i);
  h_jet_y_pt_Gen[i] = new TH1F(name16,name16,8,j_Y_range);
  sprintf(name17,"jet_pt_y_Gen_z0_%i",i);
  h_jet_pt_y_z0_Gen[i] = new TH1F(name17,name17,8,j_pT_range);
  sprintf(name18,"jet_pt_y_Gen_z1_%i",i);
  h_jet_pt_y_z1_Gen[i] = new TH1F(name18,name18,8,j_pT_range);
  sprintf(name19,"jet_y_pt_Gen_z0_%i",i);
  h_jet_y_pt_z0_Gen[i] = new TH1F(name19,name19,8,j_Y_range);
  sprintf(name20,"jet_y_pt_Gen_z1_%i",i);
  h_jet_y_pt_z1_Gen[i] = new TH1F(name20,name20,8,j_Y_range);




  sprintf(name25,"ljet_pt_y_%i",i);
  h_ljet_pt_y[i] = new TH1F (name25,name25, 8, j_pT_range);
  sprintf(name26,"ljet_y_pt_%i",i);
  h_ljet_y_pt[i] = new TH1F (name26,name26, 8, j_Y_range);
  sprintf(name27,"ljet_pt_y_z0_%i",i);
  h_ljet_pt_y_z0[i] = new TH1F (name27,name27, 8, j_pT_range);
  sprintf(name28,"ljet_pt_y_z1_%i",i);
  h_ljet_pt_y_z1[i] = new TH1F (name28,name28, 8, j_pT_range);
  sprintf(name29,"ljet_y_pt_z0_%i",i);
  h_ljet_y_pt_z0[i] = new TH1F (name29,name29, 8, j_Y_range);
  sprintf(name30,"ljet_y_pt_z1_%i",i);
  h_ljet_y_pt_z1[i] = new TH1F (name30,name30, 8, j_Y_range);


  sprintf(name31,"ljet_pt_y_Gen_%i",i);
  h_ljet_pt_y_Gen[i] = new TH1F(name31,name31,8,j_pT_range);
  sprintf(name32,"ljet_y_pt_Gen_%i",i);
  h_ljet_y_pt_Gen[i] = new TH1F(name32,name32,8,j_Y_range);
  sprintf(name33,"ljet_pt_y_Gen_z0_%i",i);
  h_ljet_pt_y_z0_Gen[i] = new TH1F(name33,name33,8,j_pT_range);
  sprintf(name34,"ljet_pt_y_Gen_z1_%i",i);
  h_ljet_pt_y_z1_Gen[i] = new TH1F(name34,name34,8,j_pT_range);
  sprintf(name35,"ljet_y_pt_Gen_z0_%i",i);
  h_ljet_y_pt_z0_Gen[i] = new TH1F(name35,name35,8,j_Y_range);
  sprintf(name36,"ljet_y_pt_Gen_z1_%i",i);
  h_ljet_y_pt_z1_Gen[i] = new TH1F(name36,name36,8,j_Y_range);

  sprintf(name9,"ljet_pt_y_Gen_VS_reco_%i",i);
  h_ljet_pt_y_Gen_VS_reco[i] = new TH2F(name9,name9,8,j_pT_range,8,j_pT_range);
  sprintf(name10,"ljet_y_pt_Gen_VS_reco_%i",i);
  h_ljet_y_pt_Gen_VS_reco[i] = new TH2F(name10,name10,8,j_Y_range,8,j_Y_range);
  sprintf(name11,"ljet_pt_y_Gen_VS_reco_z0_%i",i);
  h_ljet_pt_y_z0_Gen_VS_reco[i] = new TH2F(name11,name11,8,j_pT_range,8,j_pT_range);
  sprintf(name12,"ljet_pt_y_Gen_VS_reco_z1_%i",i);
  h_ljet_pt_y_z1_Gen_VS_reco[i] = new TH2F(name12,name12,8,j_pT_range,8,j_pT_range);
  sprintf(name13,"ljet_y_pt_Gen_VS_reco_z0_%i",i);
  h_ljet_y_pt_z0_Gen_VS_reco[i] = new TH2F(name13,name13,8,j_Y_range,8,j_Y_range);
  sprintf(name14,"ljet_y_pt_Gen_VS_reco_z1_%i",i);
  h_ljet_y_pt_z1_Gen_VS_reco[i] = new TH2F(name14,name14,8,j_Y_range,8,j_Y_range);


}

//  double j_pT_range_unf[18]={30,50,70,90,110,130,150,170,190,210,230,250,275,300,325,350,375,400};
  double n_jet_range[10]={-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5};

  TH1F* h_n_jet_30_unfold  = new TH1F("n_jet_30_unfold","n_jet_30_unfold",9,n_jet_range);
  TH1F* h_n_jet_30_gen  = new TH1F("n_jet_30_gen","n_jet_30_gen",9,n_jet_range);
  TH2* h_n_jet_30_Gen_VS_reco = new TH2F("n_jet_30_Gen_VS_reco","n_jet_30_Gen_VS_reco",9,n_jet_range,9,n_jet_range);

  TH1F* h_n_jet_30_unselected = new TH1F("n_jet_30_unselected","n_jet_30_unselected",9,n_jet_range);
//  TH2* h_jetpt_Gen_VS_reco= new TH2F("jetpt_Gen_VS_reco","jetpt_Gen_VS_reco",17,j_pT_range_unf,17,j_pT_range_unf);
//  TH2* h_jetY_Gen_VS_reco= new TH2F("h_jetY_Gen_VS_reco","h_jetY_Gen_VS_reco",25,-2.5,2.5,25,-2.5,2.5);
 TH1F* h_1jetpt  = new TH1F("jetpt_1","1jetpt",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  h_1jetpt->Sumw2(); 
  TH1F* h_2jetpt  = new TH1F("jetpt_2","2jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_2jetpt->Sumw2();
  //TH1F* h_jetY = new TH1F("jetY","jetY",24,-2.4,2.4);
  //h_jetY->Sumw2();
  TH1F* h_1jetY  = new TH1F("jetY_1","1jetY",24,-2.4,2.4);
  h_1jetY->Sumw2();
  TH1F* h_2jetY  = new TH1F("jetY_2","2jetY",24,-2.4,2.4);
  h_2jetY->Sumw2();

  //TH1F* h_Gjetpt  = new TH1F("Gjetpt","Gjetpt",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  //h_Gjetpt->Sumw2();
  TH1F* h_1Gjetpt  = new TH1F("Gjetpt_1","G1jetpt",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  h_1Gjetpt->Sumw2(); 
  TH1F* h_2Gjetpt  = new TH1F("Gjetpt_2","G2jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_2Gjetpt->Sumw2();
  //TH1F* h_GjetY = new TH1F("GjetY","GjetY",24,-2.4,2.4);
  //h_GjetY->Sumw2();
  TH1F* h_1GjetY  = new TH1F("GjetY_1","G1jetY",24,-2.4,2.4);
  h_1GjetY->Sumw2();
  TH1F* h_2GjetY  = new TH1F("GjetY_2","G2jetY",24,-2.4,2.4);
  h_2GjetY->Sumw2();

  //TH2* h_jetpt_Gen_VS_reco= new TH2F("jetpt_Gen_VS_reco","jetpt_Gen_VS_reco",nJetPt_Zinc1jet,jetPt_Zinc1jet,nJetPt_Zinc1jet,jetPt_Zinc1jet);
  //TH2* h_jetY_Gen_VS_reco= new TH2F("h_jetY_Gen_VS_reco","h_jetY_Gen_VS_reco",24,-2.4,2.4,24,-2.4,2.4);
  TH2* h_1jetpt_Gen_VS_reco= new TH2F("jet1pt_Gen_VS_reco","jet1pt_Gen_VS_reco",nJetPt_Zinc1jet,jetPt_Zinc1jet,nJetPt_Zinc1jet,jetPt_Zinc1jet);
  TH2* h_1jetY_Gen_VS_reco = new TH2F("jet1Y_Gen_VS_reco","jet1Y_Gen_VS_reco",24,-2.4,2.4,24,-2.4,2.4);
  TH2* h_2jetpt_Gen_VS_reco= new TH2F("jet2pt_Gen_VS_reco","2jet2pt_Gen_VS_reco",nJetPt_Zinc2jet,jetPt_Zinc2jet,nJetPt_Zinc2jet,jetPt_Zinc2jet);
  TH2* h_2jetY_Gen_VS_reco= new TH2F("jet2Y_Gen_VS_reco","jet2Y_Gen_VS_reco",24,-2.4,2.4,24,-2.4,2.4);

