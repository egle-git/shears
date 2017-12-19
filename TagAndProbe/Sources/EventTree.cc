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
#include <TString.h>

void EventTree::Loop()
{
   if (fChain == 0) return;

   //###############################################################
   //################## DECLARATION OF HISTOGRAMS ##################
   //###############################################################
   TFile *outFile = new TFile(outputFile_,"RECREATE");
   TDirectoryFile *dir = new TDirectoryFile("tpTree","tpTree","",outFile);
   gDirectory->cd("tpTree");
   TTree *tpTree = new TTree("fitter_tree","fitter_tree");
   std::vector<Electron>  tagElectrons ;
   std::vector<Electron>  probeElectrons;
   std::vector<Muon>  tagMuons ;
   std::vector<Muon>  probeMuons ;

   
   
   TString variables[]={"mass","tag_pt","pt","tag_eta","eta","tag_phi","phi","tag_abseta","abseta","tag_charge","charge","tkIso","Tight2012","tag_HltMatch","HltMatch","IsoMu22","IsoTkMu22","IsoMu24","IsoTkMu24","HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_leg17","HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_leg8","HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_filter","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_leg17","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_leg8","HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_filter"};
  
   Int_t varnum = sizeof(variables) / sizeof(variables[0]);
   enum varnames
   {mass,tag_pt,pt,tag_eta,eta,tag_phi,phi,tag_abseta,abseta,tag_charge,charge,tkIso,Tight2012,tag_HltMatch,HltMatch,IsoMu22,IsoTkMu22,IsoMu24,IsoTkMu24,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_leg17,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_leg8,HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_filter,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_leg17,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_leg8,HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_filter
      
   };
   
   
   int name = Tight2012;
   Float_t fill_float[Tight2012];
   
   Int_t fill_int[13];
   
   for(int i=0;i<varnum;i++){
      
     if (i<name) tpTree->Branch(variables[i],&fill_float[i],variables[i]+"/F");
   
     else tpTree->Branch(variables[i],&fill_int[i-name],variables[i]+"/I");
  
   }
   
   Long64_t nentries = fChain->GetEntries();

   Long64_t nbytes = 0, nb = 0;
   cout << "nb of entries in the input file =" << nentries << endl;

   //###############################################################
   //##################     EVENT LOOP STARTS     ##################
   //###############################################################

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(jentry % 10000 ==0) cout << jentry << " of " << nentries << endl;
      if (leptongeneration_ =="electron"){
         objectSelection::selectElectrons(tagElectrons, probeElectrons, ElCh,ElPt, ElEta, ElPhi, ElE, ElId, ElEtaSc,ElPfIsoRho);
         for(int i=0;i<tagElectrons.size();i++)
            for(int j=0;j<probeElectrons.size();j++)
            {
               if(tagElectrons[i].Ch== probeElectrons[j].Ch) continue;  
               if(tagElectrons[i].Seq == probeElectrons[j].Seq) continue;// avoiding a pair containing 2 same lepton
               float mll;
               mll= (tagElectrons[i].lvector+probeElectrons[j].lvector).M();
               if(mll<60||mll>120) continue;
               fill_float[mass        ] = mll;
               fill_float[tag_pt      ] = tagElectrons  [i].Pt   ;
               fill_float[pt          ] = probeElectrons[j].Pt   ;
               fill_float[tag_eta     ] = tagElectrons  [i].Eta  ;
               fill_float[eta         ] = probeElectrons[j].Eta  ;
               fill_float[tag_phi     ] = tagElectrons  [i].Phi  ;
               fill_float[phi         ] = probeElectrons[j].Phi  ;
               fill_float[tag_abseta  ] = tagElectrons  [i].E    ;
               fill_float[abseta      ] = probeElectrons[j].E    ;
               fill_float[tag_charge  ] = tagElectrons  [i].Ch   ;
               fill_float[charge      ] = probeElectrons[j].Ch   ;
               fill_float[tkIso       ] = probeElectrons[j].PfIsoRho;
               fill_int  [0] = probeElectrons[j].Id   ;
               tpTree->Fill();
            }
         }
      if(leptongeneration_ == "muon"){
        objectSelection::selectMuons(tagMuons, probeMuons, MuCh,MuPt, MuEta, MuPhi, MuE, MuId, MuIdTight, MuPfIso,MuHltMatch);
        for(int i=0;i<tagMuons.size();i++)
           for(int j=0;j<probeMuons.size();j++)
           {
              if(tagMuons[i].Seq == probeMuons[j].Seq) continue;// avoiding a pair containing 2 same lep
              if(tagMuons[i].Ch == probeMuons[j].Ch) continue;
              float mll;
              mll= (tagMuons[i].lvector+probeMuons[j].lvector).M();
              if(mll<60||mll>120) continue;
       
               fill_float[mass        ] = mll;
               fill_float[tag_pt      ] = tagMuons  [i].Pt   ;
               fill_float[pt          ] = probeMuons[j].Pt   ;
               fill_float[tag_eta     ] = tagMuons  [i].Eta  ;
               fill_float[eta         ] = probeMuons[j].Eta  ;
               fill_float[tag_phi     ] = tagMuons  [i].Phi  ;
               fill_float[phi         ] = probeMuons[j].Phi  ;
               fill_float[tag_abseta  ] = tagMuons  [i].E    ;
               fill_float[abseta      ] = probeMuons[j].E    ;
               fill_float[tag_charge  ] = tagMuons  [i].Ch   ;
               fill_float[charge      ] = probeMuons[j].Ch   ;
               fill_float[tkIso       ] = probeMuons[j].PfIso;
               fill_int  [0] = probeMuons[j].IdTight           ;
               fill_int  [1] = tagMuons[i].HltMatch           ;
               fill_int  [2] = probeMuons[j].HltMatch         ;
               for(int k=0;k<sizeof(probeMuons[j].TriggerInf)/sizeof(probeMuons[j].TriggerInf[0]);k++){fill_int[k+3]=probeMuons[j].TriggerInf[k];}

   
   
              tpTree->Fill();
           }
         }
        

         tagMuons.clear();
         probeMuons.clear();
         tagElectrons.clear();
         probeElectrons.clear();
         

   }

   //###############################################################
   //##################        END OF LOOP        ##################
   //###############################################################
   
   tpTree->Write();
   outFile->Close();

}
