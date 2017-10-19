#define EventTree_cxx
#include "../Includes/EventTree.h"
#include "../Includes/ObjectSelection.h"
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TMath.h>

void EventTree::Loop()
{
   if (fChain == 0) return;

   //###############################################################
   //################## DECLARATION OF HISTOGRAMS ##################
   //###############################################################
   TFile *outFile = new TFile("tpTree.root","RECREATE");
   TTree *tpTree = new TTree("tpTree","tpTree");
   std::vector<Electron>  tagElectrons ;
   std::vector<Electron>  probeElectrons;
   std::vector<Muon>  tagMuons ;
   std::vector<Muon>  probeMuons ;
/*
   Electron tagElectronsforfill ;
   Electron  probeElectronsforfill;
   Muon tagMuonsforfill ;
   Muon probeMuonsforfill ;
  // float Ptofmu;
   TBranch *branch1 = tpTree->Branch("tagMuons", & tagMuonsforfill,"Pt/F:Eta/F:Phi/F:E/F:PfIso/F:Id/I/IdTight/I");
   //TBranch *branch1 = tpTree->Branch("tagMuons", & tagMuonsforfill,"Pt/F:Eta/F:Phi/F:E/F");
   TBranch *branch2 = tpTree->Branch("probeMuons",& probeMuonsforfill,"Pt/F:Eta/F:Phi/F:E/F:PfIso/F:Id/I:IdTight/I");
   TBranch *branch3 = tpTree->Branch("tagElectrons",& tagElectronsforfill,"Pt/F:Eta/F:Phi/F:E/F:EtaSc/F:PfIsoRho/F:Id/I");
   TBranch *branch4 = tpTree->Branch("probeElectrons",& probeElectronsforfill,"Pt/F:Eta/F:Phi/F:E/F:EtaSc/F:PfIsoRho/F:Id/I");
  
  */
   Muonforfill temp;
   TBranch *branch = tpTree->Branch("Muons",& temp,"Zmass:Pt_tag:Pt_probe:Eta_tag:Eta_probe:Phi_tag:Phi_probe:E_tag:E_probe:Id_tag:Id_probe:IdTight_tag:IdTight_probe:PfIso_probe");
   Long64_t nentries = fChain->GetEntries();

   Long64_t nbytes = 0, nb = 0;
   cout << "nb of entries in the input file =" << nentries << endl;

   //###############################################################
   //##################     EVENT LOOP STARTS     ##################
   //###############################################################

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(jentry % 10 ==0) cout << jentry << " of " << nentries << endl;

      //objectSelection::selectElectrons(tagElectrons, probeElectrons, ElPt, ElEta, ElPhi, ElE, ElId, ElEtaSc, ElPfIsoRho);
      objectSelection::selectMuons(tagMuons, probeMuons, MuPt, MuEta, MuPhi, MuE, MuId, MuIdTight, MuPfIso);
      
      
     /* for(int i=0;i<tagElectrons.size();i++)
         for(int j=0;j<probeElectrons.size();j++)
         {
            if(tagElectrons[i].Seq == probeElectrons[j].Seq) continue;// avoiding a pair containing 2 same lep
            float mll;
            mll= (tagElectrons[i].lvector+probeElectrons[j].lvector).M();
            if(mll<60||mll>120) continue;
           tagElectronsforfill = tagElectrons[i];
           probeElectronsforfill = probeElectrons[j];
           branch3->Fill();
            branch4->Fill();
            tpTree->Fill();
         }
      
      */
      for(int i=0;i<tagMuons.size();i++)
         for(int j=0;j<probeMuons.size();j++)
         {
            if(tagMuons[i].Seq == probeMuons[j].Seq) continue;// avoiding a pair containing 2 same lep
            float mll;
            mll= (tagMuons[i].lvector+probeMuons[j].lvector).M();
            if(mll<60||mll>120) continue;
            //tagMuonsforfill.push_back(tagMuons[i]);
            // probeMuonsforfill.push_back(probeMuons[j]);
            //  std::cout << "Muon pt ===" <<tagMuonsforfill[0]->Pt<<endl;
            // tagMuonsforfill = tagMuons[i] ;
            // probeMuonsforfill = probeMuons[j];
            //branch1->Fill();
            //branch2->Fill();
            temp.Zmass = mll;
            temp.Pt_tag = tagMuons[i].Pt;
            temp.Eta_tag = tagMuons[i].Eta;
            temp.Phi_tag = tagMuons[i].Phi;
            temp.E_tag = tagMuons[i].E;
            temp.Pt_probe = probeMuons[j].Pt;
            temp.Eta_probe = probeMuons[j].Eta;
            temp.Phi_probe = probeMuons[j].Phi;
            temp.E_probe = probeMuons[j].E;
            temp.Id_tag = tagMuons[i].Id;
            temp.Id_probe = probeMuons[j].Id;
            temp.IdTight_tag = tagMuons[i].IdTight;
            temp.IdTight_probe = probeMuons[j].IdTight;
            temp.PfIso_probe = probeMuons[j].PfIso;


            tpTree->Fill();
         }
         
        

         tagMuons.clear();
         probeMuons.clear();
        // tagElectrons.clear();
        // probeElectrons.clear();
         

   }

   //###############################################################
   //##################        END OF LOOP        ##################
   //###############################################################
   tpTree->Write();
   outFile->Close();

}
