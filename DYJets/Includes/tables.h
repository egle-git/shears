#ifndef TABLES_H
#define TABLES_H

#include <string>
#include <vector>

namespace util
{

/**
 * \brief Handles table of scale factors.
 *
 * The class reads a table from the filesystem, and provides access to its entries.
 */
class table
{
    /// \brief A bin
    struct record
    {
        double ptLow, ptHi, etaLow, etaHi, effi, effiErrorLow, effiErrorHigh;

        bool belongToEta(double) const;
        bool belongTo(double, double) const;
        bool equalTo(int num) const;
    };

    std::vector<record> _recd;

  public:
    /// \brief Creates an invalid table.
    explicit table() = default;

    /**
     * \brief Reads a table from the file provided in argument.
     * \throws std::invalid_argument if the file doesn't exist
     * \warning The reader isn't very robust. Be careful when adding tables!
     */
    explicit table(const std::string &filename);

    double getEfficiency(double, double) const;
    double getEfficiencyLow(double, double) const;
    double getEfficiencyHigh(double, double) const;
    double getTTbarSF(int) const;
    double getTTbarSFLow(int) const;
    double getTTbarSFHigh(int) const;
};
} // namespace util

#endif // TABLES_H
