 /*
  TH1D*   = new TH1D("","",);
  ->Sumw2();
  */
  ///////////////////////////////////////
/* int nJetPt_Zinc1jet(11);
  double jetPt_Zinc1jet[12] = {30, 40, 52, 68, 88, 113, 144, 184, 234, 297, 377, 480};
  int nJetPt_Zinc2jet(10);
  double jetPt_Zinc2jet[11] = {30, 40, 52, 68, 88, 113, 144, 184, 234, 297, 377};
*/
  TH1::SetDefaultSumw2();
  TH2::SetDefaultSumw2();
  TH1D* h_weight_plus_sherpa= new TH1D("weight_plus_sherpa","weight_plus_sherpa",150,0.,15.);
  TH1D* h_weight_minus_sherpa= new TH1D("weight_minus_sherpa","weight_minus_sherpa",150,0.,15.);
  TH1D* h_ljet_rapidity= new TH1D("ljet_rapidity","ljet_rapidity",50,-5.1,5.1);
  TH1D* h_ljet_eta= new TH1D("ljet_eta","ljet_eta",50,-5.1,5.1);
  TH1D* h_sljet_rapidity= new TH1D("sljet_rapidity","sljet_rapidity",50,-5.1,5.1);
  TH1D* h_sljet_eta= new TH1D("sljet_eta","sljet_eta",50,-5.1,5.1);

  TH1D* h_ljet_rapidity_0= new TH1D("ljet_rapidity_0","ljet_rapidity_0",50,-5.1,5.1);
  TH1D* h_ljet_eta_0= new TH1D("ljet_eta_0","ljet_eta_0",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_1= new TH1D("ljet_rapidity_1","ljet_rapidity_1",50,-5.1,5.1);
  TH1D* h_ljet_eta_1= new TH1D("ljet_eta_1","ljet_eta_1",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_2= new TH1D("ljet_rapidity_2","ljet_rapidity_2",50,-5.1,5.1);
  TH1D* h_ljet_eta_2= new TH1D("ljet_eta_2","ljet_eta_2",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_3= new TH1D("ljet_rapidity_3","ljet_rapidity_3",50,-5.1,5.1);
  TH1D* h_ljet_eta_3= new TH1D("ljet_eta_3","ljet_eta_3",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_4= new TH1D("ljet_rapidity_4","ljet_rapidity_4",50,-5.1,5.1);
  TH1D* h_ljet_eta_4= new TH1D("ljet_eta_4","ljet_eta_4",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_5= new TH1D("ljet_rapidity_5","ljet_rapidity_5",50,-5.1,5.1);
  TH1D* h_ljet_eta_5= new TH1D("ljet_eta_5","ljet_eta_5",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_6= new TH1D("ljet_rapidity_6","ljet_rapidity_6",50,-5.1,5.1);
  TH1D* h_ljet_eta_6= new TH1D("ljet_eta_6","ljet_eta_6",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_7= new TH1D("ljet_rapidity_7","ljet_rapidity_7",50,-5.1,5.1);
  TH1D* h_ljet_eta_7= new TH1D("ljet_eta_7","ljet_eta_7",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_8= new TH1D("ljet_rapidity_8","ljet_rapidity_8",50,-5.1,5.1);
  TH1D* h_ljet_eta_8= new TH1D("ljet_eta_8","ljet_eta_8",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_9= new TH1D("ljet_rapidity_9","ljet_rapidity_9",50,-5.1,5.1);
  TH1D* h_ljet_eta_9= new TH1D("ljet_eta_9","ljet_eta_9",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_10= new TH1D("ljet_rapidity_10","ljet_rapidity_10",50,-5.1,5.1);
  TH1D* h_ljet_eta_10= new TH1D("ljet_eta_10","ljet_eta_10",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_11= new TH1D("ljet_rapidity_11","ljet_rapidity_11",50,-5.1,5.1);
  TH1D* h_ljet_eta_11= new TH1D("ljet_eta_11","ljet_eta_11",50,-5.1,5.1);
  TH1D* h_ljet_rapidity_12= new TH1D("ljet_rapidity_12","ljet_rapidity_12",50,-5.1,5.1);
  TH1D* h_ljet_eta_12= new TH1D("ljet_eta_12","ljet_eta_12",50,-5.1,5.1);



  TH1D* h_deltar = new TH1D("h_deltar","h_deltar",50,0.0,2.0);
  int nJetPt_Zinc1jet(22);
  double jetPt_Zinc1jet[23] = {20, 24, 30, 39, 49, 60, 72, 85, 100, 117, 136, 157, 187, 220, 258, 300, 350, 400, 450, 500, 590, 700, 1000};

 int nJetPt_Zinc2jet(21);
  double jetPt_Zinc2jet[22] = {20, 24, 30, 39, 49, 60, 72, 85, 100, 117, 136, 157, 187, 220, 258, 300, 350, 400, 450, 500, 590, 800};


  TH1D* h_nup = new TH1D("nup","nup",6,-0.5,5.5);
  TH1D* h_nup_elec = new TH1D("nup_elec","nup",6,-0.5,5.5);
  TH1D* h_nup_mu = new TH1D("nup_mu","nup",6,-0.5,5.5);
 TH1D* h_nup_elec_with_cuts_noweigt = new TH1D("nup_elec_with_cuts_noweigt","nup",6,-0.5,5.5);
  TH1D* h_nup_mu_with_cuts_noweigt = new TH1D("nup_mu_with_cuts_noweigt","nup",6,-0.5,5.5);

 TH1D* h_nup_elec_with_cuts = new TH1D("nup_elec_with_cuts","nup",6,-0.5,5.5);
  TH1D* h_nup_mu_with_cuts = new TH1D("nup_mu_with_cuts","nup",6,-0.5,5.5);
  TH1D* h_dE_gen_raw = new TH1D("dE_gen_raw","dE_gen_raw",50,-30,30);
  TH1D* h_dE_gen_corr = new TH1D(" dE_gen_corr","dE_gen_corr",50,-30,30);
  TH1D* h_dE_gen_corr_rescorr= new TH1D(" dE_gen_corr_rescorr","dE_gen_corr_rescorr",50,-30,30);
  TH1D* h_mjjjj = new TH1D("mjjjj","M(jjjj)",100, 10.,200.);
  TH1D* h_mjjjj_BEF = new TH1D("mjjjj_BEF","M(jjjj) BEF",100, 10.,200.);
 // TH1D* h_hist_efe  = new TH1D("h_hist_efe","#Delta R (Gj1,J1)",50,0.,1.);
 // TH1D* h_hist_efee  = new TH1D("h_hist_efee","#Delta R (Gj,J)",50,0.,1.);

  TH1D* h_nvtx = new TH1D("nvtx","nvtx",40,0,40);

  TH1D* h_nvtx_noWeight = new TH1D("nvtx_noWeight","nvtx_noWeight",40,0,40);
  
  TH1D* h_Elecpt = new TH1D("Elecpt","Elecpt",200,0,1000);

  TH1D*h_Eleceta   = new TH1D("Eleceta","eleceta",50,-5,5);
  TH1D*h_Elecphi   = new TH1D("Elecphi","elecphi",50,-5,5);

  TH1D* h_MZElec = new TH1D("MZElec","M(Z)#rightarrow #mu#mu",40, 71,111.);
  TH1D* h_diElecpt = new TH1D("diElecpt","diElecpt",200,0,1000);
  TH1D* h_diElecrapidity = new TH1D("diElecrapidity","diElecrapidity",50,-2.5,2.5);
  TH1D* h_diElecphi = new TH1D("diElecphi","diElecphi",12,-3.2,3.2);

 TH1D* h_diElecpt_pc = new TH1D("diElecpt_pc","diElecpt_pc",200,0,1000);
  TH1D* h_diElecphi_pc = new TH1D("diElecphi_pc","diElecphi_pc",12,-3.2,3.2);
  TH1D* h_diElecpt_p0 = new TH1D("diElecpt_p0","diElecpt_p0",200,0,1000);
  TH1D* h_diElecphi_p0 = new TH1D("diElecphi_p0","diElecphi_p0",12,-3.2,3.2);
  TH1D* h_diElecpt_p1 = new TH1D("diElecpt_p1","diElecpt_p1",200,0,1000);
  TH1D* h_diElecphi_p1 = new TH1D("diElecphi_p1","diElecphi_p1",12,-3.2,3.2);
  TH1D* h_diElecpt_p2 = new TH1D("diElecpt_p2","diElecpt_p2",200,0,1000);
  TH1D* h_diElecphi_p2 = new TH1D("diElecphi_p2","diElecphi_p2",12,-3.2,3.2);


 TH1D* h_diElecpt_mc = new TH1D("diElecpt_mc","diElecpt_mc",200,0,1000);
  TH1D* h_diElecphi_mc = new TH1D("diElecphi_mc","diElecphi_mc",12,-3.2,3.2);
 TH1D* h_diElecpt_m0 = new TH1D("diElecpt_m0","diElecpt_m0",200,0,1000);
  TH1D* h_diElecphi_m0 = new TH1D("diElecphi_m0","diElecphi_m0",12,-3.2,3.2);
  TH1D* h_diElecpt_m1 = new TH1D("diElecpt_m1","diElecpt_m1",200,0,1000);
  TH1D* h_diElecphi_m1 = new TH1D("diElecphi_m1","diElecphi_m1",12,-3.2,3.2);
  TH1D* h_diElecpt_m2 = new TH1D("diElecpt_m2","diElecpt_m2",200,0,1000);
  TH1D* h_diElecphi_m2 = new TH1D("diElecphi_m2","diElecphi_m2",12,-3.2,3.2);


  TH1D*h_felec_pt_p0    = new TH1D("felec_pt_p0 ","felec_pt_p0",200,0,1000);
  TH1D*h_felec_eta_p0   = new TH1D("felec_eta_p0","felec_eta_p0",50,-5,5);
  TH1D*h_felec_phi_p0   = new TH1D("felec_phi_p0","felec_phi_p0",50,-5,5);
  TH1D*h_celec_pt_p0    = new TH1D("celec_pt_p0 ","celec_pt_p0",200,0,1000);
  TH1D*h_celec_eta_p0   = new TH1D("celec_eta_p0","celec_eta_p0",50,-5,5);
  TH1D*h_celec_phi_p0   = new TH1D("celec_phi_p0","celec_phi_p0",50,-5,5);

  TH1D*h_lelec_pt_p0    = new TH1D("lelec_pt_p0  ","lelec_pt_p0  ",200,0,1000);
  TH1D*h_lelec_eta_p0   = new TH1D("lelec_eta_p0 ","lelec_eta_p0 ",50,-5,5);
  TH1D*h_lelec_phi_p0   = new TH1D("lelec_phi_p0 ","lelec_phi_p0 ",50,-5,5);
  TH1D*h_slelec_pt_p0   = new TH1D("slelec_pt_p0 ","slelec_pt_p0 ",200,0,1000);
  TH1D*h_slelec_eta_p0  = new TH1D("slelec_eta_p0","slelec_eta_p0",50,-5,5);
  TH1D*h_slelec_phi_p0  = new TH1D("slelec_phi_p0","slelec_phi_p0",50,-5,5);

  TH1D*h_felec_pt_p1    = new TH1D("felec_pt_p1 ","felec_pt_p1",200,0,1000);
  TH1D*h_felec_eta_p1   = new TH1D("felec_eta_p1","felec_eta_p1",50,-5,5);
  TH1D*h_felec_phi_p1   = new TH1D("felec_phi_p1","felec_phi_p1",50,-5,5);
  TH1D*h_celec_pt_p1    = new TH1D("celec_pt_p1 ","celec_pt_p1",200,0,1000);
  TH1D*h_celec_eta_p1   = new TH1D("celec_eta_p1","celec_eta_p1",50,-5,5);
  TH1D*h_celec_phi_p1   = new TH1D("celec_phi_p1","celec_phi_p1",50,-5,5);

  TH1D*h_lelec_pt_p1    = new TH1D("lelec_pt_p1  ","lelec_pt_p1  ",200,0,1000);
  TH1D*h_lelec_eta_p1   = new TH1D("lelec_eta_p1 ","lelec_eta_p1 ",50,-5,5);
  TH1D*h_lelec_phi_p1   = new TH1D("lelec_phi_p1 ","lelec_phi_p1 ",50,-5,5);
  TH1D*h_slelec_pt_p1   = new TH1D("slelec_pt_p1 ","slelec_pt_p1 ",200,0,1000);
  TH1D*h_slelec_eta_p1  = new TH1D("slelec_eta_p1","slelec_eta_p1",50,-5,5);
  TH1D*h_slelec_phi_p1  = new TH1D("slelec_phi_p1","slelec_phi_p1",50,-5,5);

  TH1D*h_felec_pt_p2    = new TH1D("felec_pt_p2 ","felec_pt_p2",200,0,1000);
  TH1D*h_felec_eta_p2   = new TH1D("felec_eta_p2","felec_eta_p2",50,-5,5);
  TH1D*h_felec_phi_p2   = new TH1D("felec_phi_p2","felec_phi_p2",50,-5,5);
  TH1D*h_celec_pt_p2    = new TH1D("celec_pt_p2 ","celec_pt_p2",200,0,1000);
  TH1D*h_celec_eta_p2   = new TH1D("celec_eta_p2","celec_eta_p2",50,-5,5);
  TH1D*h_celec_phi_p2   = new TH1D("celec_phi_p2","celec_phi_p2",50,-5,5);

  TH1D*h_lelec_pt_p2    = new TH1D("lelec_pt_p2  ","lelec_pt_p2  ",200,0,1000);
  TH1D*h_lelec_eta_p2   = new TH1D("lelec_eta_p2 ","lelec_eta_p2 ",50,-5,5);
  TH1D*h_lelec_phi_p2   = new TH1D("lelec_phi_p2 ","lelec_phi_p2 ",50,-5,5);
  TH1D*h_slelec_pt_p2   = new TH1D("slelec_pt_p2 ","slelec_pt_p2 ",200,0,1000);
  TH1D*h_slelec_eta_p2  = new TH1D("slelec_eta_p2","slelec_eta_p2",50,-5,5);
  TH1D*h_slelec_phi_p2  = new TH1D("slelec_phi_p2","slelec_phi_p2",50,-5,5);

  TH1D*h_felec_pt_pc    = new TH1D("felec_pt_pc ","felec_pt_pc",200,0,1000);
  TH1D*h_felec_eta_pc   = new TH1D("felec_eta_pc","felec_eta_pc",50,-5,5);
  TH1D*h_felec_phi_pc   = new TH1D("felec_phi_pc","felec_phi_pc",50,-5,5);
  TH1D*h_celec_pt_pc    = new TH1D("celec_pt_pc ","celec_pt_pc",200,0,1000);
  TH1D*h_celec_eta_pc   = new TH1D("celec_eta_pc","celec_eta_pc",50,-5,5);
  TH1D*h_celec_phi_pc   = new TH1D("celec_phi_pc","celec_phi_pc",50,-5,5);

  TH1D*h_lelec_pt_pc    = new TH1D("lelec_pt_pc  ","lelec_pt_pc  ",200,0,1000);
  TH1D*h_lelec_eta_pc   = new TH1D("lelec_eta_pc ","lelec_eta_pc ",50,-5,5);
  TH1D*h_lelec_phi_pc   = new TH1D("lelec_phi_pc ","lelec_phi_pc ",50,-5,5);
  TH1D*h_slelec_pt_pc   = new TH1D("slelec_pt_pc ","slelec_pt_pc ",200,0,1000);
  TH1D*h_slelec_eta_pc  = new TH1D("slelec_eta_pc","slelec_eta_pc",50,-5,5);
  TH1D*h_slelec_phi_pc  = new TH1D("slelec_phi_pc","slelec_phi_pc",50,-5,5);

  TH1D*h_felec_pt_mc    = new TH1D("felec_pt_mc ","felec_pt_mc",200,0,1000);
  TH1D*h_felec_eta_mc   = new TH1D("felec_eta_mc","felec_eta_mc",50,-5,5);
  TH1D*h_felec_phi_mc   = new TH1D("felec_phi_mc","felec_phi_mc",50,-5,5);
  TH1D*h_celec_pt_mc    = new TH1D("celec_pt_mc ","celec_pt_mc",200,0,1000);
  TH1D*h_celec_eta_mc   = new TH1D("celec_eta_mc","celec_eta_mc",50,-5,5);
  TH1D*h_celec_phi_mc   = new TH1D("celec_phi_mc","celec_phi_mc",50,-5,5);

  TH1D*h_lelec_pt_mc    = new TH1D("lelec_pt_mc  ","lelec_pt_mc  ",200,0,1000);
  TH1D*h_lelec_eta_mc   = new TH1D("lelec_eta_mc ","lelec_eta_mc ",50,-5,5);
  TH1D*h_lelec_phi_mc   = new TH1D("lelec_phi_mc ","lelec_phi_mc ",50,-5,5);
  TH1D*h_slelec_pt_mc   = new TH1D("slelec_pt_mc ","slelec_pt_mc ",200,0,1000);
  TH1D*h_slelec_eta_mc  = new TH1D("slelec_eta_mc","slelec_eta_mc",50,-5,5);
  TH1D*h_slelec_phi_mc  = new TH1D("slelec_phi_mc","slelec_phi_mc",50,-5,5);  

  TH1D*h_felec_pt_m0    = new TH1D("felec_pt_m0 ","felec_pt_m0",200,0,1000);
  TH1D*h_felec_eta_m0   = new TH1D("felec_eta_m0","felec_eta_m0",50,-5,5);
  TH1D*h_felec_phi_m0   = new TH1D("felec_phi_m0","felec_phi_m0",50,-5,5);
  TH1D*h_celec_pt_m0    = new TH1D("celec_pt_m0 ","celec_pt_m0",200,0,1000);
  TH1D*h_celec_eta_m0   = new TH1D("celec_eta_m0","celec_eta_m0",50,-5,5);
  TH1D*h_celec_phi_m0   = new TH1D("celec_phi_m0","celec_phi_m0",50,-5,5);

  TH1D*h_lelec_pt_m0    = new TH1D("lelec_pt_m0  ","lelec_pt_m0  ",200,0,1000);
  TH1D*h_lelec_eta_m0   = new TH1D("lelec_eta_m0 ","lelec_eta_m0 ",50,-5,5);
  TH1D*h_lelec_phi_m0   = new TH1D("lelec_phi_m0 ","lelec_phi_m0 ",50,-5,5);
  TH1D*h_slelec_pt_m0   = new TH1D("slelec_pt_m0 ","slelec_pt_m0 ",200,0,1000);
  TH1D*h_slelec_eta_m0  = new TH1D("slelec_eta_m0","slelec_eta_m0",50,-5,5);
  TH1D*h_slelec_phi_m0  = new TH1D("slelec_phi_m0","slelec_phi_m0",50,-5,5);

  TH1D*h_felec_pt_m1    = new TH1D("felec_pt_m1 ","felec_pt_m1",200,0,1000);
  TH1D*h_felec_eta_m1   = new TH1D("felec_eta_m1","felec_eta_m1",50,-5,5);
  TH1D*h_felec_phi_m1   = new TH1D("felec_phi_m1","felec_phi_m1",50,-5,5);
  TH1D*h_celec_pt_m1    = new TH1D("celec_pt_m1 ","celec_pt_m1",200,0,1000);
  TH1D*h_celec_eta_m1   = new TH1D("celec_eta_m1","celec_eta_m1",50,-5,5);
  TH1D*h_celec_phi_m1   = new TH1D("celec_phi_m1","celec_phi_m1",50,-5,5);

  TH1D*h_lelec_pt_m1    = new TH1D("lelec_pt_m1  ","lelec_pt_m1  ",200,0,1000);
  TH1D*h_lelec_eta_m1   = new TH1D("lelec_eta_m1 ","lelec_eta_m1 ",50,-5,5);
  TH1D*h_lelec_phi_m1   = new TH1D("lelec_phi_m1 ","lelec_phi_m1 ",50,-5,5);
  TH1D*h_slelec_pt_m1   = new TH1D("slelec_pt_m1 ","slelec_pt_m1 ",200,0,1000);
  TH1D*h_slelec_eta_m1  = new TH1D("slelec_eta_m1","slelec_eta_m1",50,-5,5);
  TH1D*h_slelec_phi_m1  = new TH1D("slelec_phi_m1","slelec_phi_m1",50,-5,5);

  TH1D*h_felec_pt_m2    = new TH1D("felec_pt_m2 ","felec_pt_m2",200,0,1000);
  TH1D*h_felec_eta_m2   = new TH1D("felec_eta_m2","felec_eta_m2",50,-5,5);
  TH1D*h_felec_phi_m2   = new TH1D("felec_phi_m2","felec_phi_m2",50,-5,5);
  TH1D*h_celec_pt_m2    = new TH1D("celec_pt_m2 ","celec_pt_m2",200,0,1000);
  TH1D*h_celec_eta_m2   = new TH1D("celec_eta_m2","celec_eta_m2",50,-5,5);
  TH1D*h_celec_phi_m2   = new TH1D("celec_phi_m2","celec_phi_m2",50,-5,5);

  TH1D*h_lelec_pt_m2    = new TH1D("lelec_pt_m2  ","lelec_pt_m2  ",200,0,1000);
  TH1D*h_lelec_eta_m2   = new TH1D("lelec_eta_m2 ","lelec_eta_m2 ",50,-5,5);
  TH1D*h_lelec_phi_m2   = new TH1D("lelec_phi_m2 ","lelec_phi_m2 ",50,-5,5);
  TH1D*h_slelec_pt_m2   = new TH1D("slelec_pt_m2 ","slelec_pt_m2 ",200,0,1000);
  TH1D*h_slelec_eta_m2  = new TH1D("slelec_eta_m2","slelec_eta_m2",50,-5,5);
  TH1D*h_slelec_phi_m2  = new TH1D("slelec_phi_m2","slelec_phi_m2",50,-5,5);



  TH1D* h_GMZElec = new TH1D("GMZElec","M(Z)#rightarrow #mu#mu",40, 71,111.);
  TH1D* h_GdiElecpt = new TH1D("GdiElecpt","diElecpt",200,0,1000);
  TH1D* h_GdiElecrapidity = new TH1D("GdiElecrapidity","diElecrapidity",50,-2.5,2.5);
  TH1D* h_GdiElecphi = new TH1D("GdiElecphi","diElecphi",12,-3.2,3.2);
 TH1D* h_GMZElec_mu = new TH1D("GMZElec_mu","M(Z)#rightarrow #mu#mu",40, 71,111.);
  TH1D* h_GdiElecpt_mu = new TH1D("GdiElecpt_mu","diElecpt",200,0,1000);
  TH1D* h_GdiElecrapidity_mu = new TH1D("GdiElecrapidity_mu","diElecrapidity",50,-2.5,2.5);
  TH1D* h_GdiElecphi_mu = new TH1D("GdiElecphi_mu","diElecphi",12,-3.2,3.2);
 TH1D* h_GMZElec_st3mu = new TH1D("GMZElec_st3mu","M(Z)#rightarrow #mu#mu",40, 71,111.);
  TH1D* h_GdiElecpt_st3mu = new TH1D("GdiElecpt_st3mu","diElecpt",200,0,1000);
  TH1D* h_GdiElecrapidity_st3mu = new TH1D("GdiElecrapidity_st3mu","diElecrapidity",50,-2.5,2.5);
  TH1D* h_GdiElecphi_st3mu = new TH1D("GdiElecphi_st3mu","diElecphi",12,-3.2,3.2);
 TH1D* h_GMZElec_st3e = new TH1D("GMZElec_st3e","M(Z)#rightarrow #mu#mu",40, 71,111.);
  TH1D* h_GdiElecpt_st3e = new TH1D("GdiElecpt_st3e","diElecpt",200,0,1000);
  TH1D* h_GdiElecrapidity_st3e = new TH1D("GdiElecrapidity_st3e","diElecrapidity",50,-2.5,2.5);
  TH1D* h_GdiElecphi_st3e = new TH1D("GdiElecphi_st3e","diElecphi",12,-3.2,3.2);


  TH1D *h_ljet_pt_y[100];
  TH1D *h_ljet_y_pt[100];
  TH1D *h_ljet_pt_y_z0[100];
  TH1D *h_ljet_pt_y_z1[100];
  TH1D *h_ljet_y_pt_z0[100];
  TH1D *h_ljet_y_pt_z1[100];

  TH1D *h_ljet_pt_y_Gen[100];
  TH1D *h_ljet_y_pt_Gen[100];
  TH1D *h_ljet_pt_y_z0_Gen[100];
  TH1D *h_ljet_pt_y_z1_Gen[100];
  TH1D *h_ljet_y_pt_z0_Gen[100];
  TH1D *h_ljet_y_pt_z1_Gen[100];


  TH2* h_ljet_pt_y_Gen_VS_reco[100];
  TH2* h_ljet_y_pt_Gen_VS_reco[100];
  TH2* h_ljet_pt_y_z0_Gen_VS_reco[100];
  TH2* h_ljet_pt_y_z1_Gen_VS_reco[100];
  TH2* h_ljet_y_pt_z0_Gen_VS_reco[100];
  TH2* h_ljet_y_pt_z1_Gen_VS_reco[100];


  TH1D *h_inc_ljet_pt_y[100];
  TH1D *h_inc_ljet_pt_y_fb[100];
  TH1D *h_inc_ljet_pt_y_fb_z0[100];
  TH1D *h_inc_ljet_pt_y_fb_z1[100];
  TH1D *h_inc_ljet_pt_sljetpt[100];
  TH1D *h_inc_ljet_y_pt[100];
  TH1D *h_inc_ljet_pt_y_z0[100];
  TH1D *h_inc_ljet_pt_y_z1[100];

  TH1D *h_inc_ljet_pt_sljetpt_z0[100];
  TH1D *h_inc_ljet_pt_sljetpt_z1[100];
  TH1D *h_inc_ljet_y_pt_z0[100];
  TH1D *h_inc_ljet_y_pt_z1[100];

  TH1D *h_inc_ljet_pt_y_Gen[100];
  TH1D *h_inc_ljet_pt_sljetpt_Gen[100];
  TH1D *h_inc_ljet_y_pt_Gen[100];
  TH1D *h_inc_ljet_pt_y_z0_Gen[100];
  TH1D *h_inc_ljet_pt_y_z1_Gen[100];
  TH1D *h_inc_ljet_pt_sljetpt_z0_Gen[100];
  TH1D *h_inc_ljet_pt_sljetpt_z1_Gen[100];
  TH1D *h_inc_ljet_y_pt_z0_Gen[100];
  TH1D *h_inc_ljet_y_pt_z1_Gen[100];
  TH1D *h_inc_ljet_pt_y_fb_Gen[100];
  TH1D *h_inc_ljet_pt_y_fb_z0_Gen[100];
  TH1D *h_inc_ljet_pt_y_fb_z1_Gen[100];

  TH2* h_inc_ljet_pt_y_Gen_VS_reco[100];
  TH2 *h_inc_ljet_pt_sljetpt_Gen_VS_reco[100];
  TH2* h_inc_ljet_y_pt_Gen_VS_reco[100];
  TH2* h_inc_ljet_pt_y_z0_Gen_VS_reco[100];
  TH2* h_inc_ljet_pt_y_z1_Gen_VS_reco[100];
  TH2 *h_inc_ljet_pt_sljetpt_z0_Gen_VS_reco[100];
  TH2 *h_inc_ljet_pt_sljetpt_z1_Gen_VS_reco[100];
  TH2* h_inc_ljet_y_pt_z0_Gen_VS_reco[100];
  TH2* h_inc_ljet_y_pt_z1_Gen_VS_reco[100];
  TH2 *h_inc_ljet_pt_y_fb_Gen_VS_reco[100];
  TH2 *h_inc_ljet_pt_y_fb_z0_Gen_VS_reco[100];
  TH2 *h_inc_ljet_pt_y_fb_z1_Gen_VS_reco[100];

  TH1D *h_inc_sljet_pt_y[100];
  TH1D *h_inc_sljet_y_pt[100];
  TH1D *h_inc_sljet_pt_y_z0[100];
  TH1D *h_inc_sljet_pt_y_z1[100];
  TH1D *h_inc_sljet_y_pt_z0[100];
  TH1D *h_inc_sljet_y_pt_z1[100];

  TH1D *h_inc_sljet_pt_y_Gen[100];
  TH1D *h_inc_sljet_y_pt_Gen[100];
  TH1D *h_inc_sljet_pt_y_z0_Gen[100];
  TH1D *h_inc_sljet_pt_y_z1_Gen[100];
  TH1D *h_inc_sljet_y_pt_z0_Gen[100];
  TH1D *h_inc_sljet_y_pt_z1_Gen[100];


  TH2* h_inc_sljet_pt_y_Gen_VS_reco[100];
  TH2* h_inc_sljet_y_pt_Gen_VS_reco[100];
  TH2* h_inc_sljet_pt_y_z0_Gen_VS_reco[100];
  TH2* h_inc_sljet_pt_y_z1_Gen_VS_reco[100];
  TH2* h_inc_sljet_y_pt_z0_Gen_VS_reco[100];
  TH2* h_inc_sljet_y_pt_z1_Gen_VS_reco[100];

  TH1D *h_z_pt_y[100][100];
  TH1D *h_z_y_pt[100][100];
  TH1D *h_z_pt_y_Gen[100][100];
  TH1D *h_z_y_pt_Gen[100][100];
  TH2* h_z_pt_y_Gen_VS_reco[100][100];
  TH2* h_z_y_pt_Gen_VS_reco[100][100];

  char name[100];


  double j_pT_range[9]={30.,50.,70.,100.,130.,170.,220.,350.,1000.};
  double j_pT_range2[21] = { 30, 39, 49, 60, 72, 85, 100, 117, 136, 157, 187, 220, 258, 300, 350, 400, 450, 500, 590, 700, 1000};

  double j_Y_range[8]={0.,0.5,1.0,1.5,2.0,2.5,3.2,4.7};

  double Z_pT_range[6]={20.,30.,50.,100.,200.,1000.};
  double Z_Y_range[6]={0.,0.5,1.0,1.5,2.0,2.5};
 //   Z_y_range[5]={0,0.9,1.5,2.1,2.5};//
 //   threed_j_y_range[4]={0,1.5,2.4,4.7};//
 // pt_range[6]={20.,30.,50.,100.,200.,1000.};//
for(int i = 0; i<5;i++){//Z rapidity range
  for(int j = 0; j<3;j++){//jet rapidity range
//Z PT vrt jet 
    sprintf(name,"Z_pt_y_%i_%i",i,j);
    h_z_pt_y[i][j] = new TH1D (name,name, 5, Z_pT_range);
    sprintf(name,"Z_y_pt_%i_%i",i,j);
    h_z_y_pt[i][j] = new TH1D (name,name,5, Z_Y_range);
  
    sprintf(name,"Z_pt_y_Gen_%i_%i",i,j);
    h_z_pt_y_Gen[i][j] = new TH1D (name,name, 5, Z_pT_range);
    sprintf(name,"Z_y_pt_Gen_%i_%i",i,j);
    h_z_y_pt_Gen[i][j] = new TH1D (name,name, 5, Z_Y_range);

    sprintf(name,"Z_pt_y_Gen_VS_reco_%i_%i",i,j);
    h_z_pt_y_Gen_VS_reco[i][j] = new TH2D (name,name, 5, Z_pT_range, 5, Z_pT_range);
    sprintf(name,"Z_y_pt_Gen_VS_reco_%i_%i",i,j);
    h_z_y_pt_Gen_VS_reco[i][j] = new TH2D (name,name, 5, Z_Y_range, 5, Z_Y_range);
     
  }
}
for(int i = 0; i<9;i++){//j rapidity range

  sprintf(name,"ljet_pt_y_%i",i);
  h_ljet_pt_y[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"ljet_y_pt_%i",i);
  h_ljet_y_pt[i] = new TH1D (name,name, 7,j_Y_range);
  sprintf(name,"ljet_pt_y_z0_%i",i);
  h_ljet_pt_y_z0[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"ljet_pt_y_z1_%i",i);
  h_ljet_pt_y_z1[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"ljet_y_pt_z0_%i",i);
  h_ljet_y_pt_z0[i] = new TH1D (name,name, 7,j_Y_range);
  sprintf(name,"ljet_y_pt_z1_%i",i);
  h_ljet_y_pt_z1[i] = new TH1D (name,name, 7,j_Y_range);


  sprintf(name,"ljet_pt_y_Gen_%i",i);
  h_ljet_pt_y_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"ljet_y_pt_Gen_%i",i);
  h_ljet_y_pt_Gen[i] = new TH1D(name,name,7,j_Y_range);
  sprintf(name,"ljet_pt_y_Gen_z0_%i",i);
  h_ljet_pt_y_z0_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"ljet_pt_y_Gen_z1_%i",i);
  h_ljet_pt_y_z1_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"ljet_y_pt_Gen_z0_%i",i);
  h_ljet_y_pt_z0_Gen[i] = new TH1D(name,name,7,j_Y_range);
  sprintf(name,"ljet_y_pt_Gen_z1_%i",i);
  h_ljet_y_pt_z1_Gen[i] = new TH1D(name,name,7,j_Y_range);

  sprintf(name,"ljet_pt_y_Gen_VS_reco_%i",i);
  h_ljet_pt_y_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"ljet_y_pt_Gen_VS_reco_%i",i);
  h_ljet_y_pt_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);
  sprintf(name,"ljet_pt_y_Gen_VS_reco_z0_%i",i);
  h_ljet_pt_y_z0_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"ljet_pt_y_Gen_VS_reco_z1_%i",i);
  h_ljet_pt_y_z1_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"ljet_y_pt_Gen_VS_reco_z0_%i",i);
  h_ljet_y_pt_z0_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);
  sprintf(name,"ljet_y_pt_Gen_VS_reco_z1_%i",i);
  h_ljet_y_pt_z1_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);

  sprintf(name,"inc_ljet_pt_y_%i",i);
  h_inc_ljet_pt_y[i] = new TH1D (name,name, 8, j_pT_range);

  sprintf(name,"inc_ljet_pt_y_fb_%i",i);
  h_inc_ljet_pt_y_fb[i] = new TH1D (name,name, 20, j_pT_range2);

  sprintf(name,"inc_ljet_y_pt_%i",i);
  h_inc_ljet_y_pt[i] = new TH1D (name,name, 7,j_Y_range);
  sprintf(name,"inc_ljet_pt_y_z0_%i",i);
  h_inc_ljet_pt_y_z0[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_ljet_pt_y_z1_%i",i);
  h_inc_ljet_pt_y_z1[i] = new TH1D (name,name, 8, j_pT_range);

  sprintf(name,"inc_ljet_pt_y_fb_z0_%i",i);
  h_inc_ljet_pt_y_fb_z0[i] = new TH1D (name,name, 20, j_pT_range2);
  sprintf(name,"inc_ljet_pt_y_fb_z1_%i",i);
  h_inc_ljet_pt_y_fb_z1[i] = new TH1D (name,name, 20, j_pT_range2);


  sprintf(name,"inc_ljet_pt_sljetpt_%i",i);
  h_inc_ljet_pt_sljetpt[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_ljet_pt_sljetpt_z0_%i",i);
  h_inc_ljet_pt_sljetpt_z0[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_ljet_pt_sljetpt_z1_%i",i);
  h_inc_ljet_pt_sljetpt_z1[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_ljet_y_pt_z0_%i",i);
  h_inc_ljet_y_pt_z0[i] = new TH1D (name,name, 7,j_Y_range);
  sprintf(name,"inc_ljet_y_pt_z1_%i",i);
  h_inc_ljet_y_pt_z1[i] = new TH1D (name,name, 7,j_Y_range);


  sprintf(name,"inc_ljet_pt_y_Gen_%i",i);
  h_inc_ljet_pt_y_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"inc_ljet_pt_y_fb_Gen_%i",i);
  h_inc_ljet_pt_y_fb_Gen[i] = new TH1D(name,name,20,j_pT_range2);

  sprintf(name,"inc_ljet_y_pt_Gen_%i",i);
  h_inc_ljet_y_pt_Gen[i] = new TH1D(name,name,7,j_Y_range);
  sprintf(name,"inc_ljet_pt_y_Gen_z0_%i",i);
  h_inc_ljet_pt_y_z0_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"inc_ljet_pt_y_Gen_z1_%i",i);
  h_inc_ljet_pt_y_z1_Gen[i] = new TH1D(name,name,8,j_pT_range);

  sprintf(name,"inc_ljet_pt_y_fb_Gen_z0_%i",i);
  h_inc_ljet_pt_y_fb_z0_Gen[i] = new TH1D(name,name,20,j_pT_range2);
  sprintf(name,"inc_ljet_pt_y_fb_Gen_z1_%i",i);
  h_inc_ljet_pt_y_fb_z1_Gen[i] = new TH1D(name,name,20,j_pT_range2);

  sprintf(name,"inc_ljet_pt_sljetpt_Gen_%i",i);
  h_inc_ljet_pt_sljetpt_Gen[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_ljet_pt_sljetpt_Gen_z0_%i",i);
  h_inc_ljet_pt_sljetpt_z0_Gen[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_ljet_pt_sljetpt_Gen_z1_%i",i);
  h_inc_ljet_pt_sljetpt_z1_Gen[i] = new TH1D (name,name, 8, j_pT_range);

  sprintf(name,"inc_ljet_y_pt_Gen_z0_%i",i);
  h_inc_ljet_y_pt_z0_Gen[i] = new TH1D(name,name,7,j_Y_range);
  sprintf(name,"inc_ljet_y_pt_Gen_z1_%i",i);
  h_inc_ljet_y_pt_z1_Gen[i] = new TH1D(name,name,7,j_Y_range);


  sprintf(name,"inc_ljet_y_pt_Gen_VS_reco_%i",i);
  h_inc_ljet_y_pt_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);
  sprintf(name,"inc_ljet_pt_y_Gen_VS_reco_%i",i);
  h_inc_ljet_pt_y_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_ljet_pt_y_Gen_VS_reco_z0_%i",i);
  h_inc_ljet_pt_y_z0_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_ljet_pt_y_Gen_VS_reco_z1_%i",i);
  h_inc_ljet_pt_y_z1_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);

  sprintf(name,"inc_ljet_pt_y_fb_Gen_VS_reco_%i",i);
  h_inc_ljet_pt_y_fb_Gen_VS_reco[i] = new TH2D(name,name,20,j_pT_range2,20,j_pT_range2);
  sprintf(name,"inc_ljet_pt_y_fb_Gen_VS_reco_z0_%i",i);
  h_inc_ljet_pt_y_fb_z0_Gen_VS_reco[i] = new TH2D(name,name,20,j_pT_range2,20,j_pT_range2);
  sprintf(name,"inc_ljet_pt_y_fb_Gen_VS_reco_z1_%i",i);
  h_inc_ljet_pt_y_fb_z1_Gen_VS_reco[i] = new TH2D(name,name,20,j_pT_range2,20,j_pT_range2);

  sprintf(name,"inc_ljet_pt_sljetpt_Gen_VS_reco_%i",i);
  h_inc_ljet_pt_sljetpt_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_ljet_pt_sljetpt_Gen_VS_reco_z0_%i",i);
  h_inc_ljet_pt_sljetpt_z0_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_ljet_pt_sljetpt_Gen_VS_reco_z1_%i",i);
  h_inc_ljet_pt_sljetpt_z1_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_ljet_y_pt_Gen_VS_reco_z0_%i",i);

  sprintf(name,"inc_ljet_y_pt_Gen_VS_reco_z0_%i",i);
  h_inc_ljet_y_pt_z0_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);
  sprintf(name,"inc_ljet_y_pt_Gen_VS_reco_z1_%i",i);
  h_inc_ljet_y_pt_z1_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);

sprintf(name,"inc_sljet_pt_y_%i",i);
  h_inc_sljet_pt_y[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_sljet_y_pt_%i",i);
  h_inc_sljet_y_pt[i] = new TH1D (name,name, 7,j_Y_range);
  sprintf(name,"inc_sljet_pt_y_z0_%i",i);
  h_inc_sljet_pt_y_z0[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_sljet_pt_y_z1_%i",i);
  h_inc_sljet_pt_y_z1[i] = new TH1D (name,name, 8, j_pT_range);
  sprintf(name,"inc_sljet_y_pt_z0_%i",i);
  h_inc_sljet_y_pt_z0[i] = new TH1D (name,name, 7,j_Y_range);
  sprintf(name,"inc_sljet_y_pt_z1_%i",i);
  h_inc_sljet_y_pt_z1[i] = new TH1D (name,name, 7,j_Y_range);


  sprintf(name,"inc_sljet_pt_y_Gen_%i",i);
  h_inc_sljet_pt_y_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"inc_sljet_y_pt_Gen_%i",i);
  h_inc_sljet_y_pt_Gen[i] = new TH1D(name,name,7,j_Y_range);
  sprintf(name,"inc_sljet_pt_y_Gen_z0_%i",i);
  h_inc_sljet_pt_y_z0_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"inc_sljet_pt_y_Gen_z1_%i",i);
  h_inc_sljet_pt_y_z1_Gen[i] = new TH1D(name,name,8,j_pT_range);
  sprintf(name,"inc_sljet_y_pt_Gen_z0_%i",i);
  h_inc_sljet_y_pt_z0_Gen[i] = new TH1D(name,name,7,j_Y_range);
  sprintf(name,"inc_sljet_y_pt_Gen_z1_%i",i);
  h_inc_sljet_y_pt_z1_Gen[i] = new TH1D(name,name,7,j_Y_range);

  sprintf(name,"inc_sljet_pt_y_Gen_VS_reco_%i",i);
  h_inc_sljet_pt_y_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_sljet_y_pt_Gen_VS_reco_%i",i);
  h_inc_sljet_y_pt_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);
  sprintf(name,"inc_sljet_pt_y_Gen_VS_reco_z0_%i",i);
  h_inc_sljet_pt_y_z0_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_sljet_pt_y_Gen_VS_reco_z1_%i",i);
  h_inc_sljet_pt_y_z1_Gen_VS_reco[i] = new TH2D(name,name,8,j_pT_range,8,j_pT_range);
  sprintf(name,"inc_sljet_y_pt_Gen_VS_reco_z0_%i",i);
  h_inc_sljet_y_pt_z0_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);
  sprintf(name,"inc_sljet_y_pt_Gen_VS_reco_z1_%i",i);
  h_inc_sljet_y_pt_z1_Gen_VS_reco[i] = new TH2D(name,name,7,j_Y_range,7,j_Y_range);

}

//X-Check histograms
  double n_jet_range[10]={-0.5,0.5,1.5,2.5,3.5,4.5,5.5,6.5,7.5,8.5};

  TH1D* h_n_jet_30_unfold  = new TH1D("n_jet_30_unfold","n_jet_30_unfold",9,n_jet_range);
  TH1D* h_n_jet_30_gen  = new TH1D("n_jet_30_gen","n_jet_30_gen",9,n_jet_range);
  TH2* h_n_jet_30_Gen_VS_reco = new TH2D("n_jet_30_Gen_VS_reco","n_jet_30_Gen_VS_reco",9,n_jet_range,9,n_jet_range);

  TH1D* h_n_jet_30_unselected = new TH1D("n_jet_30_unselected","n_jet_30_unselected",9,n_jet_range);
//  TH2* h_jetpt_Gen_VS_reco= new TH2D("jetpt_Gen_VS_reco","jetpt_Gen_VS_reco",17,j_pT_range_unf,17,j_pT_range_unf);
//  TH2* h_jetY_Gen_VS_reco= new TH2D("h_jetY_Gen_VS_reco","h_jetY_Gen_VS_reco",25,-2.5,2.5,25,-2.5,2.5);
 TH1D* h_1jetpt  = new TH1D("jetpt_1","1jetpt",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  h_1jetpt->Sumw2(); 
  TH1D* h_2jetpt  = new TH1D("jetpt_2","2jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_2jetpt->Sumw2();
  //TH1D* h_jetY = new TH1D("jetY","jetY",24,-2.4,2.4);
  //h_jetY->Sumw2();
  TH1D* h_1jetY  = new TH1D("jetY_1","1jetY",24,-2.4,2.4);
  h_1jetY->Sumw2();
  TH1D* h_2jetY  = new TH1D("jetY_2","2jetY",24,-2.4,2.4);
  h_2jetY->Sumw2();

  //TH1D* h_Gjetpt  = new TH1D("Gjetpt","Gjetpt",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  //h_Gjetpt->Sumw2();
  TH1D* h_1Gjetpt  = new TH1D("Gjetpt_1","G1jetpt",nJetPt_Zinc1jet,jetPt_Zinc1jet);
  h_1Gjetpt->Sumw2(); 
  TH1D* h_2Gjetpt  = new TH1D("Gjetpt_2","G2jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_2Gjetpt->Sumw2();
  //TH1D* h_GjetY = new TH1D("GjetY","GjetY",24,-2.4,2.4);
  //h_GjetY->Sumw2();
  TH1D* h_1GjetY  = new TH1D("GjetY_1","G1jetY",24,-2.4,2.4);
  h_1GjetY->Sumw2();
  TH1D* h_2GjetY  = new TH1D("GjetY_2","G2jetY",24,-2.4,2.4);
  h_2GjetY->Sumw2();

 TH1D* h_3Gjetpt  = new TH1D("Gjetpt_3","G3jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_3Gjetpt->Sumw2();
  TH1D* h_4Gjetpt  = new TH1D("Gjetpt_4","G4jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_4Gjetpt->Sumw2();
  TH1D* h_3GjetY  = new TH1D("GjetY_3","G3jetY",24,-2.4,2.4);
  h_3GjetY->Sumw2();
  TH1D* h_4GjetY  = new TH1D("GjetY_4","G4jetY",24,-2.4,2.4);
  h_4GjetY->Sumw2();

  TH1D* h_5Gjetpt  = new TH1D("Gjetpt_5","G5jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_5Gjetpt->Sumw2();
  TH1D* h_6Gjetpt  = new TH1D("Gjetpt_6","G6jetpt",nJetPt_Zinc2jet,jetPt_Zinc2jet);
  h_6Gjetpt->Sumw2();
  TH1D* h_5GjetY  = new TH1D("GjetY_5","G5jetY",24,-2.4,2.4);
  h_5GjetY->Sumw2();
  TH1D* h_6GjetY  = new TH1D("GjetY_6","G6jetY",24,-2.4,2.4);
  h_6GjetY->Sumw2();



  //TH2* h_jetpt_Gen_VS_reco= new TH2D("jetpt_Gen_VS_reco","jetpt_Gen_VS_reco",nJetPt_Zinc1jet,jetPt_Zinc1jet,nJetPt_Zinc1jet,jetPt_Zinc1jet);
  //TH2* h_jetY_Gen_VS_reco= new TH2D("h_jetY_Gen_VS_reco","h_jetY_Gen_VS_reco",24,-2.4,2.4,24,-2.4,2.4);
  TH2* h_1jetpt_Gen_VS_reco= new TH2D("jet1pt_Gen_VS_reco","jet1pt_Gen_VS_reco",nJetPt_Zinc1jet,jetPt_Zinc1jet,nJetPt_Zinc1jet,jetPt_Zinc1jet);
  TH2* h_1jetY_Gen_VS_reco = new TH2D("jet1Y_Gen_VS_reco","jet1Y_Gen_VS_reco",24,-2.4,2.4,24,-2.4,2.4);
  TH2* h_2jetpt_Gen_VS_reco= new TH2D("jet2pt_Gen_VS_reco","2jet2pt_Gen_VS_reco",nJetPt_Zinc2jet,jetPt_Zinc2jet,nJetPt_Zinc2jet,jetPt_Zinc2jet);
  TH2* h_2jetY_Gen_VS_reco= new TH2D("jet2Y_Gen_VS_reco","jet2Y_Gen_VS_reco",24,-2.4,2.4,24,-2.4,2.4);


