#include "catalog.h"

#include <regex.h>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include <TChain.h>
#include <TFile.h>

#include "functions.h"

catalog::catalog(const std::string &filename,
                 const std::string &bonzaiDir,
                 int maxFiles) :
    _event_chain(std::make_shared<TChain>()),
    _bonzai_header_chain(std::make_shared<TChain>()),
    _bit_fields_chain(std::make_shared<TChain>()),
    _lumi(0),
    _xsec(0)
{
    std::string fullpath = filename;
    if (filename[0] == '/') { // Absolute path
        fullpath = filename;
    } else {
        fullpath = bonzaiDir + "/" + filename;
    }

    if (boost::starts_with(fullpath, "/store/")) {
        fullpath.insert(0, "root://eoscms.cern.ch//eos/cms");
    }

    regex_t xsecLine;
    int rc =  regcomp(&xsecLine,"[#*][[:space:]]*sample xsec[[:space:]:=]\\+\\([[:digit:].eE+-]\\+\\)", 0);
    if(rc){
        char buffer[256];
        regerror(rc, &xsecLine, buffer, sizeof(buffer));
        buffer[sizeof(buffer)-1] = 0;
        std::cerr << "Bug found in " << __FILE__  << ":" << __LINE__ << ": " << buffer << "\n";
    }

    regex_t lumiLine;
    rc =  regcomp(&lumiLine,"[#*][[:space:]]*lumi[[:space:]:=]\\+\\([[:digit:].eE+-]\\+\\)", 0);
    if(rc){
        char buffer[256];
        regerror(rc, &lumiLine, buffer, sizeof(buffer));
        buffer[sizeof(buffer)-1] = 0;
        std::cerr << "Bug found in " << __FILE__  << ":" << __LINE__ << ": " << buffer << "\n";
    }

    if (isRootFile(fullpath.c_str())){
        TString treePath = fullpath + "/tupel/EventTree";
        TString bonzaiHeaderPath = fullpath + "/tupel/BonzaiHeader";
        TString bonzaiBitFieldsPath = fullpath + "/tupel/BitFields";
        cout << "Loading file: " << fullpath << endl;
        if(_event_chain) _event_chain->Add(treePath);
        if(_bonzai_header_chain){
            //check presence of the BonzaiHeader tree:
            TFile* f = TFile::Open(fullpath.c_str());
            if(f && !f->IsZombie()){
                if(f->GetDirectory("tupel")->FindKey("BonzaiHeader")){
                    _bonzai_header_chain->Add(bonzaiHeaderPath);
                } else{
                    std::cerr << "Warning: the tree BonzaiHeader was not found in file "
                        << fullpath
                        << ". We will assume we run on a boabab file and not Baobab->Bonzai "
                        << "acceptance correction will be considered. This message can be ignored "
                        << "if for this sample Boabab ntuples are usd as input.\n";
                }
            }
        }
        if(_bit_fields_chain) _bit_fields_chain->Add(bonzaiBitFieldsPath);
    } else {
        int (*closeFunc)(FILE*);
        FILE* f = eosOpen(fullpath.c_str(), &closeFunc);
        if(!f){
            std::cerr << "Failed to  open file " << fullpath << ".\n";
        } else{
            std::cout << "Reading input files from catalog file " << fullpath << "\n";
            string line;
            char* buffer = 0;
            size_t buffer_size = 0;
            int ifile = 0;
            //if maxFiles = 0 only catalog header is read.
            enum {False = 0, True, Unknown } isBonzai = Unknown;
            while (!feof(f)){
                ssize_t len = getline(&buffer, &buffer_size, f);
                if(len  < 0) break;
                char* line = buffer;
                size_t n = len - 1;
                //trim white spaces:
                while(line[0] == ' ' || line[0] == '\t') {++line; --n;}
                while(line[n] == ' ' || line[n] == '\t' || line[n] == '\r' || line[n] == '\n' ) {
                    line[n] = 0; --n;
                }

                regmatch_t pmatch[2];
                if (!regexec(&xsecLine, line, sizeof(pmatch)/sizeof(pmatch[0]), pmatch, 0)){
                    line[pmatch[1].rm_eo] = 0;
                    _xsec = strtod(line + pmatch[1].rm_so, 0);
                    if(_xsec == 0){
                        std::cerr << "Value of parameter 'sample xsec', " << line + pmatch[1].rm_so
                            << " found in file " << fullpath << " is not valid.\n";
                    }
                }
                else if(!regexec(&lumiLine, line, sizeof(pmatch)/sizeof(pmatch[0]), pmatch, 0)){
                    line[pmatch[1].rm_eo] = 0;
                    _lumi = strtod(line + pmatch[1].rm_so, 0);
                    if(_lumi == 0){
                        std::cerr << "Integrated luminosity parameter value, " << line + pmatch[1].rm_so
                            << " found in file " << fullpath << " is not valid.\n";
                    }
                }

                //skip empty lines,  comment lines and metadata lines:
                if (line[0] == 0 || line[0] == '#' || line[0] == '*') continue;

                if(maxFiles == 0 || (_event_chain == 0 && _event_chain ==0)) break;

                //keep content of first column only:
                char* p = line;
                while(*p != 0 && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') ++p;
                *p = 0;

                //following check is done having read the header
                //such that maxFiles = 0 can be used to read only
                //the header.
                if(maxFiles >= 0 && ifile >= maxFiles) break;

                TString filePath = TString(line);

                if(filePath[0]!='/' && !filePath.BeginsWith("root:")) {
                    filePath.Insert(0, TString(bonzaiDir) + "/");
                }
                if(filePath.BeginsWith("/store/")){
                    filePath.Insert(0, "root://eoscms.cern.ch//eos/cms");
                }

                TString treePath = filePath + "/tupel";
                TString bonzaiHeaderPath = treePath + "/BonzaiHeader";
                TString bonzaiBitFieldsPath = treePath + "/BitFields";
                treePath += "/EventTree";
                //std::cout << "Adding path " << treePath << " to the tree chain.\n";
                if(_event_chain) _event_chain->Add(treePath);

                if(_bonzai_header_chain && (isBonzai == Unknown)){
                    //check presence of the BonzaiHeader tree. It is checked
                    //only on the first file which can be succesfully opened
                    //assuming that all files of the catalog are the same.
                    TFile* f = TFile::Open(filePath);
                    if(f && !f->IsZombie()){
                        if(f->GetDirectory("tupel")->FindKey("BonzaiHeader")){
                            isBonzai = True;
                        } else{
                            isBonzai = False;
                            std::cerr << "Warning: the tree BonzaiHeader was not found in file "
                                << fullpath
                                << ". We will assume we run on a boabab file and not Baobab->Bonzai "
                                << "acceptance correction will be considered. This message can be ignored "
                                << "if for this sample Boabab ntuples are usd as input.\n";
                        }
                    }
                }
                if(isBonzai) _bonzai_header_chain->Add(bonzaiHeaderPath);
                if(_bit_fields_chain) _bit_fields_chain->Add(bonzaiBitFieldsPath);
                ++ifile;
            }//next line
            std::cout << "Closing catalog file " << fullpath << "\n";
            if(buffer) free(buffer);
            closeFunc(f);
        } //file opening succeeded
    }//is root file
    regfree(&xsecLine);
    regfree(&lumiLine);
}

catalog::~catalog()
{}
