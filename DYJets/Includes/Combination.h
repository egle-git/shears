#ifndef _COMBINATION_H_
#define _COMBINATION_H_

#include <TString.h>

void Combination(TString unfoldDir, TString combDir, TString algo, int jetPtMin, int jetEtaMax, bool diagXChanCov, bool fullXChanCov, bool fullSChanCov, bool modifiedSWA, TString gen1, TString gen2, TString variable = "", bool doNormalized = false);

#endif
