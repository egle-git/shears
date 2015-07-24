{
    string srcdir = "Sources/";
    vector<string> sources;
    sources.push_back("getFilesAndHistograms");
    sources.push_back("functions");
    sources.push_back("HistoSet");
    sources.push_back("ZJetsAndDPS");

    //--- Load shared libraries -------------------------------------------------------
    unsigned int nSources = sources.size();
    // choose the right library (MAC or Linux)
    //gROOT->ProcessLine(".L /usr/local/lib/libLHAPDF.dylib");
    gROOT->ProcessLine(".L /user/aleonard/LHAPDF/lib/libLHAPDF.so");
    for (unsigned int i(0); i < nSources; i++){
        std::cout << "Compiling " << srcdir + sources[i] << ".cc" << std::endl;
        gROOT->ProcessLine(string(".L " + srcdir + sources[i] + ".cc+").c_str());
    }
    //----------------------------------------------------------------------------------

    int doRoch        = 0;
    int doFlat        = 0;
    int doBJets       = 0;   // 0 - no infor on B-jets will be used ; 1, 2 .. require at least 1, 2, .. ;  -1, -2, .... veto the event if you have 1, 2 ... number off b-jets
    int doMETcut      = 0;   // if you want to apply MET cut
    int doQCD         = 0;   // 1-4 : 4 combination between isolation/anti-isolation and MT cuts
    int doPUStudy     = -10; // default int the ZJets
    bool doSameSign   = 0;   // contribution of QCD to emu in TTbar 
    bool doInvMassCut = 0; 
    bool hasRecoInfo  = 1;
    bool hasGenInfo   = 1;
    int jetPtMin      = 30;
    int jetPtMax      = 0;
    int ZPtMin        = 0 ;
    int ZEtaMin       = -999999; // default value -999999       !!!!!!!  factor 100 to keep things integer ....    eta 2.4  = eta Cut 240 
    int ZEtaMax       = 999999;  // default value  999999

    enum Samples {
        DATA, BACKGROUND, TAU, DYJETS, WJETS, ALL, PDF, PULL, SHERPA
    };

    //--- Settings ---------
    string lepSel = "DE";
    bool doDataEff(1);
    int NSystData(3);
    int NSystMC(5);
    bool doSysRunning(0);
    Samples DoWhat = DATA;
    //----------------------


    double lumi; 
    if      (lepSel == "DE") lumi = 19.618;
    else if (lepSel == "DMu") lumi = 19.584;
    else if (lepSel == "SMuE") lumi = 19.673;
    else if (lepSel == "SMu") lumi = 19.244;
    else if (lepSel == "SE") lumi = 19.174;

    short dataSyst[5] = {0, 2, 2, 5, 5};
    short dataDir[5]  = {0,-1, 1,-1, 1};

    short ttSyst[5]  = {0, 1, 1, 3, 3};
    short ttDir[5]   = {0,-1, 1,-1, 1};
    float ttScale[5] = {1, 1, 1, 0.10, 0.10};

    short tauSyst[5]  = {0, 1, 1, 3, 3};
    short tauDir[5]   = {0,-1, 1,-1, 1};
    float tauScale[5] = {1, 1, 1, 0.05, 0.05};

    short bgSyst[5]  = {0, 1, 1, 3, 3};
    short bgDir[5]   = {0,-1, 1,-1, 1};
    float bgScale[5] = {1, 1, 1, 0.15, 0.15};
    //int doWhat = 222; // 100 - all ; 0 - data, 1 - background , 2 - tau ???, 3 - DY, 4 - W+jets, 51 - MC gen, 90 - PDF Syst., 1001 - do pull DY samples 


    if (lepSel == "DE") NSystData = 5;

    if (!doSysRunning) {
        NSystData = 1; 
        NSystMC = 1;
    }

    if (0) { // for individual production
        hasRecoInfo = true;
        hasGenInfo = false;
        ZJetsAndDPS DMuTT(lepSel + "_8TeV_TTJets", lumi * 245. * 1000 / 6923750., 1, 1, !doDataEff, ttSyst[0], ttDir[0], ttScale[0], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        DMuTT.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
    }

    if (DoWhat == DATA || DoWhat == ALL) {
        hasRecoInfo = true;
        hasGenInfo = false;
        for (unsigned int i(0); i < NSystData; i++) {
            ZJetsAndDPS DMudata(lepSel + "_8TeV_Data_dR", 1, 1, 1, doDataEff, dataSyst[i], dataDir[i], 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMudata.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMudataA(lepSel + "_8TeV_Data_dR_RunA", 1, 1, 1, doDataEff, dataSyst[i], dataDir[i], 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMudataA.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMudataB(lepSel + "_8TeV_Data_dR_RunB", 1, 1, 1, doDataEff, dataSyst[i], dataDir[i], 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMudataB.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMudataC(lepSel + "_8TeV_Data_dR_RunC", 1, 1, 1, doDataEff, dataSyst[i], dataDir[i], 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMudataC.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMudataD(lepSel + "_8TeV_Data_dR_RunD", 1, 1, 1, doDataEff, dataSyst[i], dataDir[i], 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMudataD.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
        }
    }

    if (DoWhat == BACKGROUND || DoWhat == ALL) {
        hasRecoInfo = true;
        hasGenInfo = false;

        for (unsigned int i(0); i < NSystMC; i++) { 

            ZJetsAndDPS DMuZZInc(lepSel + "_8TeV_ZZ_dR",              lumi*17.654        *1000/9799908.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuZZInc.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWZInc(lepSel + "_8TeV_WZ_dR",              lumi*33.21         *1000/10000283., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWZInc.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWWInc(lepSel + "_8TeV_WW_dR",              lumi*54.838        *1000/10000431., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWWInc.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuZZ(lepSel + "_8TeV_ZZJets2L2Nu_dR",        lumi*17.654*0.04039 *1000/954911.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuZZ.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWW(lepSel + "_8TeV_WWJets2L2Nu_dR",        lumi*54.838*0.10608 *1000/1933235., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWW.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuZZ1(lepSel + "_8TeV_ZZJets2L2Q_dR",        lumi*17.654*0.14118 *1000/1936727., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuZZ1.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuZZ2(lepSel + "_8TeV_ZZJets4L_dR",          lumi*17.654*0.010196*1000/4807893., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuZZ2.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWZ(lepSel + "_8TeV_WZJets3LNu_dR",         lumi*33.21 *0.032887*1000/2017979., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWZ.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWZ1(lepSel + "_8TeV_WZJets2L2Q_dR",        lumi*33.21 *0.068258*1000/3215990., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWZ1.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuT(lepSel + "_8TeV_T_s_channel_dR",         lumi*3.79           *1000/259961.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuT.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuT1(lepSel + "_8TeV_T_t_channel_dR",        lumi*56.4           *1000/3747932., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuT1.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuT2(lepSel + "_8TeV_T_tW_channel_dR",       lumi*11.1           *1000/497658.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuT2.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuTbar(lepSel + "_8TeV_Tbar_s_channel_dR",   lumi*1.76           *1000/139974.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuTbar.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuTbar1(lepSel + "_8TeV_Tbar_t_channel_dR",  lumi*30.7           *1000/1935072., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuTbar1.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuTbar2(lepSel + "_8TeV_Tbar_tW_channel_dR", lumi*11.1           *1000/493460.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuTbar2.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMuDY10(lepSel + "_8TeV_DYJets10to50",    lumi*860.5         *1000/11707222.,  1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMuDY10.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            //--- ttbar ---
            ZJetsAndDPS DMuTT(lepSel + "_8TeV_TTJets_dR", lumi * 245. * 1000 / 6923750., 1, 1, !doDataEff, ttSyst[i], ttDir[i], ttScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuTT.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            ////--- ttbar reweigthed ---
            //ZJetsAndDPS DMuTTrew(lepSel + "_8TeV_TTJets_dR_5311_TopReweighting", lumi * 245. * 1000 / 6923652., 1, 1, !doDataEff, ttSyst[i], ttDir[i], ttScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMuTTrew.Loop(1, 1, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            //--- W Jets background for DMu and DE
            if (lepSel != "SE" && lepSel != "SMu") {
                //ZJetsAndDPS DMuWJ(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi * 36703. * 1000 / 76102995., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin , ZEtaMin, ZEtaMax);
                //DMuWJ.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

                //ZJetsAndDPS DMuWJmix(lepSel + "_8TeV_WJetsALL_MIX_UNFOLDING_dR_5311",           lumi*36703.         *1000/76102995., 1, 1, !doDataEff, bgSyst[i], bgDir[i], bgScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
                //DMuWJmix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            }
        }
    }

    if (DoWhat == TAU || DoWhat == ALL) {
        hasRecoInfo = true; 
        hasGenInfo = true;
        for (unsigned int i(0); i < NSystMC; i++) { 
            ZJetsAndDPS DMuDYTau(lepSel + "_8TeV_DYJetsToLL_FromTau_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, tauSyst[i], tauDir[i], tauScale[i], jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuDYTau.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
        }
    }


    if (DoWhat == DYJETS || DoWhat == ALL) {
        hasRecoInfo = true; 
        hasGenInfo = false;
        if (lepSel == "DMu" || lepSel == "DE") hasGenInfo = true;

        //ZJetsAndDPS DMuDY(lepSel + "_8TeV_DYJetsToLL_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        //DMuDY.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

        ZJetsAndDPS DMuDYMix(lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        DMuDYMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

        // scale files
        //ZJetsAndDPS DMuDYscaleUp(lepSel + "_8TeV_DYJets_UNFOLDING_dR_5311_Inf3_scaleUp", lumi*3531.8*1000/2170270., 1, 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        //DMuDYscaleUp.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
        //ZJetsAndDPS DMuDYscaleDown(lepSel + "_8TeV_DYJets_UNFOLDING_dR_5311_Inf3_scaleUp", lumi*3531.8*1000/1934901., 1, 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        //DMuDYscaleDown.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

        if (doSysRunning) {
            ////PU
            //ZJetsAndDPS DMuDY1(lepSel + "_8TeV_DYJetsToLL_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 1, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMuDY1.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMuDY2(lepSel + "_8TeV_DYJetsToLL_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 1,-1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMuDY2.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            ////JER
            //ZJetsAndDPS DMuDY3(lepSel + "_8TeV_DYJetsToLL_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 4, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMuDY3.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            //ZJetsAndDPS DMuDY4(lepSel + "_8TeV_DYJetsToLL_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 4,-1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            //DMuDY4.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            // MIX
            //PU
            ZJetsAndDPS DMuDYMIX1(lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 1, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuDYMIX1.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuDYMIX2(lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 1,-1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuDYMIX2.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            //JER
            ZJetsAndDPS DMuDYMIX3(lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 4, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuDYMIX3.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuDYMIX4(lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1, 1, !doDataEff, 4,-1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuDYMIX4.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

        }
    }

    // this is unfolding part for W+jets 
    if (DoWhat == WJETS || DoWhat == ALL) {
        hasRecoInfo = true; 
        hasGenInfo = false;
        if (lepSel == "SE" || lepSel == "SMu") hasGenInfo = true;

        //ZJetsAndDPS DMuWJ(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311",  lumi*36703.         *1000/76102995., 1., 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        //DMuWJ.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
        ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_MIX_UNFOLDING_dR", lumi*36703.         *1000/76102995., 1., 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
        DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

        if (doSysRunning){
            //PU
            ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1., 1, !doDataEff, 1, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1., 1, !doDataEff, 1,-1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            // jet smearing part
            if (hasGenInfo) {
                ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1, 1, !doDataEff, 4, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
                DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            }
            else { // xsec offsets
                ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1., 1, !doDataEff, 3, 1, 0.04, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
                DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

                ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1, 1, !doDataEff, 3, -1, 0.04, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
                DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);

            }
            ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1, 1, !doDataEff, 1, 1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
            ZJetsAndDPS DMuWJMix(lepSel + "_8TeV_WJetsALL_UNFOLDING_dR_5311", lumi*36703*1000/76102995., 1, 1, !doDataEff, 1,-1, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
            DMuWJMix.Loop(hasRecoInfo, hasGenInfo, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy);
        }
    }

    if (DoWhat == SHERPA) {

        //    // this is setup for sherpa NLO 
        //    ZJetsAndDPS DESherpaTest2NLO("DE_8TeV_DY_Sherpa_2NLO4_HepMC_dR_Full_List",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //DESherpaTest2NLO.Loop(0, 1, 0, 0, 0);

        ZJetsAndDPS DESherpaTest1NLO("DMu_8TeV_Sherpa_UNFOLDING_dR",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        DESherpaTest1NLO.Loop(1, 1, 0, 0, 0); 

        //    ZJetsAndDPS DESherpaTest1NLO("DE_8TeV_DY_Sherpa_1NLO4_HepMC_dR_Full_List",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    DESherpaTest1NLO.Loop(0, 1, 0, 0, 0); 

        //    ZJetsAndDPS DESherpaTest1NLOScaleDown("DE_8TeV_DY_Sherpa_1NLO4_scaleDown_HepMC_dR_Full_List",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //DESherpaTest1NLOScaleDown.Loop(0, 1, 0, 0); 

        //    ZJetsAndDPS DESherpaTest1NLOScaleUp("DE_8TeV_DY_Sherpa_1NLO4_scaleUp_HepMC_dR_Full_List",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //DESherpaTest1NLOScaleUp.Loop(0, 1, 0, 0); 



        //    //ZJetsAndDPS DMuSherpa(lepSel+"_DYJets_Sherpa_mcEveWeight",   lumi * 3531.8           * 1000 / 30459503.,    1.,  0,  0,  0,  0,  1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //ZJetsAndDPS DESherpALL("DE_8TeV_Sherpa_HepMC_Z2jetNLO4jetLO_multithread_ALL_dR",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //DESherpALL.Loop(0, 1, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy); 


        //    ZJetsAndDPS DMuPowMiNLO("DMu_8TeV_DYJets_PowhegZ2jMiNLO_dR_GEN_Cern",                             lumi * 1.            * 1000 / 1964662.,    1.013,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax   ); 
        //    DMuPowMiNLO.Loop(0, 1, 0, 0, 0);
        //    //ZJetsAndDPS DEPow("DE_8TeV_DYJets_PowhegNLO1Jet_dR_GEN",                             lumi * 334.            * 1000 / 2948078.,    1.013,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax   ); 
        //    //DEPow.Loop(0, 1, 0, 0);
        //    //ZJetsAndDPS DEPowSU("DE_8TeV_DYJets_PowhegNLO1Jet_dR_ScaleUp_GEN",                             lumi * 318.4            * 1000 / 5446372.,    1.013,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax   ); 
        //    //DEPowSU.Loop(0, 1, 0, 0);
        //    //ZJetsAndDPS DEPowSD("DE_8TeV_DYJets_PowhegNLO1Jet_dR_ScaleDown_GEN",                             lumi * 357.2            * 1000 / 5856584.,    1.013,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax   ); 
        //    //DEPowSD.Loop(0, 1, 0, 0);

        //    //ZJetsAndDPS DESherpaTest("DE_8TeV_DY_Sherpa_HepMC_dR_Full_List",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //ZJetsAndDPS DESherpaTest("Sherpa\/DE_8TeV_Sherpa_HepMC_num*",  lumi         * 1000.          , 1.,    0,   0,     0,    0,     1.,  jetPtMin,  jetPtMax, ZPtMin , ZEtaMin,    ZEtaMax );
        //    //DESherpaTest.Loop(0, 1, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy); 

        //}

        if (DoWhat == PDF) {
            for (int pdfMember(0); pdfMember <= 0; pdfMember++) {
                ZJetsAndDPS DMuDYMixPDF(lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR", lumi*3531.8*1000/30459503., 1., 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
                //DMuDYMixPDF.Loop(0, 1, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy, 0, 0, 1, 0, "CT10.LHgrid", pdfMember);
                DMuDYMixPDF.Loop(0, 1, doQCD, doSameSign, doInvMassCut, doBJets, doPUStudy, 0, 0, 1, 0, "MSTW2008nlo68cl.LHgrid", pdfMember);
            }
        }

        // now produce files for pulls
        if (DoWhat == PULL) {
            int doGen = 1;
            doDataEff = 1;
            int NPulls = 25;
            for (int loopPull(0); loopPull < NPulls; loopPull++) {
                if (lepSel == "DMu" || lepSel == "DE") {
                    ZJetsAndDPS DMuDYMixPull(lepSel + "_8TeV_DYJets_MIX_UNFOLDING_dR_5311_Inf3", lumi*3531.8*1000/30459503., 1., 1, !doDataEff, 0, 0, 1, jetPtMin, jetPtMax, ZPtMin, ZEtaMin, ZEtaMax);
                    DMuDYMixPull.Loop(1, 1, 0, 0, 0, 0, -10, 0, 0, 1, 0, "", 0, loopPull, NPulls);
                }
            }
        }

}
