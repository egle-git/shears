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

    record();
    record(double, double, double, double, double, double, double);
    bool belongToEta(double);
    bool belongTo(double, double);
    bool equalTo(int num);
};

class table
{
  public:
    explicit table() = default;
    explicit table(const std::string &filename);

    double getEfficiency(double, double);
    double getEfficiencyLow(double, double);
    double getEfficiencyHigh(double, double);
    double getTTbarSF(int);
    double getTTbarSFLow(int);
    double getTTbarSFHigh(int);

  private:
    std::vector<record> recd;
};
} // namespace util

#endif // TABLES_H
