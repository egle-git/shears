#ifndef _FinalUnfold_h_
#define _FinalUnfold_h_
#include "getFilesAndHistograms.h"
#include <iostream>

using namespace std;

void FinalUnfold(int start = 0, int end = -1);
void FuncUnfold(bool isMuon, string variable = "ZNGoodJets_Zexc", int UsedKtermBayes = 5, int UsedKtermSVD = 5, bool doFlat = false , bool doVarWidth = true);

#endif
