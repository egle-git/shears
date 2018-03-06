#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <memory>
#include <string>
#include <vector>

#include <TTreeReaderValue.h>

#include "options.h"
#include "job.h"

class TTree;

namespace physics
{

/**
 * \brief Prints a list of all available triggers.
 */
void print_available_triggers(util::chains &chains);

/**
 * \brief Represents a category of related triggers.
 *
 * This class is used internally by \ref trigger_mask.
 */
class trigger
{
  public:
    /// \brief The number of trigger categories supported by the implemenation.
    static const unsigned count = 5;

  private:
    unsigned long long _mask, _veto;
    std::vector<std::string> _names;

  public:
    /// \brief Constructor.
    explicit trigger(const std::vector<std::string> &names);

    /// \brief Destructor.
    virtual ~trigger();

    /// \brief Sets the given trigger path to be accepted.
    bool accept(const std::string &name);

    /// \brief Sets the given trigger path to be used as a veto.
    bool veto(const std::string &name);

    /// \brief Retrieves whether the trigger is used.
    bool used() const { return _mask != 0 || _veto != 0; }

    /// \brief Retrieves the mask of accepted triggers paths.
    unsigned long long mask() const { return _mask; }

    /// \brief Retrieves the mask of vetoed triggers paths.
    unsigned long long veto_mask() const { return _veto; }

    /// \brief Returns \c true if the given \c trigger is accepted.
    bool accepted(unsigned long long trigger) const { return (_mask & trigger) != 0; }

    /// \brief Returns \c true if the given \c trigger triggers a veto.
    bool is_veto(unsigned long long trigger) const { return (_veto & trigger) != 0; }
};

/**
 * \brief Holds values of the triggers for an event.
 *
 * This class does all the dirty job of setting up branches.
 */
class trigger_values
{
  private:
    mutable std::array<TTreeReaderValue<unsigned long long>, trigger::count> _values;

  public:
    /**
     * \brief Constructor.
     * \note  Don't try to read trigger branches by yourself or you'll break
     *        this class.
     */
    explicit trigger_values(util::job::info &info);

    /**
     * \brief Retrieves the value of the `i`th trigger branch without bound
     *        checking.
     */
    unsigned long long operator[](std::size_t i) const { return *(_values[i]); }

    /**
     * \brief Retrieves the value of the `i`th trigger branch with bound
     *        checking.
     * \throws std::out_of_range if \c i is out of bounds.
     */
    unsigned long long at(std::size_t i) const { return *(_values.at(i)); }
};

/**
 * \brief Holds the list of triggers used in an analysis.
 *
 * This class can be used when one needs to accept events that pass any of a
 * given set of triggers, and reject others. To do this, one first passes all
 * accepted triggers to \ref accept, and then calls \ref passes for every event.
 */
class trigger_mask
{
    bool _accepts_any_trigger;
    std::array<std::shared_ptr<trigger>, trigger::count> _triggers;

  public:
    /**
     * \brief Constructor.
     * \throws std::invalid_argument if \c bitFieldsChain is empty.
     */
    explicit trigger_mask(util::job::info &info);

    /**
     * \brief Constructs a trigger mask from user input.
     *
     * The definition provided must be a comma- or space-separated list of (possibly quoted)
     * trigger names. A trigger can be specified as a veto by putting a \c ^ before its name.
     *
     * For sample, the definition `HLT_IsoMu24, HLT_IsoTkMu24, ^HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL`
     * will produce a mask accepting events passing either of the `HLT_IsoMu24` and `HLT_IsoTkMu24`
     * triggers, and rejecting events passing the `HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL`.
     *
     * If the definition doesn't have any accepted trigger, anything will be accepted (as per
     * \ref set_accepts_any_trigger).
     *
     * If \c verbose is enabled, the constructor will log some information to \c cout.
     *
     * \throws std::invalid_argument if \c bitFieldsChain is empty, or a trigger is not found.
     */
    explicit trigger_mask(util::job::info &info,
                          const std::string &definition,
                          bool verbose = false);

    /**
     * \brief Sets the given trigger path to be accepted by \ref passes.
     * \returns \c true if the path was found.
     */
    bool accept(const std::string &name);

    /**
     * \brief Sets all of the given trigger paths to be accepted by \ref passes.
     *
     * If some paths aren't found, this function will set all others to be
     * accepted and return \c false.
     *
     * \returns \c true if all paths were found.
     */
    bool accept(const std::vector<std::string> &name);

    /// \brief Sets whether all triggers should be accepted.
    void set_accepts_any_trigger(bool enable) { _accepts_any_trigger = enable; }

    /// \brief Returns whether all triggers are accepted.
    bool accepts_any_trigger() { return _accepts_any_trigger; }

    /**
     * \brief Sets the given trigger path to cause \ref passes to return \c false.
     * \returns \c true if the path was found.
     */
    bool veto(const std::string &name);

    /**
     * \brief Sets all of the given trigger paths cause \ref passes to return \c false.
     *
     * If some paths aren't found, this function will set all others to be
     * vetoed and return \c false.
     *
     * \returns \c true if all paths were found.
     */
    bool veto(const std::vector<std::string> &name);

    /**
     * \brief Checks whether the event coorsponding to the given \c values passes the trigger
     *        requirements.
     *
     * An event passes the trigger requirement if no veto'ed trigger fired, and at least one
     * accepted trigger fired (or \c accepts_any_trigger was set).
     */
    bool passes(const trigger_values &values) const;
};
} // namespace physics

#endif // TRIGGERS_H
