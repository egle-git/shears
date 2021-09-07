#include "triggers.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <boost/tokenizer.hpp>

#include <TTree.h>

namespace physics
{

namespace /* anonymous */
{

static const char *const branch_names[trigger::count] = {
    "TrigHltPhot", "HLT_IsoTkMu24", "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ", "TrigHltEl", "TrigHltDiEl",
};
//Every available trigger in nano is single branch
static std::vector<std::string> available_triggers(util::chains &chains, std::size_t trig)
{
    const char *const branch_name = ""; //branch_names[trig];

    TTree &Events = *chains.events();

    // Fetch the mapping between position in bitfield and trigger name

    std::vector<std::string> *names_ptr = nullptr;

    if (Events.GetBranch(branch_name)) {
        Events.SetBranchAddress(branch_name, &names_ptr);
    } else {
        std::string msg = "Cannot set the trigger bits, because the branch ";
        msg += branch_name;
        msg += " was not found in the tree Events.";
        throw std::invalid_argument(msg);
    }

    if (Events.GetEntry(0) <= 0) {
        throw std::invalid_argument("Failed to read Events tree. Is the tree empty? "
                                    "Cannot set the trigger bits.");
    }

    std::vector<std::string> names;
    for (const std::string &name : *names_ptr) {
        if (!name.empty()) {
            names.push_back(name);
        }
    }

    // The line below is needed to prevent ROOT from holding a dangling
    // pointer (which leads to memory corruption).
    Events.SetBranchAddress(branch_name, nullptr);

    return names;
}

} // namespace anonymous

void print_available_triggers(util::chains &chains)
{
    for (unsigned trig = 0; trig < trigger::count; ++trig) {
        std::cout << "Available triggers for " << branch_names[trig] << ":";
        for (const auto &name : available_triggers(chains, trig)) {
            std::cout << " " << name;
        }
        std::cout << std::endl;
    }
}

/******************************************************************************/

trigger::~trigger() {}

trigger::trigger(const std::vector<std::string> &names) : _mask(0LL), _veto(0LL), _names(names) {}

bool trigger::accept(const std::string &name)
{
//    for (std::size_t i = 0; i < _names.size(); ++i) {
//        if (_names[i] == name) {
//            _mask |= (1LL << i);
//            return true;
//        }
//    }
//    return false;
      return true;
}

bool trigger::veto(const std::string &name)
{
//    for (std::size_t i = 0; i < _names.size(); ++i) {
//        if (_names[i] == name) {
//            _veto |= (1LL << i);
//            return true;
//        }
//    }
//    return false;
      return true;
}

/******************************************************************************/

trigger_values::trigger_values(util::job::info &info) //:
//    _values(
//        TTreeReaderValue<unsigned long long>(info.reader, branch_names[0]),
//        TTreeReaderValue<unsigned long long>(info.reader, branch_names[1]),
//        TTreeReaderValue<unsigned long long>(info.reader, branch_names[2]),
//        TTreeReaderValue<unsigned long long>(info.reader, branch_names[3]),
//        TTreeReaderValue<unsigned long long>(info.reader, branch_names[4])
//    )
{
    // In case this assert fails: add a new reader above and increase the value.
   // static_assert(trigger::count == 5, "Yon need to add a new trigger reader");
}

/******************************************************************************/

trigger_mask::trigger_mask(util::job::info &info) : _accepts_any_trigger(false)
{
//    for (unsigned trig = 0; trig < trigger::count; ++trig) {
//        auto trigger_names = available_triggers(info.chains, trig);
//        switch (trig) {
//        default:
//            _triggers[trig] = std::make_shared<trigger>(trigger_names);
//        }
//    }
}

trigger_mask::trigger_mask(util::job::info &info, const std::string &definition, bool verbose)
    : trigger_mask(info)
{
    using boost::escaped_list_separator;
    using boost::tokenizer;

    int accepted_count = 0;

    // Retrieve tokens
    escaped_list_separator<char> sep("\\", "\t ,\n\r", "\"\'");
    tokenizer<escaped_list_separator<char>> tok(definition, sep);
    for (auto it = tok.begin(); it != tok.end(); ++it) {
        const std::string &token = *it;

        if (token.empty()) {
            continue;
        }

        if (token[0] == '^') { // Trigger is a veto
            if (!veto(token.substr(1))) {
                std::string msg = "Could not find trigger ";
                msg += token.substr(1);
                throw std::invalid_argument(msg);
            }
            if (verbose) std::cout << "\tVETO\t" << token.substr(1) << std::endl;
        } else { // Regular trigger
            accepted_count++;
            if (!accept(info.reader, token)) {
                throw std::invalid_argument("Could not find trigger " + token);
            }
            if (verbose) std::cout << "\tACCEPT\t" << token << std::endl;
        }
    }
    if (accepted_count == 0) {
        set_accepts_any_trigger(true);
        if (verbose) std::cout << "\tACCEPT ANY TRIGGER" << std::endl;
    }
}

bool trigger_mask::accept(TTreeReader &tr, const std::string &name)
{
    _accepted_triggers.push_back (TTreeReaderValue<bool> (tr, name.data()));
    return true;
}

bool trigger_mask::accept(TTreeReader &tr, const std::vector<std::string> &names)
{
    bool ok = true;
    for (auto &name : names) {
        ok &= accept(tr, name);
    }
    return ok;
}

bool trigger_mask::veto(const std::string &name)
{
    for (auto &trig : _triggers) {
        if (trig->veto(name)) {
            return true;
        }
    }
    return false;
}

bool trigger_mask::veto(const std::vector<std::string> &names)
{
    bool ok = true;
    for (auto &name : names) {
        ok &= veto(name);
    }
    return ok;
}

bool trigger_mask::passes(const trigger_values &values)
{
    if (std::any_of(_vetoed_triggers.begin(),
                    _vetoed_triggers.end(),
                    [] (auto &reader) { return *reader; })) {
        return false;
    }
    if (std::any_of(_accepted_triggers.begin(),
                    _accepted_triggers.end(),
                    [] (auto &reader) { return *reader; })) {
        return true;
    }
    bool pass = _accepts_any_trigger;
    return pass;

}
} // namespace physics
