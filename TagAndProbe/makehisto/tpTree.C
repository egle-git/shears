#define tpTree_cxx
#include "tpTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void tpTree::Loop()
{
//   In a ROOT session, you can do:
//      root> .L tpTree.C
//      root> tpTree t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;
	TFile *f1 = new TFile("histo.root","RECREATE");
    TH1F *hpa = new TH1F("probe_all","probe_all",100,0,300);
    TH1F *hpp = new TH1F("probe_pass","probe_pass",100,0,300);
    TH1F *hpf = new TH1F("probe_fail","probe_fail",100,0,300);
    TH1F *hpd = new TH1F("probe_eff","probe_eff",100,0,300);
   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      bool passId = Muons_IdTight_probe & (1<<0);
      bool passIso = Muons_PfIso_probe < 0.15;
      hpa->Fill(Muons_Pt_probe);
      if (passId && passIso) hpp->Fill(Muons_Pt_probe);
      else hpf->Fill(Muons_Pt_probe);
   }
   hpd->Divide(hpp,hpa);
   f1->Write();
    f1->Close();
}
