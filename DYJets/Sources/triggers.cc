#include "triggers.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <boost/tokenizer.hpp>

#include <TTree.h>

static const char *const branch_names[trigger::count] = {
    "TrigHltPhot", "TrigHltMu", "TrigHltDiMu", "TrigHltEl", "TrigHltDiEl",
};

static std::vector<std::string> available_triggers(TTree &bitFieldsChain, std::size_t trig)
{
    const char *const branch_name = branch_names[trig];

    // Fetch the mapping between position in bitfield and trigger name

    std::vector<std::string> *names_ptr = nullptr;

    if (bitFieldsChain.GetBranch(branch_name)) {
        bitFieldsChain.SetBranchAddress(branch_name, &names_ptr);
    } else {
        std::string msg = "Cannot set the trigger bits, because the branch ";
        msg += branch_name;
        msg += " was not found in the tree BitFields.";
        throw std::invalid_argument(msg);
    }

    if (bitFieldsChain.GetEntry(0) <= 0) {
        throw std::invalid_argument("Failed to read BitFields tree. Is the tree empty? "
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
    bitFieldsChain.SetBranchAddress(branch_name, nullptr);

    return names;
}

void print_available_triggers(TTree &bitFieldsChain)
{
    for (unsigned trig = 0; trig < trigger::count; ++trig) {
        std::cout << "Available triggers for " << branch_names[trig] << ":";
        for (const auto &name : available_triggers(bitFieldsChain, trig)) {
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
    for (std::size_t i = 0; i < _names.size(); ++i) {
        if (_names[i] == name) {
            _mask |= (1LL << i);
            return true;
        }
    }
    return false;
}

bool trigger::veto(const std::string &name)
{
    for (std::size_t i = 0; i < _names.size(); ++i) {
        if (_names[i] == name) {
            _veto |= (1LL << i);
            return true;
        }
    }
    return false;
}

/******************************************************************************/

trigger_values::trigger_values(TTree &event_tree)
{
    for (unsigned i = 0; i < trigger::count; ++i) {
        if (event_tree.SetBranchAddress(branch_names[i], &_data[i]) < 0) {
            std::string msg = "The event tree doesn't contain branch ";
            msg += branch_names[i];
            msg += ", needed for triggers";
            throw std::invalid_argument(msg);
        }
    }
}

/******************************************************************************/

trigger_mask::trigger_mask(TTree &bitFieldsChain) : _accepts_any_trigger(false)
{
    for (unsigned trig = 0; trig < trigger::count; ++trig) {
        auto trigger_names = available_triggers(bitFieldsChain, trig);
        switch (trig) {
        default:
            _triggers[trig] = std::make_shared<trigger>(trigger_names);
        }
    }
}

trigger_mask::trigger_mask(TTree &bitFieldsChain, const std::string &definition, bool verbose)
    : trigger_mask(bitFieldsChain)
{
    using boost::escaped_list_separator;
    using boost::tokenizer;

    int accepted_count = 0;

    // Retrieve tokens
    escaped_list_separator<char> sep("\\", "\t ,", "\"\'");
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
            if (!accept(token)) {
                std::string msg = "Could not find trigger ";
                msg += token.substr(1);
                throw std::invalid_argument(msg);
            }
            if (verbose) std::cout << "\tACCEPT\t" << token << std::endl;
        }
    }
    if (accepted_count == 0) {
        set_accepts_any_trigger(true);
        if (verbose) std::cout << "\tACCEPT ANY TRIGGER" << std::endl;
    }
}

bool trigger_mask::accept(const std::string &name)
{
    for (auto &trig : _triggers) {
        if (trig->accept(name)) {
            return true;
        }
    }
    return false;
}

bool trigger_mask::accept(const std::vector<std::string> &names)
{
    bool ok = true;
    for (auto &name : names) {
        ok &= accept(name);
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

bool trigger_mask::passes(const trigger_values &values) const
{
    bool pass = _accepts_any_trigger;
    for (unsigned i = 0; i < trigger::count; ++i) {
        if (_triggers[i]->is_veto(values[i])) {
            return false;
        }
        pass |= _triggers[i]->accepted(values[i]);
    }
    return pass;
}
