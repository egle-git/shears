#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <memory>
#include <string>
#include <vector>

class TTree;

/**
 * \brief Prints a list of all available triggers.
 */
void print_available_triggers(TTree &bitFieldsChain);

/**
 * \brief Represents a category of related triggers.
 *
 * This class is used internally by \ref trigger_mask. It can be subclassed to
 * provide scale factors.
 */
class trigger
{
public:
    /// \brief The number of trigger categories supported by the implemenation.
    static const unsigned count = 5;

private:
    unsigned long long _mask;
    std::vector<std::string> _names;

public:
    /// \brief Constructor.
    explicit trigger(std::vector<std::string> names);

    /// \brief Destructor.
    virtual ~trigger();

    /// \brief Sets the given trigger path to be passes by the system.
    bool accept(const std::string &name);

    /// \brief Retrieves the mask of passes triggers paths.
    unsigned long long mask() const { return _mask; }

    /// \brief Returns \c true if the given \c trigger is accepted.
    bool passes(unsigned long long trigger) const
    { return (_mask & trigger) != 0; }

    /**
     * \brief Computes the scale factor for the given trigger.
     * \param trigger The set of triggers that the event passed.
     * \note  The result of this function is undefined when `passes(trigger)` is
     *        \c false.
     */
    virtual double scale_factor(unsigned long long trigger) const;
};

/**
 * \brief Holds values of the triggers for an event.
 *
 * This class does all the dirty job of setting up branches.
 */
class trigger_values
{
private:
    std::array<unsigned long long, trigger::count> _data;

public:
    /**
     * \brief Constructor.
     * \param event_tree The event tree from a bonzai or baobab.
     * \note  Don't try to read trigger branches by yourself or you'll break
     *        this class.
     * \throws std::invalid_argument if the event tree doesn't contain all
     *         trigger branches.
     */
    explicit trigger_values(TTree &event_tree);

    /// \brief Cannot be copied.
    trigger_values(const trigger_values &) = delete;

    /**
     * \brief Retrieves the value of the `i`th trigger branch without bound
     *        checking.
     */
    unsigned long long operator[] (std::size_t i) const { return _data[i]; }

    /**
     * \brief Retrieves the value of the `i`th trigger branch with bound
     *        checking.
     * \throws std::out_of_range if \c i is out of bounds.
     */
    unsigned long long at(std::size_t i) const { return _data.at(i); }
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
    std::array<std::shared_ptr<trigger>, trigger::count> _triggers;

public:
    /**
     * \brief Constructor.
     * \throws std::invalid_argument if \c bitFieldsChain is empty.
     */
    explicit trigger_mask(TTree &bitFieldsChain);

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

    /// \brief Checks whether the given \c values contain an accepted trigger.
    bool passes(const trigger_values &values) const;

    /**
     * \brief Retrieves the scale factor for the given trigger values.
     *
     * When an event passes several triggers, the highest scale factor is
     * returned.
     */
    double scale_factor(const trigger_values &values) const;
};

#endif // TRIGGERS_H
