#ifndef BonzaiMaker_h
#define BonzaiMaker_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


class BonzaiMaker {
    public :
        TChain          *fChain;   //!pointer to the analyzed TTree or TChain
        Int_t           fCurrent; //!current Tree number in a TChain

        // Declaration of leaf types
        Int_t           run;
        Int_t           lumi;
        Int_t           event;
        Int_t           cat;
        Int_t           f_bits;
        Int_t           tn;
        Bool_t          t_bits[15];   //[tn]
        Int_t           t_prescale[15];   //[tn]
        Int_t           nvtx;
        Float_t         instLumi;
        Float_t         rho;
        Float_t         rho25;
        Int_t           pfn;
        Int_t           pf_id[605];   //[pfn]
        Int_t           pf_charge[605];   //[pfn]
        Float_t         pf_px[605];   //[pfn]
        Float_t         pf_py[605];   //[pfn]
        Float_t         pf_pz[605];   //[pfn]
        Float_t         pf_en[605];   //[pfn]
        Float_t         pf_d0[605];   //[pfn]
        Float_t         pf_d0err[605];   //[pfn]
        Float_t         pf_dZ[605];   //[pfn]
        Float_t         pf_dZerr[605];   //[pfn]
        Int_t           ngenITpu;
        Int_t           ngenOOTpu;
        Int_t           ngenOOTpum1;
        Int_t           ngenTruepu;
        Float_t         pthat;
        Float_t         genWeight;
        Float_t         qscale;
        Float_t         x1;
        Float_t         x2;
        Int_t           id1;
        Int_t           id2;
        Int_t           nup;
        Int_t           mcn;
        Int_t           mc_id[291];   //[mcn]
        Int_t           mc_status[291];   //[mcn]
        Int_t           mc_mother[291];   //[mcn]
        Float_t         mc_px[291];   //[mcn]
        Float_t         mc_py[291];   //[mcn]
        Float_t         mc_pz[291];   //[mcn]
        Float_t         mc_en[291];   //[mcn]
        Float_t         mc_lxy[291];   //[mcn]
        Int_t           ln;
        Int_t           ln_id[5];   //[ln]
        Int_t           ln_idbits[5];   //[ln]
        Int_t           ln_pid[5];   //[ln]
        Int_t           ln_genid[5];   //[ln]
        Int_t           ln_Tbits[5];   //[ln]
        Float_t         ln_px[5];   //[ln]
        Float_t         ln_py[5];   //[ln]
        Float_t         ln_pz[5];   //[ln]
        Float_t         ln_en[5];   //[ln]
        Float_t         ln_genpx[5];   //[ln]
        Float_t         ln_genpy[5];   //[ln]
        Float_t         ln_genpz[5];   //[ln]
        Float_t         ln_genen[5];   //[ln]
        Float_t         ln_ecalIso03[5];   //[ln]
        Float_t         ln_hcalIso03[5];   //[ln]
        Float_t         ln_trkIso03[5];   //[ln]
        Float_t         ln_gIso03[5];   //[ln]
        Float_t         ln_chIso03[5];   //[ln]
        Float_t         ln_puchIso03[5];   //[ln]
        Float_t         ln_nhIso03[5];   //[ln]
        Float_t         ln_ecalIso04[5];   //[ln]
        Float_t         ln_hcalIso04[5];   //[ln]
        Float_t         ln_trkIso04[5];   //[ln]
        Float_t         ln_gIso04[5];   //[ln]
        Float_t         ln_chIso04[5];   //[ln]
        Float_t         ln_puchIso04[5];   //[ln]
        Float_t         ln_nhIso04[5];   //[ln]
        Float_t         ln_d0[5];   //[ln]
        Float_t         ln_dZ[5];   //[ln]
        Float_t         ln_ip3d[5];   //[ln]
        Float_t         ln_ip3dsig[5];   //[ln]
        Float_t         ln_trkpt[5];   //[ln]
        Float_t         ln_trketa[5];   //[ln]
        Float_t         ln_trkphi[5];   //[ln]
        Float_t         ln_trkchi2[5];   //[ln]
        Float_t         ln_trkValidPixelHits[5];   //[ln]
        Float_t         ln_trkValidTrackerHits[5];   //[ln]
        Float_t         ln_trkLostInnerHits[5];   //[ln]
        Float_t         ln_trkPtErr[5];   //[ln]
        Int_t           gn;
        Int_t           gn_idbits[6];   //[gn]
        Int_t           gn_pid[6];   //[gn]
        Float_t         gn_px[6];   //[gn]
        Float_t         gn_py[6];   //[gn]
        Float_t         gn_pz[6];   //[gn]
        Float_t         gn_en[6];   //[gn]
        Float_t         gn_ecalIso03[6];   //[gn]
        Float_t         gn_hcalIso03[6];   //[gn]
        Float_t         gn_trkIso03[6];   //[gn]
        Float_t         gn_gIso03[6];   //[gn]
        Float_t         gn_chIso03[6];   //[gn]
        Float_t         gn_puchIso03[6];   //[gn]
        Float_t         gn_nhIso03[6];   //[gn]
        Float_t         gn_ecalIso04[6];   //[gn]
        Float_t         gn_hcalIso04[6];   //[gn]
        Float_t         gn_trkIso04[6];   //[gn]
        Float_t         gn_gIso04[6];   //[gn]
        Float_t         gn_chIso04[6];   //[gn]
        Float_t         gn_puchIso04[6];   //[gn]
        Float_t         gn_nhIso04[6];   //[gn]
        Int_t           egn;
        Bool_t          egn_isConv[6];   //[egn]
        Float_t         egn_hoe[6];   //[egn]
        Float_t         egn_h2te[6];   //[egn]
        Float_t         egn_dphiin[6];   //[egn]
        Float_t         egn_detain[6];   //[egn]
        Float_t         egn_sihih[6];   //[egn]
        Float_t         egn_sipip[6];   //[egn]
        Float_t         egn_sihip[6];   //[egn]
        Float_t         egn_eopin[6];   //[egn]
        Float_t         egn_eopout[6];   //[egn]
        Float_t         egn_r9[6];   //[egn]
        Float_t         egn_fbrem[6];   //[egn]
        Float_t         egn_sce[6];   //[egn]
        Float_t         egn_sceta[6];   //[egn]
        Float_t         egn_scphi[6];   //[egn]
        Float_t         egn_ooemoop[6];   //[egn]
        Float_t         egn_mvatrigv0[6];   //[egn]
        Float_t         egn_mvanontrigv0[6];   //[egn]
        Int_t           mn;
        Float_t         mn_nMatches[5];   //[mn]
        Float_t         mn_nMatchedStations[5];   //[mn]
        Float_t         mn_validMuonHits[5];   //[mn]
        Float_t         mn_innerTrackChi2[5];   //[mn]
        Float_t         mn_trkLayersWithMeasurement[5];   //[mn]
        Float_t         mn_pixelLayersWithMeasurement[5];   //[mn]
        Int_t           jn;
        Int_t           jn_idbits[30];   //[jn]
        Int_t           jn_pfstart[30];   //[jn]
        Int_t           jn_pfend[30];   //[jn]
        Float_t         jn_px[30];   //[jn]
        Float_t         jn_py[30];   //[jn]
        Float_t         jn_pz[30];   //[jn]
        Float_t         jn_en[30];   //[jn]
        Float_t         jn_torawsf[30];   //[jn]
        Float_t         jn_neutHadFrac[30];   //[jn]
        Float_t         jn_neutEmFrac[30];   //[jn]
        Float_t         jn_chHadFrac[30];   //[jn]
        Float_t         jn_muFrac[30];   //[jn]
        Float_t         jn_area[30];   //[jn]
        Float_t         jn_tchp[30];   //[jn]
        Float_t         jn_jp[30];   //[jn]
        Float_t         jn_origcsv[30];   //[jn]
        Float_t         jn_csv[30];   //[jn]
        Float_t         jn_jpcsv[30];   //[jn]
        Float_t         jn_slcsv[30];   //[jn]
        Float_t         jn_supercsv[30];   //[jn]
        Float_t         jn_ssvhe[30];   //[jn]
        Float_t         jn_ivf[30];   //[jn]
        Float_t         jn_svxPx[30];   //[jn]
        Float_t         jn_svxPy[30];   //[jn]
        Float_t         jn_svxPz[30];   //[jn]
        Float_t         jn_svxM[30];   //[jn]
        Float_t         jn_svxNtrk[30];   //[jn]
        Float_t         jn_svxLxy[30];   //[jn]
        Float_t         jn_svxLxyErr[30];   //[jn]
        Float_t         jn_ivfPx[30];   //[jn]
        Float_t         jn_ivfPy[30];   //[jn]
        Float_t         jn_ivfPz[30];   //[jn]
        Float_t         jn_ivfM[30];   //[jn]
        Float_t         jn_ivfNtrk[30];   //[jn]
        Float_t         jn_ivfLxy[30];   //[jn]
        Float_t         jn_ivfLxyErr[30];   //[jn]
        Float_t         jn_puMVA[30];   //[jn]
        Float_t         jn_qgMVA[30];   //[jn]
        Float_t         jn_beta[30];   //[jn]
        Float_t         jn_betaStar[30];   //[jn]
        Float_t         jn_dRMean[30];   //[jn]
        Float_t         jn_dR2Mean[30];   //[jn]
        Float_t         jn_ptRMS[30];   //[jn]
        Float_t         jn_ptD[30];   //[jn]
        Float_t         jn_etaW[30];   //[jn]
        Float_t         jn_phiW[30];   //[jn]
        Int_t           jn_genflav[30];   //[jn]
        Int_t           jn_genid[30];   //[jn]
        Float_t         jn_genpx[30];   //[jn]
        Float_t         jn_genpy[30];   //[jn]
        Float_t         jn_genpz[30];   //[jn]
        Float_t         jn_genen[30];   //[jn]
        Float_t         jn_genjpx[30];   //[jn]
        Float_t         jn_genjpy[30];   //[jn]
        Float_t         jn_genjpz[30];   //[jn]
        Float_t         jn_genjen[30];   //[jn]
        Int_t           jnUnf;
        Float_t         jn_genUnfjpx[20];   //[jnUnf]
        Float_t         jn_genUnfjpy[20];   //[jnUnf]
        Float_t         jn_genUnfjpz[20];   //[jnUnf]
        Float_t         jn_genUnfjen[20];   //[jnUnf]
        Float_t         jn_genUnfjhad[20];   //[jnUnf]
        Float_t         jn_genUnfjptcf[20];   //[jnUnf]
        Int_t           metn;
        Float_t         met_phi[4];   //[metn]
        Float_t         met_pt[4];   //[metn]
        Float_t         met_sig[4];   //[metn]
        Float_t         met_sigx2[4];   //[metn]
        Float_t         met_sigxy[4];   //[metn]
        Float_t         met_sigy2[4];   //[metn]

        // List of branches
        TBranch        *b_run;   //!
        TBranch        *b_lumi;   //!
        TBranch        *b_event;   //!
        TBranch        *b_cat;   //!
        TBranch        *b_f_bits;   //!
        TBranch        *b_tn;   //!
        TBranch        *b_t_bits;   //!
        TBranch        *b_t_prescale;   //!
        TBranch        *b_nvtx;   //!
        TBranch        *b_instLumi;   //!
        TBranch        *b_rho;   //!
        TBranch        *b_rho25;   //!
        TBranch        *b_pfn;   //!
        TBranch        *b_pf_id;   //!
        TBranch        *b_pf_charge;   //!
        TBranch        *b_pf_px;   //!
        TBranch        *b_pf_py;   //!
        TBranch        *b_pf_pz;   //!
        TBranch        *b_pf_en;   //!
        TBranch        *b_pf_d0;   //!
        TBranch        *b_pf_d0err;   //!
        TBranch        *b_pf_dZ;   //!
        TBranch        *b_pf_dZerr;   //!
        TBranch        *b_ngenITpu;   //!
        TBranch        *b_ngenOOTpu;   //!
        TBranch        *b_ngenOOTpum1;   //!
        TBranch        *b_ngenTruepu;   //!
        TBranch        *b_pthat;   //!
        TBranch        *b_genWeight;   //!
        TBranch        *b_qscale;   //!
        TBranch        *b_x1;   //!
        TBranch        *b_x2;   //!
        TBranch        *b_id1;   //!
        TBranch        *b_id2;   //!
        TBranch        *b_nup;   //!
        TBranch        *b_mcn;   //!
        TBranch        *b_mc_id;   //!
        TBranch        *b_mc_status;   //!
        TBranch        *b_mc_mother;   //!
        TBranch        *b_mc_px;   //!
        TBranch        *b_mc_py;   //!
        TBranch        *b_mc_pz;   //!
        TBranch        *b_mc_en;   //!
        TBranch        *b_mc_lxy;   //!
        TBranch        *b_ln;   //!
        TBranch        *b_ln_id;   //!
        TBranch        *b_ln_idbits;   //!
        TBranch        *b_ln_pid;   //!
        TBranch        *b_ln_genid;   //!
        TBranch        *b_ln_Tbits;   //!
        TBranch        *b_ln_px;   //!
        TBranch        *b_ln_py;   //!
        TBranch        *b_ln_pz;   //!
        TBranch        *b_ln_en;   //!
        TBranch        *b_ln_genpx;   //!
        TBranch        *b_ln_genpy;   //!
        TBranch        *b_ln_genpz;   //!
        TBranch        *b_ln_genen;   //!
        TBranch        *b_ln_ecalIso03;   //!
        TBranch        *b_ln_hcalIso03;   //!
        TBranch        *b_ln_trkIso03;   //!
        TBranch        *b_ln_gIso03;   //!
        TBranch        *b_ln_chIso03;   //!
        TBranch        *b_ln_puchIso03;   //!
        TBranch        *b_ln_nhIso03;   //!
        TBranch        *b_ln_ecalIso04;   //!
        TBranch        *b_ln_hcalIso04;   //!
        TBranch        *b_ln_trkIso04;   //!
        TBranch        *b_ln_gIso04;   //!
        TBranch        *b_ln_chIso04;   //!
        TBranch        *b_ln_puchIso04;   //!
        TBranch        *b_ln_nhIso04;   //!
        TBranch        *b_ln_d0;   //!
        TBranch        *b_ln_dZ;   //!
        TBranch        *b_ln_ip3d;   //!
        TBranch        *b_ln_ip3dsig;   //!
        TBranch        *b_ln_trkpt;   //!
        TBranch        *b_ln_trketa;   //!
        TBranch        *b_ln_trkphi;   //!
        TBranch        *b_ln_trkchi2;   //!
        TBranch        *b_ln_trkValidPixelHits;   //!
        TBranch        *b_ln_trkValidTrackerHits;   //!
        TBranch        *b_ln_trkLostInnerHits;   //!
        TBranch        *b_ln_trkPtErr;   //!
        TBranch        *b_gn;   //!
        TBranch        *b_gn_idbits;   //!
        TBranch        *b_gn_pid;   //!
        TBranch        *b_gn_px;   //!
        TBranch        *b_gn_py;   //!
        TBranch        *b_gn_pz;   //!
        TBranch        *b_gn_en;   //!
        TBranch        *b_gn_ecalIso03;   //!
        TBranch        *b_gn_hcalIso03;   //!
        TBranch        *b_gn_trkIso03;   //!
        TBranch        *b_gn_gIso03;   //!
        TBranch        *b_gn_chIso03;   //!
        TBranch        *b_gn_puchIso03;   //!
        TBranch        *b_gn_nhIso03;   //!
        TBranch        *b_gn_ecalIso04;   //!
        TBranch        *b_gn_hcalIso04;   //!
        TBranch        *b_gn_trkIso04;   //!
        TBranch        *b_gn_gIso04;   //!
        TBranch        *b_gn_chIso04;   //!
        TBranch        *b_gn_puchIso04;   //!
        TBranch        *b_gn_nhIso04;   //!
        TBranch        *b_egn;   //!
        TBranch        *b_egn_isConv;   //!
        TBranch        *b_egn_hoe;   //!
        TBranch        *b_egn_h2te;   //!
        TBranch        *b_egn_dphiin;   //!
        TBranch        *b_egn_detain;   //!
        TBranch        *b_egn_sihih;   //!
        TBranch        *b_egn_sipip;   //!
        TBranch        *b_egn_sihip;   //!
        TBranch        *b_egn_eopin;   //!
        TBranch        *b_egn_eopout;   //!
        TBranch        *b_egn_r9;   //!
        TBranch        *b_egn_fbrem;   //!
        TBranch        *b_egn_sce;   //!
        TBranch        *b_egn_sceta;   //!
        TBranch        *b_egn_scphi;   //!
        TBranch        *b_egn_ooemoop;   //!
        TBranch        *b_egn_mvatrigv0;   //!
        TBranch        *b_egn_mvanontrigv0;   //!
        TBranch        *b_mn;   //!
        TBranch        *b_mn_nMatches;   //!
        TBranch        *b_mn_nMatchedStations;   //!
        TBranch        *b_mn_validMuonHits;   //!
        TBranch        *b_mn_innerTrackChi2;   //!
        TBranch        *b_mn_trkLayersWithMeasurement;   //!
        TBranch        *b_mn_pixelLayersWithMeasurement;   //!
        TBranch        *b_jn;   //!
        TBranch        *b_jn_idbits;   //!
        TBranch        *b_jn_pfstart;   //!
        TBranch        *b_jn_pfend;   //!
        TBranch        *b_jn_px;   //!
        TBranch        *b_jn_py;   //!
        TBranch        *b_jn_pz;   //!
        TBranch        *b_jn_en;   //!
        TBranch        *b_jn_torawsf;   //!
        TBranch        *b_jn_neutHadFrac;   //!
        TBranch        *b_jn_neutEmFrac;   //!
        TBranch        *b_jn_chHadFrac;   //!
        TBranch        *b_jn_muFrac;   //!
        TBranch        *b_jn_area;   //!
        TBranch        *b_jn_tchp;   //!
        TBranch        *b_jn_jp;   //!
        TBranch        *b_jn_origcsv;   //!
        TBranch        *b_jn_csv;   //!
        TBranch        *b_jn_jpcsv;   //!
        TBranch        *b_jn_slcsv;   //!
        TBranch        *b_jn_supercsv;   //!
        TBranch        *b_jn_ssvhe;   //!
        TBranch        *b_jn_ivf;   //!
        TBranch        *b_jn_svxPx;   //!
        TBranch        *b_jn_svxPy;   //!
        TBranch        *b_jn_svxPz;   //!
        TBranch        *b_jn_svxM;   //!
        TBranch        *b_jn_svxNtrk;   //!
        TBranch        *b_jn_svxLxy;   //!
        TBranch        *b_jn_svxLxyErr;   //!
        TBranch        *b_jn_ivfPx;   //!
        TBranch        *b_jn_ivfPy;   //!
        TBranch        *b_jn_ivfPz;   //!
        TBranch        *b_jn_ivfM;   //!
        TBranch        *b_jn_ivfNtrk;   //!
        TBranch        *b_jn_ivfLxy;   //!
        TBranch        *b_jn_ivfLxyErr;   //!
        TBranch        *b_jn_puMVA;   //!
        TBranch        *b_jn_qgMVA;   //!
        TBranch        *b_jn_beta;   //!
        TBranch        *b_jn_betaStar;   //!
        TBranch        *b_jn_dRMean;   //!
        TBranch        *b_jn_dR2Mean;   //!
        TBranch        *b_jn_ptRMS;   //!
        TBranch        *b_jn_ptD;   //!
        TBranch        *b_jn_etaW;   //!
        TBranch        *b_jn_phiW;   //!
        TBranch        *b_jn_genflav;   //!
        TBranch        *b_jn_genid;   //!
        TBranch        *b_jn_genpx;   //!
        TBranch        *b_jn_genpy;   //!
        TBranch        *b_jn_genpz;   //!
        TBranch        *b_jn_genen;   //!
        TBranch        *b_jn_genjpx;   //!
        TBranch        *b_jn_genjpy;   //!
        TBranch        *b_jn_genjpz;   //!
        TBranch        *b_jn_genjen;   //!
        TBranch        *b_jnUnf;   //!
        TBranch        *b_jn_genUnfjpx;   //!
        TBranch        *b_jn_genUnfjpy;   //!
        TBranch        *b_jn_genUnfjpz;   //!
        TBranch        *b_jn_genUnfjen;   //!
        TBranch        *b_jn_genUnfjhad;   //!
        TBranch        *b_jn_genUnfjptcf;   //!
        TBranch        *b_metn;   //!
        TBranch        *b_met_phi;   //!
        TBranch        *b_met_pt;   //!
        TBranch        *b_met_sig;   //!
        TBranch        *b_met_sigx2;   //!
        TBranch        *b_met_sigxy;   //!
        TBranch        *b_met_sigy2;   //!

        BonzaiMaker(string dataset_, string fileName_ );
        virtual ~BonzaiMaker();
        virtual Int_t GetEntry(Long64_t entry);
        virtual Long64_t LoadTree(Long64_t entry);
        virtual void Init(TChain *tree);
        virtual void Loop(string dataset_, string fileName_, int leptonIdSum = 1, bool doUnfold =0, bool hasRecoInfo = 1, bool hasGenInfo = 1);

        virtual Bool_t Notify();
        virtual void Show(Long64_t entry = -1);
        string storageElement;
};

#endif

#ifdef BonzaiMaker_cxx
BonzaiMaker::BonzaiMaker(string dataset_, string fileName_) : fChain(0)
{
    // if parameter tree is not specified (or zero), connect the file
    // used to generate this class and read the Tree.

    storageElement = "dcap://maite.iihe.ac.be:/pnfs/iihe/cms/store/user/aleonard/VJets_ntuples_2015/";
    //storageElement = "dcap://maite.iihe.ac.be:/pnfs/iihe/cms/store/user/aleonard/VJets_ntuples/";
    string dirPath = "/dataAnalyzer";
    string treeName = "/data";

    //string fullFileName = "~/VJets_March2014/CMSSW_5_3_11/src/UserCode/llvv_fwk/test/grid/multicrab_data/MC8TeV_ZZJets2L2Q/res/Events_176_1_6jp.root";
    string fullFileName = "dcap://maite.iihe.ac.be:/pnfs/iihe/cms/store/user/aleonard/store/cmst3/user/psilva/tmp/MC8TeV_ZZJets4L/Events_422_1_S6f.root/dataAnalyzer/data";


    fChain = new TChain();
    if (fileName_.find(".txt") != string::npos) {
        ifstream listOfFiles(fileName_.c_str());
        if (listOfFiles.is_open()) {
            while (listOfFiles.good()) {
                string line;
                getline(listOfFiles, line);
                if (line != "") {
                    fullFileName = storageElement + dataset_ + line + dirPath + treeName;  
                    std::cout << fullFileName << std::endl;
                    fChain->Add(fullFileName.c_str());
                }
            }
        }
    }
    else {
        fChain->Add(fullFileName.c_str());
    }


    Init(fChain);
}  


BonzaiMaker::~BonzaiMaker()
{
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

Int_t BonzaiMaker::GetEntry(Long64_t entry)
{
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}
Long64_t BonzaiMaker::LoadTree(Long64_t entry)
{
    // Set the environment to read one entry
    if (!fChain) return -5;
    Long64_t centry = fChain->LoadTree(entry);
    if (centry < 0) return centry;
    if (fChain->GetTreeNumber() != fCurrent) {
        fCurrent = fChain->GetTreeNumber();
        Notify();
    }
    return centry;
}

void BonzaiMaker::Init(TChain *tree)
{
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses and branch
    // pointers of the tree will be set.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    // Set branch addresses and branch pointers
    if (!tree) return;
    fChain = tree;
    fCurrent = -1;
    fChain->SetMakeClass(1);

    fChain->SetBranchAddress("run", &run, &b_run);
    fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
    fChain->SetBranchAddress("event", &event, &b_event);
    fChain->SetBranchAddress("cat", &cat, &b_cat);
    fChain->SetBranchAddress("f_bits", &f_bits, &b_f_bits);
    fChain->SetBranchAddress("tn", &tn, &b_tn);
    fChain->SetBranchAddress("t_bits", t_bits, &b_t_bits);
    fChain->SetBranchAddress("t_prescale", t_prescale, &b_t_prescale);
    fChain->SetBranchAddress("nvtx", &nvtx, &b_nvtx);
    fChain->SetBranchAddress("instLumi", &instLumi, &b_instLumi);
    fChain->SetBranchAddress("rho", &rho, &b_rho);
    fChain->SetBranchAddress("rho25", &rho25, &b_rho25);
    fChain->SetBranchAddress("pfn", &pfn, &b_pfn);
    fChain->SetBranchAddress("pf_id", pf_id, &b_pf_id);
    fChain->SetBranchAddress("pf_charge", pf_charge, &b_pf_charge);
    fChain->SetBranchAddress("pf_px", pf_px, &b_pf_px);
    fChain->SetBranchAddress("pf_py", pf_py, &b_pf_py);
    fChain->SetBranchAddress("pf_pz", pf_pz, &b_pf_pz);
    fChain->SetBranchAddress("pf_en", pf_en, &b_pf_en);
    fChain->SetBranchAddress("pf_d0", pf_d0, &b_pf_d0);
    fChain->SetBranchAddress("pf_d0err", pf_d0err, &b_pf_d0err);
    fChain->SetBranchAddress("pf_dZ", pf_dZ, &b_pf_dZ);
    fChain->SetBranchAddress("pf_dZerr", pf_dZerr, &b_pf_dZerr);
    fChain->SetBranchAddress("ngenITpu", &ngenITpu, &b_ngenITpu);
    fChain->SetBranchAddress("ngenOOTpu", &ngenOOTpu, &b_ngenOOTpu);
    fChain->SetBranchAddress("ngenOOTpum1", &ngenOOTpum1, &b_ngenOOTpum1);
    fChain->SetBranchAddress("ngenTruepu", &ngenTruepu, &b_ngenTruepu);
    fChain->SetBranchAddress("pthat", &pthat, &b_pthat);
    fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
    fChain->SetBranchAddress("qscale", &qscale, &b_qscale);
    fChain->SetBranchAddress("x1", &x1, &b_x1);
    fChain->SetBranchAddress("x2", &x2, &b_x2);
    fChain->SetBranchAddress("id1", &id1, &b_id1);
    fChain->SetBranchAddress("id2", &id2, &b_id2);
    fChain->SetBranchAddress("nup", &nup, &b_nup);
    fChain->SetBranchAddress("mcn", &mcn, &b_mcn);
    fChain->SetBranchAddress("mc_id", mc_id, &b_mc_id);
    fChain->SetBranchAddress("mc_status", mc_status, &b_mc_status);
    fChain->SetBranchAddress("mc_mother", mc_mother, &b_mc_mother);
    fChain->SetBranchAddress("mc_px", mc_px, &b_mc_px);
    fChain->SetBranchAddress("mc_py", mc_py, &b_mc_py);
    fChain->SetBranchAddress("mc_pz", mc_pz, &b_mc_pz);
    fChain->SetBranchAddress("mc_en", mc_en, &b_mc_en);
    fChain->SetBranchAddress("mc_lxy", mc_lxy, &b_mc_lxy);
    fChain->SetBranchAddress("ln", &ln, &b_ln);
    fChain->SetBranchAddress("ln_id", ln_id, &b_ln_id);
    fChain->SetBranchAddress("ln_idbits", ln_idbits, &b_ln_idbits);
    fChain->SetBranchAddress("ln_pid", ln_pid, &b_ln_pid);
    fChain->SetBranchAddress("ln_genid", ln_genid, &b_ln_genid);
    fChain->SetBranchAddress("ln_Tbits", ln_Tbits, &b_ln_Tbits);
    fChain->SetBranchAddress("ln_px", ln_px, &b_ln_px);
    fChain->SetBranchAddress("ln_py", ln_py, &b_ln_py);
    fChain->SetBranchAddress("ln_pz", ln_pz, &b_ln_pz);
    fChain->SetBranchAddress("ln_en", ln_en, &b_ln_en);
    fChain->SetBranchAddress("ln_genpx", ln_genpx, &b_ln_genpx);
    fChain->SetBranchAddress("ln_genpy", ln_genpy, &b_ln_genpy);
    fChain->SetBranchAddress("ln_genpz", ln_genpz, &b_ln_genpz);
    fChain->SetBranchAddress("ln_genen", ln_genen, &b_ln_genen);
    fChain->SetBranchAddress("ln_ecalIso03", ln_ecalIso03, &b_ln_ecalIso03);
    fChain->SetBranchAddress("ln_hcalIso03", ln_hcalIso03, &b_ln_hcalIso03);
    fChain->SetBranchAddress("ln_trkIso03", ln_trkIso03, &b_ln_trkIso03);
    fChain->SetBranchAddress("ln_gIso03", ln_gIso03, &b_ln_gIso03);
    fChain->SetBranchAddress("ln_chIso03", ln_chIso03, &b_ln_chIso03);
    fChain->SetBranchAddress("ln_puchIso03", ln_puchIso03, &b_ln_puchIso03);
    fChain->SetBranchAddress("ln_nhIso03", ln_nhIso03, &b_ln_nhIso03);
    fChain->SetBranchAddress("ln_ecalIso04", ln_ecalIso04, &b_ln_ecalIso04);
    fChain->SetBranchAddress("ln_hcalIso04", ln_hcalIso04, &b_ln_hcalIso04);
    fChain->SetBranchAddress("ln_trkIso04", ln_trkIso04, &b_ln_trkIso04);
    fChain->SetBranchAddress("ln_gIso04", ln_gIso04, &b_ln_gIso04);
    fChain->SetBranchAddress("ln_chIso04", ln_chIso04, &b_ln_chIso04);
    fChain->SetBranchAddress("ln_puchIso04", ln_puchIso04, &b_ln_puchIso04);
    fChain->SetBranchAddress("ln_nhIso04", ln_nhIso04, &b_ln_nhIso04);
    fChain->SetBranchAddress("ln_d0", ln_d0, &b_ln_d0);
    fChain->SetBranchAddress("ln_dZ", ln_dZ, &b_ln_dZ);
    fChain->SetBranchAddress("ln_ip3d", ln_ip3d, &b_ln_ip3d);
    fChain->SetBranchAddress("ln_ip3dsig", ln_ip3dsig, &b_ln_ip3dsig);
    fChain->SetBranchAddress("ln_trkpt", ln_trkpt, &b_ln_trkpt);
    fChain->SetBranchAddress("ln_trketa", ln_trketa, &b_ln_trketa);
    fChain->SetBranchAddress("ln_trkphi", ln_trkphi, &b_ln_trkphi);
    fChain->SetBranchAddress("ln_trkchi2", ln_trkchi2, &b_ln_trkchi2);
    fChain->SetBranchAddress("ln_trkValidPixelHits", ln_trkValidPixelHits, &b_ln_trkValidPixelHits);
    fChain->SetBranchAddress("ln_trkValidTrackerHits", ln_trkValidTrackerHits, &b_ln_trkValidTrackerHits);
    fChain->SetBranchAddress("ln_trkLostInnerHits", ln_trkLostInnerHits, &b_ln_trkLostInnerHits);
    fChain->SetBranchAddress("ln_trkPtErr", ln_trkPtErr, &b_ln_trkPtErr);
    fChain->SetBranchAddress("gn", &gn, &b_gn);
    fChain->SetBranchAddress("gn_idbits", gn_idbits, &b_gn_idbits);
    fChain->SetBranchAddress("gn_pid", gn_pid, &b_gn_pid);
    fChain->SetBranchAddress("gn_px", gn_px, &b_gn_px);
    fChain->SetBranchAddress("gn_py", gn_py, &b_gn_py);
    fChain->SetBranchAddress("gn_pz", gn_pz, &b_gn_pz);
    fChain->SetBranchAddress("gn_en", gn_en, &b_gn_en);
    fChain->SetBranchAddress("gn_ecalIso03", gn_ecalIso03, &b_gn_ecalIso03);
    fChain->SetBranchAddress("gn_hcalIso03", gn_hcalIso03, &b_gn_hcalIso03);
    fChain->SetBranchAddress("gn_trkIso03", gn_trkIso03, &b_gn_trkIso03);
    fChain->SetBranchAddress("gn_gIso03", gn_gIso03, &b_gn_gIso03);
    fChain->SetBranchAddress("gn_chIso03", gn_chIso03, &b_gn_chIso03);
    fChain->SetBranchAddress("gn_puchIso03", gn_puchIso03, &b_gn_puchIso03);
    fChain->SetBranchAddress("gn_nhIso03", gn_nhIso03, &b_gn_nhIso03);
    fChain->SetBranchAddress("gn_ecalIso04", gn_ecalIso04, &b_gn_ecalIso04);
    fChain->SetBranchAddress("gn_hcalIso04", gn_hcalIso04, &b_gn_hcalIso04);
    fChain->SetBranchAddress("gn_trkIso04", gn_trkIso04, &b_gn_trkIso04);
    fChain->SetBranchAddress("gn_gIso04", gn_gIso04, &b_gn_gIso04);
    fChain->SetBranchAddress("gn_chIso04", gn_chIso04, &b_gn_chIso04);
    fChain->SetBranchAddress("gn_puchIso04", gn_puchIso04, &b_gn_puchIso04);
    fChain->SetBranchAddress("gn_nhIso04", gn_nhIso04, &b_gn_nhIso04);
    fChain->SetBranchAddress("egn", &egn, &b_egn);
    fChain->SetBranchAddress("egn_isConv", egn_isConv, &b_egn_isConv);
    fChain->SetBranchAddress("egn_hoe", egn_hoe, &b_egn_hoe);
    fChain->SetBranchAddress("egn_h2te", egn_h2te, &b_egn_h2te);
    fChain->SetBranchAddress("egn_dphiin", egn_dphiin, &b_egn_dphiin);
    fChain->SetBranchAddress("egn_detain", egn_detain, &b_egn_detain);
    fChain->SetBranchAddress("egn_sihih", egn_sihih, &b_egn_sihih);
    fChain->SetBranchAddress("egn_sipip", egn_sipip, &b_egn_sipip);
    fChain->SetBranchAddress("egn_sihip", egn_sihip, &b_egn_sihip);
    fChain->SetBranchAddress("egn_eopin", egn_eopin, &b_egn_eopin);
    fChain->SetBranchAddress("egn_eopout", egn_eopout, &b_egn_eopout);
    fChain->SetBranchAddress("egn_r9", egn_r9, &b_egn_r9);
    fChain->SetBranchAddress("egn_fbrem", egn_fbrem, &b_egn_fbrem);
    fChain->SetBranchAddress("egn_sce", egn_sce, &b_egn_sce);
    fChain->SetBranchAddress("egn_sceta", egn_sceta, &b_egn_sceta);
    fChain->SetBranchAddress("egn_scphi", egn_scphi, &b_egn_scphi);
    fChain->SetBranchAddress("egn_ooemoop", egn_ooemoop, &b_egn_ooemoop);
    fChain->SetBranchAddress("egn_mvatrigv0", egn_mvatrigv0, &b_egn_mvatrigv0);
    fChain->SetBranchAddress("egn_mvanontrigv0", egn_mvanontrigv0, &b_egn_mvanontrigv0);
    fChain->SetBranchAddress("mn", &mn, &b_mn);
    fChain->SetBranchAddress("mn_nMatches", mn_nMatches, &b_mn_nMatches);
    fChain->SetBranchAddress("mn_nMatchedStations", mn_nMatchedStations, &b_mn_nMatchedStations);
    fChain->SetBranchAddress("mn_validMuonHits", mn_validMuonHits, &b_mn_validMuonHits);
    fChain->SetBranchAddress("mn_innerTrackChi2", mn_innerTrackChi2, &b_mn_innerTrackChi2);
    fChain->SetBranchAddress("mn_trkLayersWithMeasurement", mn_trkLayersWithMeasurement, &b_mn_trkLayersWithMeasurement);
    fChain->SetBranchAddress("mn_pixelLayersWithMeasurement", mn_pixelLayersWithMeasurement, &b_mn_pixelLayersWithMeasurement);
    fChain->SetBranchAddress("jn", &jn, &b_jn);
    fChain->SetBranchAddress("jn_idbits", jn_idbits, &b_jn_idbits);
    fChain->SetBranchAddress("jn_pfstart", jn_pfstart, &b_jn_pfstart);
    fChain->SetBranchAddress("jn_pfend", jn_pfend, &b_jn_pfend);
    fChain->SetBranchAddress("jn_px", jn_px, &b_jn_px);
    fChain->SetBranchAddress("jn_py", jn_py, &b_jn_py);
    fChain->SetBranchAddress("jn_pz", jn_pz, &b_jn_pz);
    fChain->SetBranchAddress("jn_en", jn_en, &b_jn_en);
    fChain->SetBranchAddress("jn_torawsf", jn_torawsf, &b_jn_torawsf);
    fChain->SetBranchAddress("jn_neutHadFrac", jn_neutHadFrac, &b_jn_neutHadFrac);
    fChain->SetBranchAddress("jn_neutEmFrac", jn_neutEmFrac, &b_jn_neutEmFrac);
    fChain->SetBranchAddress("jn_chHadFrac", jn_chHadFrac, &b_jn_chHadFrac);
    fChain->SetBranchAddress("jn_muFrac", jn_muFrac, &b_jn_muFrac);
    fChain->SetBranchAddress("jn_area", jn_area, &b_jn_area);
    fChain->SetBranchAddress("jn_tchp", jn_tchp, &b_jn_tchp);
    fChain->SetBranchAddress("jn_jp", jn_jp, &b_jn_jp);
    fChain->SetBranchAddress("jn_origcsv", jn_origcsv, &b_jn_origcsv);
    fChain->SetBranchAddress("jn_csv", jn_csv, &b_jn_csv);
    fChain->SetBranchAddress("jn_jpcsv", jn_jpcsv, &b_jn_jpcsv);
    fChain->SetBranchAddress("jn_slcsv", jn_slcsv, &b_jn_slcsv);
    fChain->SetBranchAddress("jn_supercsv", jn_supercsv, &b_jn_supercsv);
    fChain->SetBranchAddress("jn_ssvhe", jn_ssvhe, &b_jn_ssvhe);
    fChain->SetBranchAddress("jn_ivf", jn_ivf, &b_jn_ivf);
    fChain->SetBranchAddress("jn_svxPx", jn_svxPx, &b_jn_svxPx);
    fChain->SetBranchAddress("jn_svxPy", jn_svxPy, &b_jn_svxPy);
    fChain->SetBranchAddress("jn_svxPz", jn_svxPz, &b_jn_svxPz);
    fChain->SetBranchAddress("jn_svxM", jn_svxM, &b_jn_svxM);
    fChain->SetBranchAddress("jn_svxNtrk", jn_svxNtrk, &b_jn_svxNtrk);
    fChain->SetBranchAddress("jn_svxLxy", jn_svxLxy, &b_jn_svxLxy);
    fChain->SetBranchAddress("jn_svxLxyErr", jn_svxLxyErr, &b_jn_svxLxyErr);
    fChain->SetBranchAddress("jn_ivfPx", jn_ivfPx, &b_jn_ivfPx);
    fChain->SetBranchAddress("jn_ivfPy", jn_ivfPy, &b_jn_ivfPy);
    fChain->SetBranchAddress("jn_ivfPz", jn_ivfPz, &b_jn_ivfPz);
    fChain->SetBranchAddress("jn_ivfM", jn_ivfM, &b_jn_ivfM);
    fChain->SetBranchAddress("jn_ivfNtrk", jn_ivfNtrk, &b_jn_ivfNtrk);
    fChain->SetBranchAddress("jn_ivfLxy", jn_ivfLxy, &b_jn_ivfLxy);
    fChain->SetBranchAddress("jn_ivfLxyErr", jn_ivfLxyErr, &b_jn_ivfLxyErr);
    fChain->SetBranchAddress("jn_puMVA", jn_puMVA, &b_jn_puMVA);
    fChain->SetBranchAddress("jn_qgMVA", jn_qgMVA, &b_jn_qgMVA);
    fChain->SetBranchAddress("jn_beta", jn_beta, &b_jn_beta);
    fChain->SetBranchAddress("jn_betaStar", jn_betaStar, &b_jn_betaStar);
    fChain->SetBranchAddress("jn_dRMean", jn_dRMean, &b_jn_dRMean);
    fChain->SetBranchAddress("jn_dR2Mean", jn_dR2Mean, &b_jn_dR2Mean);
    fChain->SetBranchAddress("jn_ptRMS", jn_ptRMS, &b_jn_ptRMS);
    fChain->SetBranchAddress("jn_ptD", jn_ptD, &b_jn_ptD);
    fChain->SetBranchAddress("jn_etaW", jn_etaW, &b_jn_etaW);
    fChain->SetBranchAddress("jn_phiW", jn_phiW, &b_jn_phiW);
    fChain->SetBranchAddress("jn_genflav", jn_genflav, &b_jn_genflav);
    fChain->SetBranchAddress("jn_genid", jn_genid, &b_jn_genid);
    fChain->SetBranchAddress("jn_genpx", jn_genpx, &b_jn_genpx);
    fChain->SetBranchAddress("jn_genpy", jn_genpy, &b_jn_genpy);
    fChain->SetBranchAddress("jn_genpz", jn_genpz, &b_jn_genpz);
    fChain->SetBranchAddress("jn_genen", jn_genen, &b_jn_genen);
    fChain->SetBranchAddress("jn_genjpx", jn_genjpx, &b_jn_genjpx);
    fChain->SetBranchAddress("jn_genjpy", jn_genjpy, &b_jn_genjpy);
    fChain->SetBranchAddress("jn_genjpz", jn_genjpz, &b_jn_genjpz);
    fChain->SetBranchAddress("jn_genjen", jn_genjen, &b_jn_genjen);
    fChain->SetBranchAddress("jnUnf", &jnUnf, &b_jnUnf);
    fChain->SetBranchAddress("jn_genUnfjpx", jn_genUnfjpx, &b_jn_genUnfjpx);
    fChain->SetBranchAddress("jn_genUnfjpy", jn_genUnfjpy, &b_jn_genUnfjpy);
    fChain->SetBranchAddress("jn_genUnfjpz", jn_genUnfjpz, &b_jn_genUnfjpz);
    fChain->SetBranchAddress("jn_genUnfjen", jn_genUnfjen, &b_jn_genUnfjen);
    fChain->SetBranchAddress("jn_genUnfjhad", jn_genUnfjhad, &b_jn_genUnfjhad);
    fChain->SetBranchAddress("jn_genUnfjptcf", jn_genUnfjptcf, &b_jn_genUnfjptcf);
    fChain->SetBranchAddress("metn", &metn, &b_metn);
    fChain->SetBranchAddress("met_phi", met_phi, &b_met_phi);
    fChain->SetBranchAddress("met_pt", met_pt, &b_met_pt);
    fChain->SetBranchAddress("met_sig", met_sig, &b_met_sig);
    fChain->SetBranchAddress("met_sigx2", met_sigx2, &b_met_sigx2);
    fChain->SetBranchAddress("met_sigxy", met_sigxy, &b_met_sigxy);
    fChain->SetBranchAddress("met_sigy2", met_sigy2, &b_met_sigy2);
    Notify();
}

Bool_t BonzaiMaker::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

void BonzaiMaker::Show(Long64_t entry)
{
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
}

#endif // #ifdef BonzaiMaker_cxx
