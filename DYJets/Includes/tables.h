#ifndef TABLES_H
#define TABLES_H

#include <string>
#include <vector>

namespace util
{

class record
{
  public:
    double ptLow, ptHi, etaLow, etaHi, effi, effiErrorLow, effiErrorHigh;

    explicit record() = default;
    explicit record(double, double, double, double, double, double, double);

    bool belongToEta(double) const;
    bool belongTo(double, double) const;
    bool equalTo(int num) const;
};

class table
{
  public:
    explicit table() = default;
    explicit table(const std::string &filename);

    double getEfficiency(double, double) const;
    double getEfficiencyLow(double, double) const;
    double getEfficiencyHigh(double, double) const;
    double getTTbarSF(int) const;
    double getTTbarSFLow(int) const;
    double getTTbarSFHigh(int) const;

  private:
    std::vector<record> recd;
};
} // namespace util

#endif // TABLES_H
