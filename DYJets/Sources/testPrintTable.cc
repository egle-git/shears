#include "printTable.h"
#include <iostream>
#include "TString.h"

int main(){
  int ncols = 4;
  double x[] = { 1., 2., 3., 4., 5.};
  unsigned nentries = sizeof(x) / sizeof(x[0]);
  std::vector<std::string> colHeaders(ncols);
  std::vector<std::string> lineHeaders(nentries);
  std::vector<std::vector<double> > vals(nentries, std::vector<double>(ncols - 1, 0));
  std::string numFormat = "%#.2g";
  const char* caption = "Test of printTable function.";
  const char* label = "tab:ex";
  
  colHeaders[0] = "$x$";
  colHeaders[1] = "$x^2$";
  colHeaders[2] = "$x^3$";
  colHeaders[3] = "$x^4$";

  for(unsigned i = 0; i < nentries; ++i){
    lineHeaders[i] = TString::Format("%#g", x[i]).Data();
    vals[i][0] = x[i] * x[i];
    vals[i][1] = vals[i][1] * x[i];
    vals[i][2] = vals[i][1] * x[i];
  }

  return printTable(std::cout, colHeaders, lineHeaders, vals, numFormat, caption, label);
}
