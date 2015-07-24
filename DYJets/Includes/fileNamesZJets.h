#ifndef _FILENAMESZJETS_h_
#define _FILENAMESZJETS_h_

#include <TString.h>

//-- directory of input root files --------------------
const TString FILESDIRECTORY("HistoFilesAugust/");
//---------- lets add basic information on samples inot common struct -------------------------------------------
struct processInfoStruct{
    TString name;
    double NEvents, xsec, xsecFrac, xsecError;
    int colorAN, colorPAS;
    TString legendAN, legendPAS;
};

const int DATA(0);
const int DYJETS(11); // Exclusive files
const processInfoStruct Samples[] = {
    //--  Name  ------------------------------- #events -- xsec - branch - xsec error (%) - colorAN - colorPAS - name on legend
    {"Data_dR",                                  1,          1.,      1,         1,             kBlack,    kBlack,    " Data", " Data"},
    {"DYJetsToLL_FromTau_50toInf_UNFOLDING_dR",  30459503.,  3531.8,  1,         0.033,         kRed+1,    kRed+1,    " DYtautau", " Z/#gamma^{*} #rightarrow #tau#tau and W"},
    {"WJetsALL_MIX_UNFOLDING_dR",                76102995.,  36864.,  1,         0.03,          kGreen+3,  kRed+1,    " WJets", ""},
    {"TTJets_dR",		                         6923652.,   234.,    1,         0.10,          kPink-6,   kPink-6,   " TTJets", " t#bar{t} and single top"},  
    {"Top_dR",		                             1.,         1,       1,         0.04,          kPink-3,   kPink-6,   " Single Top", ""},
    {"ZZJets2L2Nu_dR",		                     954911.,    17.654,  0.04039,   0.04,          kAzure+4,  kAzure+4,  " ZZJets2L2Nu", " WW, WZ, ZZ"},
    {"ZZJets4L_dR",		                         4807893.,   17.654,  0.010196,  0.04,          kAzure+1,  kAzure+4,  " ZZJets4L", ""},
    {"ZZJets2L2Q_dR",		                     1936727.,   17.654,  0.14118,   0.04,          kAzure-2,  kAzure+4,  " ZZJets2L2Q", ""},
    {"WZJets3LNu_dR",		                     1995334.,   33.21,   0.032887,  0.04,          kBlue,     kAzure+4,  " WZJets3LNu", ""},
    {"WZJets2L2Q_dR",		                     3215990.,   33.21,   0.068258,  0.04,          kBlue-4,   kAzure+4,  " WZJets2L2Q", ""},
    {"WWJets2L2Nu_dR",		                     1933235.,   54.838,  0.10608 ,  0.04,          kBlue-6,   kAzure+4,  " WWJets2L2Nu", ""},
    {"DYJetsToLL_MIX_50toInf_UNFOLDING_dR",      30459503.,  3531.8,  1,         0.04,          kOrange-3, kOrange-3, " DYJets", " Z/#gamma^{*} #rightarrow"}, 
//    {"DYJetsToLL_50toInf_UNFOLDING_dR",          30459503.,  3531.8,  1,         0.04,          kAzure+10, kOrange-3,  " DYJets"}, 
};

//--- first element must point to the data
//--- last element must point to the MC Signal
const unsigned int NFILESDYJETS(12);
const unsigned int NBGDYJETS(NFILESDYJETS-2);
const unsigned int FilesDYJets[NFILESDYJETS] = {0, 1, 10, 4, 9, 3, 5, 8, 7, 6, 2, 11};

const TString DYPOWHEGFILENAME("DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights");
const TString DYAMCATNLOFILENAME("DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights");
const TString DYAMCATNLOLEGEND("MG5_aMC + PY8 (#leq 2j NLO + PS)");
const TString DYSHERPA2FILENAME("DYJets_Sherpa2_0_16000");
const TString DYSHERPA2LEGEND("SHERPA 2 (#leq 2j NLO 3,4j LO + PS)");
const TString DYSHERPA14FILENAME("DYJets_Sherpa_Bugra_1_13_UNFOLDING");
const TString DYSHERPA14LEGEND("Sherpa1.4 LO");
const TString DYMGPYTHIA8FILENAME("DYJetsToLL_M-50_TuneCUETP8M1_8TeV-MG-MLM-Bonzai");
const TString DYMGPYTHIA8LEGEND("MG+PYthia8 legend");

#endif
