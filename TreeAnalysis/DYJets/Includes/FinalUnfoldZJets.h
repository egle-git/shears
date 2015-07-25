#ifndef _FinalUnfoldZJets_h_
#define _FinalUnfoldZJets_h_
#include <iostream>
#include <TString.h>

using namespace std;

void FuncUnfold(bool isMuon, TString variable, int UsedKterm, int, int);
void FinalUnfold(int start = 0, int end = -1);

#endif
