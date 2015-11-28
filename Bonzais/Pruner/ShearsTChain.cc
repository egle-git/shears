#include "ShearsTChain.h"

//ClassImp(ShearsTChain);

bool ShearsTChain::setCatalog(const char* catalog, int maxFiles, int skipFiles){
  std::ifstream f(catalog);
  if(!f.good()){
    std::cerr << "Failed to open file "<< catalog << "!\n";
    return false;
  }
    
  //Remove previously added files:
  Reset();

  int iline = 0;
  int nfiles = 0;
  while(f.good()){
    ++iline;
    std::string l;
    std::string::size_type p;
      
    std::getline(f, l);

    //trim white spaces:
    p = l.find_first_not_of(" \t");
    if(p!=std::string::npos) l.erase(0, p);
    p = l.find_last_not_of(" \t\n\r");
    if(p!=std::string::npos) l.erase(p + 1);
    else l.clear();
      
    //skip empty lines and comment lines:
    if (!l.size() || l[0] == '#') continue;
      
    if (!l.size() || l[0] == '*') continue;
      
    //extract first column (file name):
    p = l.find_first_of(" \t");
    if(p!=std::string::npos) l.erase(p);
      
    //sanity check:
    const char ext[] = ".root";
      
    if(l.size() < sizeof(ext) || l.substr(l.size() - sizeof(ext) + 1) != ext){
      std::cerr << "Line " << iline << " of catalog file " << catalog << " was skipped.\n";
      continue;
    }
      
    //Solves EOS paths:
    std::string store("/store/");
    if(l.substr(0,7) == store){
      //A CMS EOS path
      l.insert(0, "root://eoscms.cern.ch//eos/cms");
    }
      
    if(skipFiles <= 0){
      ++nfiles;
      if((maxFiles > 0) &&  (nfiles > maxFiles)) break;
      if(verbosity_>0){
	std::cout << "Add file " << l.c_str() << " to the list of input files.\n";
      }
      Add(l.c_str());
    } else{
      --skipFiles;
    }      
  }
  
  return true;
}

#if defined(__ROOTCLING__)
#pragma link C++ class ShearsTChain;
#endif

