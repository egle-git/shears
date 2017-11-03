    int n_jet_30=0;
    int n_jet_30BE=0;
    int n_jet_30B=0;
    int n_jet_30F=0; 
    int n_jet_50=0;
    int n_jet_50BE=0;
    int n_jet_50B=0;
    int n_jet_50F=0;
      std::vector<double>  patJetNoElecPfAk05Pt_;
      std::vector<double>  patJetNoElecPfAk05Eta_;
      std::vector<double>  patJetNoElecPfAk05Phi_;
      std::vector<double>  patJetNoElecPfAk05En_;
      for (unsigned int pf=0;pf < patJetPfAk05Pt_->size();pf++){
      if (patJetPfAk05LooseId_->at(pf)>=3 && patJetPfAk05jetpukTight_->at(pf)==1 ){
        bool markpfjet = false;
        for (unsigned int j = 0; j < patElecPt_->size(); ++j){  
          if ( patElecPt_->at(j) > 20. &&
	    fabs(patElecEta_->at(j)) <2.4 && 
            (
             !(fabs(patElecScEta_->at(j)) >1.4442 && fabs(patElecScEta_->at(j))<1.566) 
            ) &&
            patElecTrig_->at(j) > 0 &&
            patElecMediumIDOff_->at(j) >0 
	    ){
 	     float deltar = DeltaR(patJetPfAk05Eta_->at(pf),patElecEta_->at(j),patJetPfAk05Phi_->at(pf),patElecPhi_->at(j));
             h_deltar->Fill(deltar,weight);
             if (deltar<0.5) markpfjet = true;
          }
        }
           for (unsigned int j = 0; j < patMuonPt_->size(); ++j){  

           if( patMuonPt_->at(j) > 20. && 
	    fabs(patMuonEta_->at(j)) <2.4  &&
            patMuonTrig_->at(j) ==1   &&
            patMuonPfIsoDbeta_->at(j)<0.2  &&
	    patMuonCombId_->at(j) ==1){
              double deltar = DeltaR(patJetPfAk05Eta_->at(pf),patMuonEta_->at(j),patJetPfAk05Phi_->at(pf),patMuonPhi_->at(j));
              if(deltar<0.5) markpfjet = true;
            }
          }


        if (markpfjet) continue;

        if(patJetPfAk05Pt_->at(pf) > 30.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
          for(int i = 0; i<9;i++){
            if(fabs(patJetPfAk05Eta_->at(pf))>j_y_range[i] && fabs(patJetPfAk05Eta_->at(pf))<j_y_range[i+1]){
              h_jet_pt_y[i]->Fill(patJetPfAk05Pt_->at(pf),weight);

              if(fabs(diElecrapidity)<1.5) h_jet_pt_y_z0[i]->Fill(patJetPfAk05Pt_->at(pf),weight);
              if(fabs(diElecrapidity)>1.5) h_jet_pt_y_z1[i]->Fill(patJetPfAk05Pt_->at(pf),weight);

            }

            if(patJetPfAk05Pt_->at(pf)>pt_range[i] && patJetPfAk05Pt_->at(pf)<pt_range[i+1]){
              h_jet_y_pt[i]->Fill(fabs(patJetPfAk05Eta_->at(pf)),weight);
              if(fabs(diElecrapidity)<1.5) h_jet_y_pt_z0[i]->Fill(fabs(patJetPfAk05Eta_->at(pf)),weight);
              if(fabs(diElecrapidity)>1.5) h_jet_y_pt_z1[i]->Fill(fabs(patJetPfAk05Eta_->at(pf)),weight);

            }
          }
        }
        

        if(patJetPfAk05Pt_->at(pf) > 30.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<2.4){
//          cout<<patJetPfAk05Eta_->at(pf)<<"  "<<j.Rapidity()<<endl;

//        cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_30BE->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_30BE->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_30BE->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_30BE->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_30BE->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 30.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
//        cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_30BEF->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_30BEF->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_30BEF->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_30BEF->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_30BEF->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 30.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<1.5){
//          cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_30B->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_30B->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_30B->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_30B->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_30B->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 30.&&2.4<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
//          cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_30F->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_30F->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_30F->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_30F->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_30F->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 50.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<2.4){
//          cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_50BE->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_50BE->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_50BE->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_50BE->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_50BE->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 50.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
//          cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_50BEF->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_50BEF->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_50BEF->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_50BEF->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_50BEF->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 50.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<1.5){
//          cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_50B->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_50B->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_50B->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_50B->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_50B->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }

        if(patJetPfAk05Pt_->at(pf) > 50.&&2.4<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
//          cout<<patJetPfAk05jetBeta_->at(pf)<<"  "<<patJetPfAk05jetBetaStar_->at(pf)<<endl;
          h_beta_50F->Fill(patJetPfAk05jetBeta_->at(pf),weight);
          h_betastar_50F->Fill(patJetPfAk05jetBetaStar_->at(pf),weight);
          h_jet_mva_50F->Fill(patJetPfAk05jetpuMVA_->at(pf),weight);
          h_jet_mva_beta_50F->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBeta_->at(pf),weight);
          h_jet_mva_betastar_50F->Fill(patJetPfAk05jetpuMVA_->at(pf),patJetPfAk05jetBetaStar_->at(pf),weight);
        }


        patJetNoElecPfAk05Pt_.push_back(patJetPfAk05Pt_->at(pf));
        patJetNoElecPfAk05Phi_.push_back(patJetPfAk05Phi_->at(pf));
        patJetNoElecPfAk05Eta_.push_back(patJetPfAk05Eta_->at(pf));
        patJetNoElecPfAk05En_.push_back(patJetPfAk05En_->at(pf));

	if (patJetPfAk05Pt_->at(pf) > 30. && fabs(patJetPfAk05Eta_->at(pf))<4.7){             
          ++n_jet_30;

          if(n_jet_30>0) h_inc_jetpt_1_30_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);
          if(n_jet_30>1) h_inc_jetpt_2_30_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);
          if(n_jet_30>2) h_inc_jetpt_3_30_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);
          if(n_jet_30>3) h_inc_jetpt_4_30_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);

          if(n_jet_30>0) h_inc_jeteta_1_30_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);
          if(n_jet_30>1) h_inc_jeteta_2_30_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);
          if(n_jet_30>2) h_inc_jeteta_3_30_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);
          if(n_jet_30>3) h_inc_jeteta_4_30_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);


          if(0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<2.4){		
	    ++n_jet_30BE;
            if(n_jet_30BE>0) h_inc_jetpt_1_30_BE->Fill(patJetPfAk05Pt_->at(pf),weight);
            if(n_jet_30BE>1) h_inc_jetpt_2_30_BE->Fill(patJetPfAk05Pt_->at(pf),weight);
            if(n_jet_30BE>2) h_inc_jetpt_3_30_BE->Fill(patJetPfAk05Pt_->at(pf),weight);
            if(n_jet_30BE>3) h_inc_jetpt_4_30_BE->Fill(patJetPfAk05Pt_->at(pf),weight);

            if(n_jet_30BE>0) h_inc_jeteta_1_30_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
            if(n_jet_30BE>1) h_inc_jeteta_2_30_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
            if(n_jet_30BE>2) h_inc_jeteta_3_30_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
            if(n_jet_30BE>3) h_inc_jeteta_4_30_BE->Fill(patJetPfAk05Eta_->at(pf),weight);

	  }
          if(0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<1.5){		
	    ++n_jet_30B;
	  }
          if(2.4<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
            ++n_jet_30F;
          }
        }


        if (patJetPfAk05Pt_->at(pf) > 50. && fabs(patJetPfAk05Eta_->at(pf))<4.7){             
          ++n_jet_50;

          if(n_jet_50>0) h_inc_jetpt_1_50_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);
          if(n_jet_50>1) h_inc_jetpt_2_50_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);
          if(n_jet_50>2) h_inc_jetpt_3_50_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);
          if(n_jet_50>3) h_inc_jetpt_4_50_BEF->Fill(patJetPfAk05Pt_->at(pf),weight);

          if(n_jet_50>0) h_inc_jeteta_1_50_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);
          if(n_jet_50>1) h_inc_jeteta_2_50_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);
          if(n_jet_50>2) h_inc_jeteta_3_50_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);
          if(n_jet_50>3) h_inc_jeteta_4_50_BEF->Fill(patJetPfAk05Eta_->at(pf),weight);

          if(0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<2.4){		
	    ++n_jet_50BE;
            if(n_jet_50BE>0) h_inc_jetpt_1_50_BE->Fill(patJetPfAk05Pt_->at(pf),weight);
            if(n_jet_50BE>1) h_inc_jetpt_2_50_BE->Fill(patJetPfAk05Pt_->at(pf),weight);
            if(n_jet_50BE>2) h_inc_jetpt_3_50_BE->Fill(patJetPfAk05Pt_->at(pf),weight);
            if(n_jet_50BE>3) h_inc_jetpt_4_50_BE->Fill(patJetPfAk05Pt_->at(pf),weight);

            if(n_jet_50BE>0) h_inc_jeteta_1_50_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
            if(n_jet_50BE>1) h_inc_jeteta_2_50_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
            if(n_jet_50BE>2) h_inc_jeteta_3_50_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
            if(n_jet_50BE>3) h_inc_jeteta_4_50_BE->Fill(patJetPfAk05Eta_->at(pf),weight);
	  }
         if(0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<1.5){		
	    ++n_jet_50B;
	  }
          if(2.4<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7){
            ++n_jet_50F;
          }
        }
      }
      }

    //INCLUSIVE JET PT IN FORWARD VETOING CENTRAL JET ACTIVITY//
      if(n_jet_30BE ==0){
          for (int i=0; i<n_jet_30F;i++){
            if(n_jet_30F>0) h_inc_jetpt_1_30_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
            if(n_jet_30F>1) h_inc_jetpt_2_30_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
            if(n_jet_30F>2) h_inc_jetpt_3_30_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
            if(n_jet_30F>3) h_inc_jetpt_4_30_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
        
            if(n_jet_30F>0) h_inc_jeteta_1_30_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
            if(n_jet_30F>1) h_inc_jeteta_2_30_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
            if(n_jet_30F>2) h_inc_jeteta_3_30_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
            if(n_jet_30F>3) h_inc_jeteta_4_30_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
          }
        }

      if(n_jet_50BE ==0){
        for (int i=0; i<n_jet_50F;i++){
          if(n_jet_50F>0) h_inc_jetpt_1_50_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
          if(n_jet_50F>1) h_inc_jetpt_2_50_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
          if(n_jet_50F>2) h_inc_jetpt_3_50_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
          if(n_jet_50F>3) h_inc_jetpt_4_50_F->Fill(patJetNoElecPfAk05Pt_[i],weight);
  
          if(n_jet_50F>0) h_inc_jeteta_1_50_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
          if(n_jet_50F>1) h_inc_jeteta_2_50_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
          if(n_jet_50F>2) h_inc_jeteta_3_50_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
          if(n_jet_50F>3) h_inc_jeteta_4_50_F->Fill(patJetNoElecPfAk05Eta_[i],weight);
        }
      }
//INCLUSIVE JET MULTIPLICITIES//

      for(int i = 0 ; i<20;i++){
        if(n_jet_30>(i-1))h_n_inc_jet_30->Fill(i,weight);
        if(n_jet_30BE>(i-1)){
          h_n_inc_jet_30BE->Fill(i,weight);
	  if(n_jet_30F ==0) h_n_inc_jet_30BE_only->Fill(i,weight);	  
	}
        if(n_jet_30F>(i-1)){
	  h_n_inc_jet_30F->Fill(i,weight);
          if(n_jet_30BE ==0) h_n_inc_jet_30F_only->Fill(i,weight);	  
	}
        if(n_jet_30B>(i-1)){
	  h_n_inc_jet_30B->Fill(i,weight);
          if(n_jet_30F ==0 &&(n_jet_30BE==n_jet_30B)) h_n_inc_jet_30B_only->Fill(i,weight);
	}
        if(n_jet_50>(i-1))h_n_inc_jet_50->Fill(i,weight);
        if(n_jet_50BE>(i-1)){
          h_n_inc_jet_50BE->Fill(i,weight);
	  if(n_jet_50F ==0) h_n_inc_jet_50BE_only->Fill(i,weight);	  
	}
        if(n_jet_50F>(i-1)){
	  h_n_inc_jet_50F->Fill(i,weight);
          if(n_jet_50BE ==0) h_n_inc_jet_50F_only->Fill(i,weight);	  
	}
        if(n_jet_50B>(i-1)){
	  h_n_inc_jet_50B->Fill(i,weight);
          if(n_jet_50F ==0 &&(n_jet_50BE==n_jet_50B)) h_n_inc_jet_50B_only->Fill(i,weight);
	}
      }
//EXCLUSIVE JET MULTIPLICITIES//
      if(n_jet_30BE ==0) h_n_jet_30F_only->Fill(n_jet_30F,weight);
      if(n_jet_30F ==0) h_n_jet_30BE_only->Fill(n_jet_30BE,weight);
      if(n_jet_30F ==0 &&(n_jet_30BE==n_jet_30B)) h_n_jet_30B_only->Fill(n_jet_30B,weight);
      h_n_jet_30->Fill(n_jet_30,weight);
      h_n_jet_30BE->Fill(n_jet_30BE,weight);
      h_n_jet_30F->Fill(n_jet_30F,weight);
      h_n_jet_30B->Fill(n_jet_30B,weight);

      if(n_jet_50BE ==0) h_n_jet_50F_only->Fill(n_jet_50F,weight);
      if(n_jet_50F ==0) h_n_jet_50BE_only->Fill(n_jet_50BE,weight);
      if(n_jet_50F ==0 &&(n_jet_50BE==n_jet_50B)) h_n_jet_50B_only->Fill(n_jet_50B,weight);
      h_n_jet_50->Fill(n_jet_50,weight);
      h_n_jet_50BE->Fill(n_jet_50BE,weight);
      h_n_jet_50F->Fill(n_jet_50F,weight);
      h_n_jet_50B->Fill(n_jet_50B,weight);

//LEADING & SUBLEADING JET  ETA//
      if(n_jet_30>0){
        h_jeteta_1_30->Fill(patJetNoElecPfAk05Eta_[0],weight);
      }
      if(n_jet_30>1){
        h_jeteta_2_30->Fill(patJetNoElecPfAk05Eta_[1],weight);
      }
      if(n_jet_30BE>0){
       if(patJetNoElecPfAk05Eta_[0]<2.4) h_jeteta_1_30_BE->Fill(patJetNoElecPfAk05Eta_[0],weight);
      }
      if(n_jet_30BE>1){
       if(patJetNoElecPfAk05Eta_[1]<2.4) h_jeteta_2_30_BE->Fill(patJetNoElecPfAk05Eta_[1],weight);
      }

      if(n_jet_50>0){
        h_jeteta_1_50->Fill(patJetNoElecPfAk05Eta_[0],weight);
      }
      if(n_jet_50>1){
        h_jeteta_2_50->Fill(patJetNoElecPfAk05Eta_[1],weight);
      }

//LEADING & SUBLEADING JET PT'S WRT ETA//
      if(n_jet_30>0){
        if(patJetNoElecPfAk05Eta_[0]<2.4)h_c_jetpt_1_30->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(patJetNoElecPfAk05Eta_[0]>2.4)h_f_jetpt_1_30->Fill(patJetNoElecPfAk05Pt_[0],weight);
      }
      if(n_jet_30>1){
        if(patJetNoElecPfAk05Eta_[1]<2.4)h_c_jetpt_2_30->Fill(patJetNoElecPfAk05Pt_[1],weight);
        if(patJetNoElecPfAk05Eta_[1]>2.4)h_f_jetpt_2_30->Fill(patJetNoElecPfAk05Pt_[1],weight);
      }

      if(n_jet_50>0){
        if(patJetNoElecPfAk05Eta_[0]<2.4)h_c_jetpt_1_50->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(patJetNoElecPfAk05Eta_[0]>2.4)h_f_jetpt_1_50->Fill(patJetNoElecPfAk05Pt_[0],weight);
      }
      if(n_jet_50>1){
        if(patJetNoElecPfAk05Eta_[1]<2.4)h_c_jetpt_2_50->Fill(patJetNoElecPfAk05Pt_[1],weight);
        if(patJetNoElecPfAk05Eta_[1]>2.4)h_f_jetpt_2_50->Fill(patJetNoElecPfAk05Pt_[1],weight);
      }

//DR DPHI DETA OF 1st & 2nd JETS//


      if(n_jet_30>1){
        TLorentzVector j1;
        TLorentzVector j2;
        j1.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[0],patJetNoElecPfAk05Eta_[0],patJetNoElecPfAk05Phi_[0],patJetNoElecPfAk05En_[0]);
        j2.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[1],patJetNoElecPfAk05Eta_[1],patJetNoElecPfAk05Phi_[1],patJetNoElecPfAk05En_[1]);
        h_dYj1j2_30 ->Fill(fabs(j1.Rapidity()-j2.Rapidity()),weight);
        h_dphij1j2_30-> Fill(DeltaPhi(j1.Phi(),j2.Phi()),weight);
        h_dRj1j2_30-> Fill(DeltaR(j1.Eta(),j2.Eta(),j1.Phi(),j2.Phi()),weight);
        h_Mj1j2_30-> Fill((j1+j2).M(),weight);

      }

      if(n_jet_50>1){
        TLorentzVector j1;
        TLorentzVector j2;
        j1.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[0],patJetNoElecPfAk05Eta_[0],patJetNoElecPfAk05Phi_[0],patJetNoElecPfAk05En_[0]);
        j2.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[1],patJetNoElecPfAk05Eta_[1],patJetNoElecPfAk05Phi_[1],patJetNoElecPfAk05En_[1]);
        h_dYj1j2_50 ->Fill(fabs(j1.Rapidity()-j2.Rapidity()),weight);
        h_dphij1j2_50-> Fill(DeltaPhi(j1.Phi(),j2.Phi()),weight);
        h_dRj1j2_50-> Fill(DeltaR(j1.Eta(),j2.Eta(),j1.Phi(),j2.Phi()),weight);
        h_Mj1j2_50-> Fill((j1+j2).M(),weight);
      }

      if(n_jet_30F>1 &&n_jet_30==0){
        TLorentzVector j1;
        TLorentzVector j2;
        j1.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[0],patJetNoElecPfAk05Eta_[0],patJetNoElecPfAk05Phi_[0],patJetNoElecPfAk05En_[0]);
        j2.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[1],patJetNoElecPfAk05Eta_[1],patJetNoElecPfAk05Phi_[1],patJetNoElecPfAk05En_[1]);
        h_dYj1j2_30_F ->Fill(fabs(j1.Rapidity()-j2.Rapidity()),weight);
        h_dphij1j2_30_F-> Fill(DeltaPhi(j1.Phi(),j2.Phi()),weight);
        h_dRj1j2_30_F-> Fill(DeltaR(j1.Eta(),j2.Eta(),j1.Phi(),j2.Phi()),weight);
        h_Mj1j2_30_F-> Fill((j1+j2).M(),weight);

      }

      if(n_jet_50F>1 &&n_jet_50==0){
        TLorentzVector j1;
        TLorentzVector j2;
        j1.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[0],patJetNoElecPfAk05Eta_[0],patJetNoElecPfAk05Phi_[0],patJetNoElecPfAk05En_[0]);
        j2.SetPtEtaPhiE(patJetNoElecPfAk05Pt_[1],patJetNoElecPfAk05Eta_[1],patJetNoElecPfAk05Phi_[1],patJetNoElecPfAk05En_[1]);
        h_dYj1j2_50_F ->Fill(fabs(j1.Rapidity()-j2.Rapidity()),weight);
        h_dphij1j2_50_F-> Fill(DeltaPhi(j1.Phi(),j2.Phi()),weight);
        h_dRj1j2_50_F-> Fill(DeltaR(j1.Eta(),j2.Eta(),j1.Phi(),j2.Phi()),weight);
        h_Mj1j2_50_F-> Fill((j1+j2).M(),weight);
      }

      if(n_jet_30>0) h_diElecpt_1_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30>1) h_diElecpt_2_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30>2) h_diElecpt_3_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30>3) h_diElecpt_4_30_BEF->Fill(diElecpt,weight);
		
      if(n_jet_30>0) h_diElecrapidity_1_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30>1) h_diElecrapidity_2_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30>2) h_diElecrapidity_3_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30>3) h_diElecrapidity_4_30_BEF->Fill(diElecrapidity,weight);
		
      if(n_jet_30>0) h_diElecphi_1_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30>1) h_diElecphi_2_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30>2) h_diElecphi_3_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30>3) h_diElecphi_4_30_BEF->Fill(diElecphi,weight);

      if(n_jet_30>0) h_diElecM_1_30_BEF->Fill(MZElec,weight);
      if(n_jet_30>1) h_diElecM_2_30_BEF->Fill(MZElec,weight);
      if(n_jet_30>2) h_diElecM_3_30_BEF->Fill(MZElec,weight);
      if(n_jet_30>3) h_diElecM_4_30_BEF->Fill(MZElec,weight);

      if(n_jet_30>0) h_jetpt_1_30_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30>1) h_jetpt_2_30_BEF->Fill(patJetNoElecPfAk05Pt_[1],weight);
      if(n_jet_30>2) h_jetpt_3_30_BEF->Fill(patJetNoElecPfAk05Pt_[2],weight);
      if(n_jet_30>3) h_jetpt_4_30_BEF->Fill(patJetNoElecPfAk05Pt_[3],weight);

      if(n_jet_30==0) h_diElecpt_e0_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30==1) h_diElecpt_e1_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30==2) h_diElecpt_e2_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30==3) h_diElecpt_e3_30_BEF->Fill(diElecpt,weight);
      if(n_jet_30==4) h_diElecpt_e4_30_BEF->Fill(diElecpt,weight);
		
      if(n_jet_30==0) h_diElecrapidity_e0_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30==1) h_diElecrapidity_e1_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30==2) h_diElecrapidity_e2_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30==3) h_diElecrapidity_e3_30_BEF->Fill(diElecrapidity,weight);
      if(n_jet_30==4) h_diElecrapidity_e4_30_BEF->Fill(diElecrapidity,weight);
		
      if(n_jet_30==0) h_diElecphi_e0_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30==1) h_diElecphi_e1_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30==2) h_diElecphi_e2_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30==3) h_diElecphi_e3_30_BEF->Fill(diElecphi,weight);
      if(n_jet_30==4) h_diElecphi_e4_30_BEF->Fill(diElecphi,weight);

      if(n_jet_30==0) h_diElecM_e0_30_BEF->Fill(MZElec,weight);
      if(n_jet_30==1) h_diElecM_e1_30_BEF->Fill(MZElec,weight);
      if(n_jet_30==2) h_diElecM_e2_30_BEF->Fill(MZElec,weight);
      if(n_jet_30==3) h_diElecM_e3_30_BEF->Fill(MZElec,weight);
      if(n_jet_30==4) h_diElecM_e4_30_BEF->Fill(MZElec,weight);

      if(n_jet_30==1) h_jetpt_e1_30_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30==2) h_jetpt_e2_30_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30==3) h_jetpt_e3_30_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30==4) h_jetpt_e4_30_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);

      //ONLY FORWARD JET EVENTS
      if(n_jet_30BE ==0){
        if(n_jet_30F>0) h_diElecpt_1_30_F->Fill(diElecpt,weight);
        if(n_jet_30F>1) h_diElecpt_2_30_F->Fill(diElecpt,weight);
        if(n_jet_30F>2) h_diElecpt_3_30_F->Fill(diElecpt,weight);
        if(n_jet_30F>3) h_diElecpt_4_30_F->Fill(diElecpt,weight);
		
        if(n_jet_30F>0) h_diElecrapidity_1_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F>1) h_diElecrapidity_2_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F>2) h_diElecrapidity_3_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F>3) h_diElecrapidity_4_30_F->Fill(diElecrapidity,weight);
		
        if(n_jet_30F>0) h_diElecphi_1_30_F->Fill(diElecphi,weight);
        if(n_jet_30F>1) h_diElecphi_2_30_F->Fill(diElecphi,weight);
        if(n_jet_30F>2) h_diElecphi_3_30_F->Fill(diElecphi,weight);
        if(n_jet_30F>3) h_diElecphi_4_30_F->Fill(diElecphi,weight);

        if(n_jet_30F>0) h_diElecM_1_30_F->Fill(MZElec,weight);
        if(n_jet_30F>1) h_diElecM_2_30_F->Fill(MZElec,weight);
        if(n_jet_30F>2) h_diElecM_3_30_F->Fill(MZElec,weight);
        if(n_jet_30F>3) h_diElecM_4_30_F->Fill(MZElec,weight);

        if(n_jet_30F>0) h_jetpt_1_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_30F>1) h_jetpt_2_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_30F>2) h_jetpt_3_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_30F>3) h_jetpt_4_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);

        if(n_jet_30F==0) h_diElecpt_e0_30_F->Fill(diElecpt,weight);
        if(n_jet_30F==1) h_diElecpt_e1_30_F->Fill(diElecpt,weight);
        if(n_jet_30F==2) h_diElecpt_e2_30_F->Fill(diElecpt,weight);
        if(n_jet_30F==3) h_diElecpt_e3_30_F->Fill(diElecpt,weight);
        if(n_jet_30F==4) h_diElecpt_e4_30_F->Fill(diElecpt,weight);
		
        if(n_jet_30F==0) h_diElecrapidity_e0_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F==1) h_diElecrapidity_e1_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F==2) h_diElecrapidity_e2_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F==3) h_diElecrapidity_e3_30_F->Fill(diElecrapidity,weight);
        if(n_jet_30F==4) h_diElecrapidity_e4_30_F->Fill(diElecrapidity,weight);
		
        if(n_jet_30F==0) h_diElecphi_e0_30_F->Fill(diElecphi,weight);
        if(n_jet_30F==1) h_diElecphi_e1_30_F->Fill(diElecphi,weight);
        if(n_jet_30F==2) h_diElecphi_e2_30_F->Fill(diElecphi,weight);
        if(n_jet_30F==3) h_diElecphi_e3_30_F->Fill(diElecphi,weight);
        if(n_jet_30F==4) h_diElecphi_e4_30_F->Fill(diElecphi,weight);

        if(n_jet_30F==0) h_diElecM_e0_30_F->Fill(MZElec,weight);
        if(n_jet_30F==1) h_diElecM_e1_30_F->Fill(MZElec,weight);
        if(n_jet_30F==2) h_diElecM_e2_30_F->Fill(MZElec,weight);
        if(n_jet_30F==3) h_diElecM_e3_30_F->Fill(MZElec,weight);
        if(n_jet_30F==4) h_diElecM_e4_30_F->Fill(MZElec,weight);

        if(n_jet_30F==1) h_jetpt_e1_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_30F==2) h_jetpt_e2_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_30F==3) h_jetpt_e3_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_30F==4) h_jetpt_e4_30_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
      }

      if(n_jet_50>0) h_diElecpt_1_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50>1) h_diElecpt_2_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50>2) h_diElecpt_3_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50>3) h_diElecpt_4_50_BEF->Fill(diElecpt,weight);
		
      if(n_jet_50>0) h_diElecrapidity_1_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50>1) h_diElecrapidity_2_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50>2) h_diElecrapidity_3_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50>3) h_diElecrapidity_4_50_BEF->Fill(diElecrapidity,weight);
		
      if(n_jet_50>0) h_diElecphi_1_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50>1) h_diElecphi_2_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50>2) h_diElecphi_3_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50>3) h_diElecphi_4_50_BEF->Fill(diElecphi,weight);

      if(n_jet_50>0) h_diElecM_1_50_BEF->Fill(MZElec,weight);
      if(n_jet_50>1) h_diElecM_2_50_BEF->Fill(MZElec,weight);
      if(n_jet_50>2) h_diElecM_3_50_BEF->Fill(MZElec,weight);
      if(n_jet_50>3) h_diElecM_4_50_BEF->Fill(MZElec,weight);

      if(n_jet_50>0) h_jetpt_1_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50>1) h_jetpt_2_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50>2) h_jetpt_3_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50>3) h_jetpt_4_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);

      if(n_jet_50==0) h_diElecpt_e0_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50==1) h_diElecpt_e1_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50==2) h_diElecpt_e2_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50==3) h_diElecpt_e3_50_BEF->Fill(diElecpt,weight);
      if(n_jet_50==4) h_diElecpt_e4_50_BEF->Fill(diElecpt,weight);
		
      if(n_jet_50==0) h_diElecrapidity_e0_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50==1) h_diElecrapidity_e1_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50==2) h_diElecrapidity_e2_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50==3) h_diElecrapidity_e3_50_BEF->Fill(diElecrapidity,weight);
      if(n_jet_50==4) h_diElecrapidity_e4_50_BEF->Fill(diElecrapidity,weight);
		
      if(n_jet_50==0) h_diElecphi_e0_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50==1) h_diElecphi_e1_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50==2) h_diElecphi_e2_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50==3) h_diElecphi_e3_50_BEF->Fill(diElecphi,weight);
      if(n_jet_50==4) h_diElecphi_e4_50_BEF->Fill(diElecphi,weight);

      if(n_jet_50==0) h_diElecM_e0_50_BEF->Fill(MZElec,weight);
      if(n_jet_50==1) h_diElecM_e1_50_BEF->Fill(MZElec,weight);
      if(n_jet_50==2) h_diElecM_e2_50_BEF->Fill(MZElec,weight);
      if(n_jet_50==3) h_diElecM_e3_50_BEF->Fill(MZElec,weight);
      if(n_jet_50==4) h_diElecM_e4_50_BEF->Fill(MZElec,weight);

      if(n_jet_50==1) h_jetpt_e1_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50==2) h_jetpt_e2_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50==3) h_jetpt_e3_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50==4) h_jetpt_e4_50_BEF->Fill(patJetNoElecPfAk05Pt_[0],weight);

//ONLY FORWARD JETS
      if(n_jet_50BE ==0){
        if(n_jet_50F>0) h_diElecpt_1_50_F->Fill(diElecpt,weight);
        if(n_jet_50F>1) h_diElecpt_2_50_F->Fill(diElecpt,weight);
        if(n_jet_50F>2) h_diElecpt_3_50_F->Fill(diElecpt,weight);
        if(n_jet_50F>3) h_diElecpt_4_50_F->Fill(diElecpt,weight);
		
        if(n_jet_50F>0) h_diElecrapidity_1_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F>1) h_diElecrapidity_2_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F>2) h_diElecrapidity_3_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F>3) h_diElecrapidity_4_50_F->Fill(diElecrapidity,weight);
		
        if(n_jet_50F>0) h_diElecphi_1_50_F->Fill(diElecphi,weight);
        if(n_jet_50F>1) h_diElecphi_2_50_F->Fill(diElecphi,weight);
        if(n_jet_50F>2) h_diElecphi_3_50_F->Fill(diElecphi,weight);
        if(n_jet_50F>3) h_diElecphi_4_50_F->Fill(diElecphi,weight);

        if(n_jet_50F>0) h_diElecM_1_50_F->Fill(MZElec,weight);
        if(n_jet_50F>1) h_diElecM_2_50_F->Fill(MZElec,weight);
        if(n_jet_50F>2) h_diElecM_3_50_F->Fill(MZElec,weight);
        if(n_jet_50F>3) h_diElecM_4_50_F->Fill(MZElec,weight);

        if(n_jet_50F>0) h_jetpt_1_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_50F>1) h_jetpt_2_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_50F>2) h_jetpt_3_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_50F>3) h_jetpt_4_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
    
        if(n_jet_50F==0) h_diElecpt_e0_50_F->Fill(diElecpt,weight);
        if(n_jet_50F==1) h_diElecpt_e1_50_F->Fill(diElecpt,weight);
        if(n_jet_50F==2) h_diElecpt_e2_50_F->Fill(diElecpt,weight);
        if(n_jet_50F==3) h_diElecpt_e3_50_F->Fill(diElecpt,weight);
        if(n_jet_50F==4) h_diElecpt_e4_50_F->Fill(diElecpt,weight);
		
        if(n_jet_50F==0) h_diElecrapidity_e0_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F==1) h_diElecrapidity_e1_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F==2) h_diElecrapidity_e2_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F==3) h_diElecrapidity_e3_50_F->Fill(diElecrapidity,weight);
        if(n_jet_50F==4) h_diElecrapidity_e4_50_F->Fill(diElecrapidity,weight);
		
        if(n_jet_50F==0) h_diElecphi_e0_50_F->Fill(diElecphi,weight);
        if(n_jet_50F==1) h_diElecphi_e1_50_F->Fill(diElecphi,weight);
        if(n_jet_50F==2) h_diElecphi_e2_50_F->Fill(diElecphi,weight);
        if(n_jet_50F==3) h_diElecphi_e3_50_F->Fill(diElecphi,weight);
        if(n_jet_50F==4) h_diElecphi_e4_50_F->Fill(diElecphi,weight);

        if(n_jet_50F==0) h_diElecM_e0_50_F->Fill(MZElec,weight);
        if(n_jet_50F==1) h_diElecM_e1_50_F->Fill(MZElec,weight);
        if(n_jet_50F==2) h_diElecM_e2_50_F->Fill(MZElec,weight);
        if(n_jet_50F==3) h_diElecM_e3_50_F->Fill(MZElec,weight);
        if(n_jet_50F==4) h_diElecM_e4_50_F->Fill(MZElec,weight);

        if(n_jet_50F==1) h_jetpt_e1_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_50F==2) h_jetpt_e2_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_50F==3) h_jetpt_e3_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
        if(n_jet_50F==4) h_jetpt_e4_50_F->Fill(patJetNoElecPfAk05Pt_[0],weight);
      }

      if(n_jet_30BE>0) h_diElecpt_1_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE>1) h_diElecpt_2_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE>2) h_diElecpt_3_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE>3) h_diElecpt_4_30_BE->Fill(diElecpt,weight);
		
      if(n_jet_30BE>0) h_diElecrapidity_1_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE>1) h_diElecrapidity_2_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE>2) h_diElecrapidity_3_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE>3) h_diElecrapidity_4_30_BE->Fill(diElecrapidity,weight);
		
      if(n_jet_30BE>0) h_diElecphi_1_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE>1) h_diElecphi_2_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE>2) h_diElecphi_3_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE>3) h_diElecphi_4_30_BE->Fill(diElecphi,weight);

      if(n_jet_30BE>0) h_diElecM_1_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE>1) h_diElecM_2_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE>2) h_diElecM_3_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE>3) h_diElecM_4_30_BE->Fill(MZElec,weight);

      if(n_jet_30BE>0 && patJetNoElecPfAk05Eta_[0]<2.4) h_jetpt_1_30_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30BE>1 && patJetNoElecPfAk05Eta_[1]<2.4) h_jetpt_2_30_BE->Fill(patJetNoElecPfAk05Pt_[1],weight);
      if(n_jet_30BE>2 && patJetNoElecPfAk05Eta_[2]<2.4) h_jetpt_3_30_BE->Fill(patJetNoElecPfAk05Pt_[2],weight);
      if(n_jet_30BE>3 && patJetNoElecPfAk05Eta_[3]<2.4) h_jetpt_4_30_BE->Fill(patJetNoElecPfAk05Pt_[3],weight);

      if(n_jet_30BE==0) h_diElecpt_e0_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE==1) h_diElecpt_e1_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE==2) h_diElecpt_e2_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE==3) h_diElecpt_e3_30_BE->Fill(diElecpt,weight);
      if(n_jet_30BE==4) h_diElecpt_e4_30_BE->Fill(diElecpt,weight);
		
      if(n_jet_30BE==0) h_diElecrapidity_e0_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE==1) h_diElecrapidity_e1_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE==2) h_diElecrapidity_e2_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE==3) h_diElecrapidity_e3_30_BE->Fill(diElecrapidity,weight);
      if(n_jet_30BE==4) h_diElecrapidity_e4_30_BE->Fill(diElecrapidity,weight);
		
      if(n_jet_30BE==0) h_diElecphi_e0_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE==1) h_diElecphi_e1_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE==2) h_diElecphi_e2_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE==3) h_diElecphi_e3_30_BE->Fill(diElecphi,weight);
      if(n_jet_30BE==4) h_diElecphi_e4_30_BE->Fill(diElecphi,weight);

      if(n_jet_30BE==0) h_diElecM_e0_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE==1) h_diElecM_e1_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE==2) h_diElecM_e2_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE==3) h_diElecM_e3_30_BE->Fill(MZElec,weight);
      if(n_jet_30BE==4) h_diElecM_e4_30_BE->Fill(MZElec,weight);

      if(n_jet_30BE==1) h_jetpt_e1_30_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30BE==2) h_jetpt_e2_30_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30BE==3) h_jetpt_e3_30_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30BE==4) h_jetpt_e4_30_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);


      if(n_jet_30B>0) h_diElecpt_1_30_B->Fill(diElecpt,weight);
      if(n_jet_30B>1) h_diElecpt_2_30_B->Fill(diElecpt,weight);
      if(n_jet_30B>2) h_diElecpt_3_30_B->Fill(diElecpt,weight);
      if(n_jet_30B>3) h_diElecpt_4_30_B->Fill(diElecpt,weight);
		
      if(n_jet_30B>0) h_diElecrapidity_1_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B>1) h_diElecrapidity_2_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B>2) h_diElecrapidity_3_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B>3) h_diElecrapidity_4_30_B->Fill(diElecrapidity,weight);
		
      if(n_jet_30B>0) h_diElecphi_1_30_B->Fill(diElecphi,weight);
      if(n_jet_30B>1) h_diElecphi_2_30_B->Fill(diElecphi,weight);
      if(n_jet_30B>2) h_diElecphi_3_30_B->Fill(diElecphi,weight);
      if(n_jet_30B>3) h_diElecphi_4_30_B->Fill(diElecphi,weight);

      if(n_jet_30B>0) h_diElecM_1_30_B->Fill(MZElec,weight);
      if(n_jet_30B>1) h_diElecM_2_30_B->Fill(MZElec,weight);
      if(n_jet_30B>2) h_diElecM_3_30_B->Fill(MZElec,weight);
      if(n_jet_30B>3) h_diElecM_4_30_B->Fill(MZElec,weight);

      if(n_jet_30B>0) h_jetpt_1_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30B>1) h_jetpt_2_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30B>2) h_jetpt_3_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30B>3) h_jetpt_4_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);

      if(n_jet_30B==0) h_diElecpt_e0_30_B->Fill(diElecpt,weight);
      if(n_jet_30B==1) h_diElecpt_e1_30_B->Fill(diElecpt,weight);
      if(n_jet_30B==2) h_diElecpt_e2_30_B->Fill(diElecpt,weight);
      if(n_jet_30B==3) h_diElecpt_e3_30_B->Fill(diElecpt,weight);
      if(n_jet_30B==4) h_diElecpt_e4_30_B->Fill(diElecpt,weight);
		
      if(n_jet_30B==0) h_diElecrapidity_e0_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B==1) h_diElecrapidity_e1_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B==2) h_diElecrapidity_e2_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B==3) h_diElecrapidity_e3_30_B->Fill(diElecrapidity,weight);
      if(n_jet_30B==4) h_diElecrapidity_e4_30_B->Fill(diElecrapidity,weight);
		
      if(n_jet_30B==0) h_diElecphi_e0_30_B->Fill(diElecphi,weight);
      if(n_jet_30B==1) h_diElecphi_e1_30_B->Fill(diElecphi,weight);
      if(n_jet_30B==2) h_diElecphi_e2_30_B->Fill(diElecphi,weight);
      if(n_jet_30B==3) h_diElecphi_e3_30_B->Fill(diElecphi,weight);
      if(n_jet_30B==4) h_diElecphi_e4_30_B->Fill(diElecphi,weight);

      if(n_jet_30B==0) h_diElecM_e0_30_B->Fill(MZElec,weight);
      if(n_jet_30B==1) h_diElecM_e1_30_B->Fill(MZElec,weight);
      if(n_jet_30B==2) h_diElecM_e2_30_B->Fill(MZElec,weight);
      if(n_jet_30B==3) h_diElecM_e3_30_B->Fill(MZElec,weight);
      if(n_jet_30B==4) h_diElecM_e4_30_B->Fill(MZElec,weight);

      if(n_jet_30B==1) h_jetpt_e1_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30B==2) h_jetpt_e2_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30B==3) h_jetpt_e3_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_30B==4) h_jetpt_e4_30_B->Fill(patJetNoElecPfAk05Pt_[0],weight);


      if(n_jet_50BE>0) h_diElecpt_1_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE>1) h_diElecpt_2_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE>2) h_diElecpt_3_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE>3) h_diElecpt_4_50_BE->Fill(diElecpt,weight);
		
      if(n_jet_50BE>0) h_diElecrapidity_1_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE>1) h_diElecrapidity_2_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE>2) h_diElecrapidity_3_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE>3) h_diElecrapidity_4_50_BE->Fill(diElecrapidity,weight);
		
      if(n_jet_50BE>0) h_diElecphi_1_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE>1) h_diElecphi_2_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE>2) h_diElecphi_3_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE>3) h_diElecphi_4_50_BE->Fill(diElecphi,weight);

      if(n_jet_50BE>0) h_diElecM_1_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE>1) h_diElecM_2_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE>2) h_diElecM_3_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE>3) h_diElecM_4_50_BE->Fill(MZElec,weight);

      if(n_jet_50BE>0) h_jetpt_1_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50BE>1) h_jetpt_2_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50BE>2) h_jetpt_3_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50BE>3) h_jetpt_4_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);

      if(n_jet_50BE==0) h_diElecpt_e0_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE==1) h_diElecpt_e1_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE==2) h_diElecpt_e2_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE==3) h_diElecpt_e3_50_BE->Fill(diElecpt,weight);
      if(n_jet_50BE==4) h_diElecpt_e4_50_BE->Fill(diElecpt,weight);
		
      if(n_jet_50BE==0) h_diElecrapidity_e0_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE==1) h_diElecrapidity_e1_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE==2) h_diElecrapidity_e2_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE==3) h_diElecrapidity_e3_50_BE->Fill(diElecrapidity,weight);
      if(n_jet_50BE==4) h_diElecrapidity_e4_50_BE->Fill(diElecrapidity,weight);
		
      if(n_jet_50BE==0) h_diElecphi_e0_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE==1) h_diElecphi_e1_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE==2) h_diElecphi_e2_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE==3) h_diElecphi_e3_50_BE->Fill(diElecphi,weight);
      if(n_jet_50BE==4) h_diElecphi_e4_50_BE->Fill(diElecphi,weight);

      if(n_jet_50BE==0) h_diElecM_e0_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE==1) h_diElecM_e1_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE==2) h_diElecM_e2_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE==3) h_diElecM_e3_50_BE->Fill(MZElec,weight);
      if(n_jet_50BE==4) h_diElecM_e4_50_BE->Fill(MZElec,weight);

      if(n_jet_50BE==1) h_jetpt_e1_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50BE==2) h_jetpt_e2_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50BE==3) h_jetpt_e3_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50BE==4) h_jetpt_e4_50_BE->Fill(patJetNoElecPfAk05Pt_[0],weight);

      if(n_jet_50B>0) h_diElecpt_1_50_B->Fill(diElecpt,weight);
      if(n_jet_50B>1) h_diElecpt_2_50_B->Fill(diElecpt,weight);
      if(n_jet_50B>2) h_diElecpt_3_50_B->Fill(diElecpt,weight);
      if(n_jet_50B>3) h_diElecpt_4_50_B->Fill(diElecpt,weight);
		
      if(n_jet_50B>0) h_diElecrapidity_1_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B>1) h_diElecrapidity_2_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B>2) h_diElecrapidity_3_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B>3) h_diElecrapidity_4_50_B->Fill(diElecrapidity,weight);
		
      if(n_jet_50B>0) h_diElecphi_1_50_B->Fill(diElecphi,weight);
      if(n_jet_50B>1) h_diElecphi_2_50_B->Fill(diElecphi,weight);
      if(n_jet_50B>2) h_diElecphi_3_50_B->Fill(diElecphi,weight);
      if(n_jet_50B>3) h_diElecphi_4_50_B->Fill(diElecphi,weight);

      if(n_jet_50B>0) h_diElecM_1_50_B->Fill(MZElec,weight);
      if(n_jet_50B>1) h_diElecM_2_50_B->Fill(MZElec,weight);
      if(n_jet_50B>2) h_diElecM_3_50_B->Fill(MZElec,weight);
      if(n_jet_50B>3) h_diElecM_4_50_B->Fill(MZElec,weight);

      if(n_jet_50B>0) h_jetpt_1_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50B>1) h_jetpt_2_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50B>2) h_jetpt_3_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50B>3) h_jetpt_4_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);

      if(n_jet_50B==0) h_diElecpt_e0_50_B->Fill(diElecpt,weight);
      if(n_jet_50B==1) h_diElecpt_e1_50_B->Fill(diElecpt,weight);
      if(n_jet_50B==2) h_diElecpt_e2_50_B->Fill(diElecpt,weight);
      if(n_jet_50B==3) h_diElecpt_e3_50_B->Fill(diElecpt,weight);
      if(n_jet_50B==4) h_diElecpt_e4_50_B->Fill(diElecpt,weight);
		
      if(n_jet_50B==0) h_diElecrapidity_e0_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B==1) h_diElecrapidity_e1_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B==2) h_diElecrapidity_e2_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B==3) h_diElecrapidity_e3_50_B->Fill(diElecrapidity,weight);
      if(n_jet_50B==4) h_diElecrapidity_e4_50_B->Fill(diElecrapidity,weight);
		
      if(n_jet_50B==0) h_diElecphi_e0_50_B->Fill(diElecphi,weight);
      if(n_jet_50B==1) h_diElecphi_e1_50_B->Fill(diElecphi,weight);
      if(n_jet_50B==2) h_diElecphi_e2_50_B->Fill(diElecphi,weight);
      if(n_jet_50B==3) h_diElecphi_e3_50_B->Fill(diElecphi,weight);
      if(n_jet_50B==4) h_diElecphi_e4_50_B->Fill(diElecphi,weight);

      if(n_jet_50B==0) h_diElecM_e0_50_B->Fill(MZElec,weight);
      if(n_jet_50B==1) h_diElecM_e1_50_B->Fill(MZElec,weight);
      if(n_jet_50B==2) h_diElecM_e2_50_B->Fill(MZElec,weight);
      if(n_jet_50B==3) h_diElecM_e3_50_B->Fill(MZElec,weight);
      if(n_jet_50B==4) h_diElecM_e4_50_B->Fill(MZElec,weight);

      if(n_jet_50B==1) h_jetpt_e1_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50B==2) h_jetpt_e2_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50B==3) h_jetpt_e3_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);
      if(n_jet_50B==4) h_jetpt_e4_50_B->Fill(patJetNoElecPfAk05Pt_[0],weight);


