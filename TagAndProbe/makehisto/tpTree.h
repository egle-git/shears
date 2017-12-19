//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Oct 16 14:38:32 2017 by ROOT version 6.08/06
// from TTree tpTree/tpTree
// found on file: test.root
//////////////////////////////////////////////////////////

#ifndef tpTree_h
#define tpTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class tpTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Float_t         Muons_Zmass;
   Float_t         Muons_Pt_tag;
   Float_t         Muons_Pt_probe;
   Float_t         Muons_Eta_tag;
   Float_t         Muons_Eta_probe;
   Float_t         Muons_Phi_tag;
   Float_t         Muons_Phi_probe;
   Float_t         Muons_E_tag;
   Float_t         Muons_E_probe;
   int         Muons_Id_tag;
   int         Muons_Id_probe;
   int         Muons_IdTight_tag;
   int         Muons_IdTight_probe;
   float         Muons_PfIso_probe;
   // List of branches
   TBranch        *b_Muons;   //!

   tpTree(TString);
   virtual ~tpTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef tpTree_cxx
tpTree::tpTree(TString filename) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
 TChain * chain = new TChain("tpTree","");
   chain->Add(filename);
     TTree *tree = chain;
    if (tree == 0) {
#ifdef SINGLE_TREE
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("test.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("test.root");
      }
      f->GetObject("tpTree",tree);
#else
      // of trees.
            TChain * chain = new TChain("tpTree","");
                  chain->Add("tpTree/Muons");
                        tree = chain;
#endif
   }
   Init(tree);
}

tpTree::~tpTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t tpTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t tpTree::LoadTree(Long64_t entry)
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

void tpTree::Init(TTree *tree)
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

   fChain->SetBranchAddress("Muons", &Muons_Zmass, &b_Muons);
   Notify();
}

Bool_t tpTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void tpTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t tpTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef tpTree_cxx
