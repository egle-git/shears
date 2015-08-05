// Definition of the Squeezer main class
// Author: Ph. Gras CEA/IRFU Saclay
// Jan. 3, 11. Squeezer code for AnaNaS framework
// Jul. 24, 15. Cloned Squeezer and adapted to the Shears framework

#ifndef COPYANATUPLE_H
#define COPYANATUPLE_H

#include <algorithm>
#include <iomanip>
#include <memory>

#include "TObject.h"
#include "TTree.h"

#define DECLARE_PRUNER(Class, description) \
  static Pruner::Registrator<Class> prunerRegistration ## __LINE__  (#Class, description);

class TFile;

/**
 * Utility class to copy and skim AnaNaS n-tuple files.
 * This class is used by the standalone application
 * squeezer which is based. It can also be instantiated
 * directly in custom code.
 *
 * By default the class allows to select a list of branches
 * specified in a text file for a list of events identified
 * by their run number and event number read from a second
 * text file. Advanced selection can be peformed by
 * implementing a custom class as described below.
 *
 * Implementation of a custom Pruner:
 *
 *     class MyPruner: public Pruner{
 *     protected:
 *         bool filterEvent();
 *     };
 *
 *    DECLARE_PRUNER(MyPruner, "Description of your filter")
 *
 *    bool MyPruner::filterEvent(){
 *        bool pass;
 *        ... code to perform the event selection,
 *            which will set the variable pass according
 *            if the event pass the section ...
 *        return pass && Prune::filterEvent();
 *    }
 *
 * The filterBranch can be overidden for a custom
 * branch selection. The default one can optinally
 * copy whole the branches or read the list
 * of branches to copy from a text file.
 *
 * To use the custom class with the pruner
 * application specified the class name with the option
 * --pruner.
 *
 */
class Pruner{
protected:

  struct TreeRcd: public TObject{

    TreeRcd(): nelts(1), begin(0) {}
    TreeRcd(const TreeRcd& r): TObject(r) {}
    virtual ~TreeRcd() {}

    TTree* inTree;
    TTree* outTree;
    Int_t  nelts;
    //index in the tree of the first element of current events (Pruner::ievent)
    Long64_t begin;
  };

  std::auto_ptr<TFile> fin_;
  std::auto_ptr<TFile> fout_;
  TDirectory* foutDir_;
  
  //  TObjArray trees_;
  TreeRcd eventTree_;

  Int_t maxEvents_;

  Int_t ievent;

  Int_t runNum_;

  Int_t eventNum_;

  std::vector<Long64_t> eventList_;

  std::vector<std::string> excludedBranchList_;

  static const int RUN_OFFSET = 32;

  bool allEvent_;

  //numbr of copied events:
  int nCopied_;

public:

  int verbose_;

  Pruner(): foutDir_(0), maxEvents_(-1), ievent(-1), allEvent_(true), nCopied_(0), verbose_(0){
    //    trees_.SetOwner(kTRUE);
  }

  ~Pruner(){
  }

  /** Creates a Pruner instance
   * @param className, name of the selection class. It
   * must be a class inherited from Pruner and
   * registered with the macro DECLARE_PRUNER(class, description)
   */
  static Pruner* create(const char* className){
    //TODO: all selectors are created on registration.
    //Memory footprint can be optimised by creating the
    //instance on demand.
    if(!className) return new Pruner;
    std::map<std::string, Pruner::ClassRecord>::iterator res = daughters_.find(className);
    return res != daughters_.end() ? res->second.instance : 0;
  }
  
  /** Limits the number of events to copy.
   * @param val maximum number of events
   */
  void setMaxEvents(int val) { maxEvents_ = val; }

  /** List the events in the format which can be read back
   * the specify the list of events to copy.
   * @param o output stream to write the list.
   * @param inputDataFile ROOT file to read the events from.
   */
  void listEvents(std::ostream& o, const char* inputDataFile);


  /** List the event tree branched in the format which can
   * be read back the specify the list of branch to copy.
   * @param o output stream to write the list.
   * @param inputDataFile ROOT file to read the events from.
   */
  void listBranches(std::ostream& o, const char* inputDataFile);

  void fillRunSummary();

  /** Perform the event copy.
   * @param inputDataFile input file the events must be read from.
   * @param outputDataFile ouput file the events must be written to.
   */
  void run(const char* inputDataFile, const char* outputDataFile);

  /** Read the list of events to copy from a text file.
   * @fileName path to the text file.
   */
  virtual void readEventList(const char* fileName);

  /** Read the list of branches to not copy from a text file.
   * @fileName path to the text file.
   */
  void readExcludedBranchList(const char* fileName);

  struct ClassRecord{
    ClassRecord(): instance(0){}
    std::string className;
    std::string description;
    Pruner* instance;
  };

  template<typename T>
  class Registrator{
  public:
    Registrator(const char* className, const char* description){
      ClassRecord rcd;
      rcd.className = className;
      rcd.description = description;
      rcd.instance = new T();
      daughters_[className] = rcd;
    }
  };

  static std::map<std::string, Pruner::ClassRecord> daughters_;
  
protected:
  /** Check if a branch must be copied. Return false the branch must excluded
   * from the copy,
   * Can be overwritten in a derived class to customize the selection.
   */
  virtual bool filterBranch( const char* branchName){
    return std::find(excludedBranchList_.begin(), excludedBranchList_.end(),
		     std::string(branchName))
      == excludedBranchList_.end();
  }

  /** Check if curren event must be used. Typically use eventNum_ and runNum_
   * fields which contain the event and the run numbers. Return true if event
   * must be copied
   */
  virtual bool filterEvent(){
    if(allEvent_) return true; //copy every event
    bool selected = (std::find(eventList_.begin(), eventList_.end(),
			       (((Long64_t) runNum_) <<RUN_OFFSET) + eventNum_)
		     != eventList_.end());
    return selected;
  }

  /** Called before the event loop. To be used to set the branch addresses
   * to access in the filterEvent method.
   */
  virtual void init(TTree* tree){}
  
private:
  /** initialized event summary tree. This method is already called by
   * init() method.
   */
  void setEventSummaryTree();

  /** Initialize input and output files and trees
   */
  bool init(const char* inputDataFile, const char* outputDataFile = 0);

  /** Copy an event.
   */
  void copyEvent();

  /** Pass to the next event.
   */
  bool nextEvent();

private:
  /** Process a tree
   */
  void processTree(TTree* tree);
};

#endif //COPYANATUPLE_H not defined
