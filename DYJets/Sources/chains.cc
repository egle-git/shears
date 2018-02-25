#include "chains.h"

#include <string>

#include <TChain.h>
#include <TFile.h>

#include "logging.h"

chains::chains(const std::vector<std::string> &files)
    : _events(std::make_shared<TChain>()), _bit_fields(std::make_shared<TChain>())
{
    bool first_file = true;

    for (const std::string &fullpath : files) {
        logging::debug << "Adding " << fullpath << " to the list of input files." << std::endl;
        std::string treePath = fullpath + "/tupel/EventTree";
        std::string bonzaiHeaderPath = fullpath + "/tupel/BonzaiHeader";
        std::string bonzaiBitFieldsPath = fullpath + "/tupel/BitFields";
        _events->Add(treePath.c_str());
        if (first_file) {
            // Check presence of the BonzaiHeader tree. It is checked
            // only on the first file which can be succesfully opened
            // assuming that all files of the catalog are the same.
            TFile *f = TFile::Open(fullpath.c_str());
            if (f && !f->IsZombie()) {
                if (f->GetDirectory("tupel")->FindKey("BonzaiHeader")) {
                    _bonzai_header = std::make_shared<TChain>();
                    _bonzai_header->Add(bonzaiHeaderPath.c_str());
                } else {
                    logging::warn
                        << "The tree BonzaiHeader was not found in file " << fullpath
                        << ". We will assume we run on a boabab file and not Baobab->Bonzai "
                        << "acceptance correction will be considered. This message can be ignored "
                        << "if for this sample Boabab ntuples are used as input." << std::endl;
                }
            }
            if (f) {
                delete f;
            }
            first_file = false;
        }
        _bit_fields->Add(bonzaiBitFieldsPath.c_str());
    }
}
