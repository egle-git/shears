// Implemantation of the Pruner main class
// Author: Ph. Gras CEA/IRFU Saclay
// Jan.  3, 11. Squeezer code for AnaNaS framework
// Jul. 24, 15. Cloned Squeezer and adapted to the Shears framework

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sys/time.h>

#include "TObjArray.h"
#include "TBranch.h"
#include "TFile.h"
#include "TKey.h"

#include "Pruner.h"

using std::cout;
using std::cerr;
using std::endl;

std::map<std::string, Pruner::ClassRecord> Pruner::daughters_;

void Pruner::listEvents(std::ostream& o, const char* inputDataFile){

  fin_ = std::auto_ptr<TFile>(new TFile(inputDataFile));

  if(fin_->IsZombie()){ cout << "Failed to open input ntuple file "
			     << inputDataFile << endl;
    return;
  }
  //  for(Long64_t i = 0; i < eventSummary_->GetEntries(); ++i){
  //    eventSummary_->GetEntry(i);
  //  o << std::setw(8) << runNum_ << std::setw(8) << " " << eventNum_ << endl;
  //}
  o << "This functionnality is not implemented yet" << endl;
}

void Pruner::listBranches(std::ostream& o, const char* inputDataFile){

  init(inputDataFile);

  if(eventTree_.inTree){
    //TIter itTree(&trees_);
    //while(itTree.Next()){
    //TTree* tree = ((TreeRcd*) (*itTree))->tree;
    //TO CHECK: is list of leaves fine or should we use GetListOfBranches
    //recursively?
    //TObjArray* ls = tree->GetListOfLeaves();
    TObjArray* ls = eventTree_.inTree->GetListOfLeaves();
    TIter it(ls);
    while(it.Next()) o << (*it)->GetName() << endl;
  }
}

void Pruner::fillRunSummary(){

  const char* treesToCopy[] = {"Header", "Description", "BitFields"};

  fin_->cd();
  fin_->cd("tupel");
  TList* fcontent = gDirectory->GetListOfKeys();
  TIter it(fcontent);
  TKey* key = 0;
  while(it.Next()){
    key = (TKey*) (*it);

    if(strcmp(key->GetClassName(), "TTree") != 0) continue;

    bool toCopy = false;
    for(unsigned i = 0; i < sizeof(treesToCopy)/sizeof(treesToCopy[0]); ++i){
      if(strcmp(key->GetName(), treesToCopy[i]) == 0) toCopy = true;
    }
    if(!toCopy) continue;

    TTree* tree =  (TTree*) key->ReadObj();
    if(tree==0){
      std::cerr << "Warning: Failed to read tree " << key->GetName() << ".\n";
      continue;
    }


    if(verbose_ > 0){
      std::cout << "Copying  ROOT tree " << tree->GetName() << "\n";
    }

    fout_->cd();
    TDirectory::CurrentDirectory()->cd("tupel");

    TTree* treeOut = tree->CloneTree(0);
    tree->GetEntry(0);
    treeOut->Fill();
    treeOut->AutoSave();
    fin_->cd();
  }
}

void Pruner::run(const char* inputDataFile, const char* outputDataFile){
  timeval start;

  gettimeofday(&start, 0);

  init(inputDataFile, outputDataFile);

  fin_->cd();

  ////----
  Long64_t nevts;
//  TIter itT(&trees_);
//  while(itT.Next()){
//    nevts = ((TreeRcd*)(*itT))->tree->GetEntries();
//  }

  if(eventTree_.inTree == 0) return;

  init(eventTree_.inTree);



  nevts = eventTree_.inTree->GetEntries();


  if(maxEvents_ >=0 && nevts > maxEvents_) nevts = maxEvents_;

  if(verbose_ > 0){
    std::cout << "Number of events to read: " << nevts << "\n";
  }

  timeval t;
  for(Long64_t i = 1; i <= nevts; ++i){
    nextEvent();
    if(filterEvent()){
      copyEvent();
      ++nCopied_;
    }
    const static int step = 100;
    if(i%step==0 || i == nevts){
      cout << "\rNumber of events. Read: " << std::setw(8) << i << " Copied "
	   << std::setw(8) << nCopied_
	   << " Remaining: " << std::setw(8) << (nevts-i)
	   << " Total: " << std::setw(8) << nevts; // << std::flush;
      timeval t0;
      if(i==step) gettimeofday(&t0, 0);
      else if(i >= 2 * step) {
	gettimeofday(&t, 0);
	double remaining = double(nevts - step) / (i - step)
	  * ((t.tv_sec - t0.tv_sec) + 1.e-6 * (t.tv_usec - t0.tv_usec));
	time_t eat = int(t0.tv_sec +  1.e-6 * t0.tv_usec + remaining + 0.5);
	char buf[256];
	strftime(buf, sizeof(buf),  "%a, %d %b %Y %T", localtime(&eat));
	cout << " ETA: " << std::setw(16) << buf;
      }
      cout << std::flush;
    }
  } //next event
  cout << "\n";

  fillRunSummary();

  //  TIter itTree(&trees_);
  //while(itTree.Next()){ ((TreeRcd*) (*itTree))->outTree->AutoSave(); }
  if(eventTree_.outTree) eventTree_.outTree->AutoSave();
  fout_->Close();

  gettimeofday(&t, 0);

  double dt = (t.tv_sec - start.tv_sec) + 1.e-6 * (t.tv_usec - start.tv_usec);

  printf("Elapsed time: %.2g sec. %.1f event/sec.\n",  dt, nevts/dt);
  fflush(stdout);
}

void Pruner::readEventList(const char* fileName){
  allEvent_ = false;
  FILE* f = fopen(fileName, "r");
  if(f==0) { cout << "Failed to open event list file " << fileName << endl; abort(); }
  eventList_.clear();
  int nerr = 0;
  while(!feof(f)){
    //      while(0==fscanf(f, " # ")) {/*NOP*/}
    int run;
    int event;
    int n = fscanf(f, " %d %d ", &run, &event);
    if(n!=2){
      ++nerr;
    } else{
      eventList_.push_back((((Long64_t)run) <<RUN_OFFSET) + event);
    }
  }
  sort(eventList_.begin(), eventList_.end());
  if(nerr!=0) cout << nerr << " lines of " << fileName << " were skipped\n";
  if(verbose_>1){
    cout << "Event to select: \n";
    for(size_t i = 0; i < eventList_.size(); ++i){
      cout << "Run " << (eventList_[i] >>32) << " event "
	   << (eventList_[i] & 0xFFFFFFFF) << "\n";
    }
  }
}

void Pruner::readExcludedBranchList(const char* fileName){
  FILE* f = fopen(fileName, "r");
  if(f==0) {
    cout << "Failed to open exluded tree list file "
	 << fileName << endl; abort();
  }
  excludedBranchList_.clear();
  int nerr = 0;
  while(!feof(f)){
    //      while(0==fscanf(f, " # ")) {/*NOP*/}
    char branchName[256];
    //TODO use +/- sign to include/exclude a branch
    int n = fscanf(f, "%255s ", branchName);
    if(n!=1){
      ++nerr;
    } else{
      excludedBranchList_.push_back(std::string(branchName));
    }
  }

  sort(excludedBranchList_.begin(), excludedBranchList_.end());
  if(nerr!=0) cout << nerr << " lines of " << fileName << " were skipped\n";
  if(verbose_>1){
    cout << "Branches to exclude from copy: \n";
    for(size_t i = 0; i < excludedBranchList_.size(); ++i){
      cout << excludedBranchList_[i] << "\n";
    }
  }
}


/** Initialize input and output files and trees
 */
bool Pruner::init(const char* inputDataFile, const char* outputDataFile){

  fin_ = std::auto_ptr<TFile>(new TFile(inputDataFile));

  if(fin_->IsZombie()){
    cout << "Failed to open input ntuple file "
	 << inputDataFile << endl;
    abort();
  }

  fin_->ls();
  fin_->cd("tupel");
  TList* fcontent = gDirectory->GetListOfKeys();
  if(fcontent==0) {
    cout << "File " << inputDataFile << " is empty! " << endl;
    abort();
  }

  if(outputDataFile){
    fout_ = std::auto_ptr<TFile>(new TFile(outputDataFile, "RECREATE"));

    if(fout_->IsZombie()){
      cout << "Failed to open output ntuple file " << outputDataFile << endl;
      abort();
    }

    foutDir_ =  fout_->mkdir("tupel");
    if(foutDir_ == 0){
      cerr << "Failed to create directory tupel in output ROOT file " << outputDataFile << endl;
    } else{
      foutDir_ = fout_.get();
    }
  }

  TIter it(fcontent);

  //looks for the EventTree ROOT tree:
  bool found = false;
  TKey* key = 0;
  while(it.Next() && !found){
    key = (TKey*) (*it);
    found = (strcmp(key->GetClassName(), "TTree") == 0)
      && (strcmp(key->GetName(), "EventTree") == 0);
  }

  if(!found){
    std::cerr << "No tree with name EventTree was found in tupel directory of "
	      << inputDataFile << " ROOT file!\n";
    return false;
  }

  //  TreeRcd* treeRcd = new TreeRcd;
  //treeRcd->tree = (TTree*)  key->ReadObj();
  eventTree_.inTree = (TTree*)  key->ReadObj();

  //  if(treeRcd->tree==0) {
  if(eventTree_.inTree==0) {
    std::cerr << "Failed to read object " << key->GetName() << endl;
    return false;
  }

  //check branch to be copied
  //  TObjArray* bs = treeRcd->tree->GetListOfBranches();
  TObjArray* bs = eventTree_.inTree->GetListOfBranches();
  TIter itBranches(bs);
  while(itBranches.Next()){
    TBranch* b = (TBranch*)(*itBranches);
    //disable copy of filered-out branches:
    if(!filterBranch(b->GetName())){
      if(verbose_) cout << "Disable branch " << b->GetName() << "\n";
      //      treeRcd->tree->SetBranchStatus(b->GetName(), 0);
      eventTree_.inTree->SetBranchStatus(b->GetName(), 0);
    }
  }

  //  trees_.Add(treeRcd);
  if(fout_.get()){
    fout_->cd();
    TDirectory::CurrentDirectory()->cd("tupel");
    eventTree_.outTree = eventTree_.inTree->CloneTree(0);
    fin_->cd();
  }

  return true;
}

void Pruner::copyEvent(){
  if(verbose_ > 1) cout << "Copying event " << eventNum_ << " of run " << runNum_ << endl;
  //  TIter itTree(&trees_);
  //while(itTree.Next()){
  //TreeRcd* tree = (TreeRcd*) *itTree;
  // if(verbose_ > 1) cout << "\ttree " << tree->tree->GetName() << endl;
    for(Int_t ielt = 0; ielt < eventTree_.nelts; ++ielt){
      if(verbose_ > 1) cout << "\t\tcopy object " << (ielt+1) << endl;
      fin_->cd();
      eventTree_.inTree->GetEntry(eventTree_.begin + ielt);
      fout_->cd();
      eventTree_.outTree->Fill();
    }
    //}
}

bool Pruner::nextEvent(){
  ++ievent;
  //update tree event startpointers:
  //  bool rc = false;
  //  if(ievent>0){
  //    TIter it(&trees_);
  //    while(it.Next()) rc |= ((TreeRcd*)(*it))->tree->GetEntry(ievent);
  //  }
  //  return rc;
  return eventTree_.inTree != 0 && eventTree_.inTree->GetEntry(ievent);
}

void Pruner::processTree(TTree* tree){

  foutDir_->cd();
  TTree* outTree = tree->CloneTree(0);
  fin_->cd();
  const Long64_t nentries = tree->GetEntriesFast();
  for(Long64_t i = 0; i < nentries && i < 2; ++i){
    tree->GetEntry(i);
    outTree->Fill();
  }
  outTree->AutoSave();
}
