#include "chains.h"

#include <stdexcept>
#include <string>

#include <TChain.h>
#include <TFile.h>

#include "logging.h"

namespace util
{

chains::chains(const std::vector<std::string> &files)
    : _events(std::make_shared<TChain>()), _bit_fields(std::make_shared<TChain>())
{
    bool first_file = true;
    bool is_bonzai = false;

    for (const std::string &fullpath : files) {
        logging::debug << "Adding " << fullpath << " to the list of input files." << std::endl;
        std::string treePath = fullpath + "/tupel/EventTree";
        std::string bonzaiHeaderPath = fullpath + "/tupel/BonzaiHeader";
        std::string bonzaiBitFieldsPath = fullpath + "/tupel/BitFields";
        _events->Add(treePath.c_str());

        TFile *f = TFile::Open(fullpath.c_str());
        if (f && !f->IsZombie()) {
            // Check presence of the BonzaiHeader tree.
            if (f->GetDirectory("tupel")->FindKey("BonzaiHeader")) {
                // This is a bonzai
                if (first_file) {
                    _bonzai_header = std::make_shared<TChain>();
                } else if (!is_bonzai) {
                    // Not the first file, and previous files were baobabs
                    throw std::invalid_argument(
                        "The catalog contains a mixture of baobabs and bonzais.");
                }

                is_bonzai = true;
                _bonzai_header->Add(bonzaiHeaderPath.c_str());
            } else {
                // This is a baobab
                if (is_bonzai) {
                    throw std::invalid_argument(
                        "The catalog contains a mixture of baobabs and bonzais.");
                }
                if (first_file) {
                    logging::warn
                        << "The tree BonzaiHeader was not found in file " << fullpath
                        << ". We will assume we run on a boabab file and not Baobab->Bonzai "
                        << "acceptance correction will be considered. This message can be ignored "
                        << "if for this sample Boabab ntuples are used as input." << std::endl;
                }
            }
        }
        if (f) {
            delete f;
        }
        first_file = false;

        _bit_fields->Add(bonzaiBitFieldsPath.c_str());
    }
}
} // namespace util
