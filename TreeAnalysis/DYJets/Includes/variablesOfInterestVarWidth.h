#ifndef __variablesOfInterest__
#define __variablesOfInterest__

#include <TObject.h>
#include <TObjString.h>
#include <TList.h>

struct variableStruct{
   string name;
   bool log, decrease;  
   int MuSVDkterm, MuBayeskterm, ESVDkterm, EBayeskterm, BayesktermCombine;
};

//--- Z Jets ------------------------------------

const int NVAROFINTERESTZJETS(64);
variableStruct VAROFINTERESTZJETS[] = {
   //--  Name  ------------------ log - decrease - Mu SVD kterm - Mu Bayes kterm - E SVD kterm - E Bayes kterm  - E Bayes kterm COMBINE
   {"ZNGoodJets_Zexc",             1,      1,           6,              4,             3,            3 ,               3    }, // 0

   //-- jet Pt distribution
   {"FirstJetPt_Zinc1jet",         1,      1,           9,              5,             9,            9 ,               6    }, // 1
   {"FirstJetPt_2_Zinc1jet",       1,      1,          14,              6,            14,           10 ,               7    }, // 2
   {"SecondJetPt_Zinc2jet",        1,      1,          11,              8,            11,            7 ,               7    }, // 3
   {"SecondJetPt_2_Zinc2jet",      1,      1,          15,              8,            14,            8 ,               8    }, // 4
   {"ThirdJetPt_Zinc3jet",         1,      1,           8,              9,             8,            7 ,               7    }, // 5
   {"FourthJetPt_Zinc4jet",        1,      1,           6,              4,             6,            5 ,               4    }, // 6
   {"FifthJetPt_Zinc5jet",         1,      1,           4,              2,             4,            3 ,               2    }, // 7 

   {"FirstHighestJetPt_Zinc1jet",  1,      1,           7,              4,             8,            3 ,               7    }, // 8
   {"FirstHighestJetPt_Zinc2jet",  1,      1,           9,              4,             8,            4 ,               7    }, // 9
   {"FirstHighestJetPt_Zinc3jet",  1,      1,           8,              4,             8,            3 ,               7    }, // 10
   {"FirstHighestJetPt_Zinc4jet",  1,      0,           5,              2,             7,            5 ,               7    }, // 11
   {"FirstHighestJetPt_Zinc5jet",  1,      0,           9,              3,             5,            5 ,               7    }, // 12

   {"SecondHighestJetPt_Zinc2jet", 1,      1,           9,              3,            11,            3 ,               7    }, // 13
   {"SecondHighestJetPt_Zinc3jet", 1,      1,          10,              3,            10,            4 ,               7    }, // 14
   {"SecondHighestJetPt_Zinc4jet", 1,      1,           9,              3,             9,            3 ,               7    }, // 15
   {"SecondHighestJetPt_Zinc5jet", 1,      0,           4,              2,             6,            4 ,               7    }, // 16
  
   {"ThirdHighestJetPt_Zinc3jet",  1,      1,           6,              2,             6,            2 ,               7    }, // 17
   {"ThirdHighestJetPt_Zinc4jet",  1,      1,           6,              3,             5,            2 ,               7    }, // 18
   {"ThirdHighestJetPt_Zinc5jet",  1,      1,           5,              3,             3,            3 ,               7    }, // 19

   //-- jets HT distribution
   {"JetsHT_Zinc1jet",             1,      1,           7,              4,             7,            3 ,               3    }, // 20
   {"JetsHT_1_Zinc1jet",           1,      1,           9,              4,             8,            4 ,               4    }, // 21
   {"JetsHT_2_Zinc1jet",           1,      1,           9,              4,             8,            4 ,               4    }, // 22
   {"JetsHT_Zinc2jet",             1,      1,           7,              3,             9,            4 ,               2    }, // 23
   {"JetsHT_1_Zinc2jet",           1,      1,           7,              3,             8,            4 ,               2    }, // 24
   {"JetsHT_2_Zinc2jet",           1,      1,           7,              3,             8,            4 ,               2    }, // 25
   {"JetsHT_Zinc3jet",             1,      1,           6,              3,             6,            6 ,               3    }, // 26
   {"JetsHT_Zinc4jet",             1,      1,           5,              5,             4,            4 ,               3    }, // 27
   {"JetsHT_Zinc5jet",             1,      1,           3,              2,             4,            3 ,               1    }, // 28       

   //-- jet Eta distribution
   {"FirstJetEta_Zinc1jet",        0,      1,          28,              4,            18,            4 ,               5    }, // 29
   {"SecondJetEta_Zinc2jet",       0,      1,          11,              5,            16,            5 ,               5    }, // 30
   {"ThirdJetEta_Zinc3jet",        0,      1,          10,              4,             8,            4 ,               5    }, // 31
   {"FourthJetEta_Zinc4jet",       0,      1,           4,              4,             4,            5 ,               4    }, // 32
   {"FifthJetEta_Zinc5jet",        0,      1,           3,              2,             2,            2 ,               1    }, // 33    

   //-- Z Mass distribution
   //{"ZMass_Zinc0jet",              0,      1,          19,             17,             7,          6 ,               7    },

   //-- Jets Mass distribution
   {"JetsMass_Zexc2jet",           0,      1,           7,              5,            13,           11 ,               4    }, // 34
   {"JetsMass_Zinc2jet",           0,      1,           9,              5,            13,           11 ,               5    }, // 35

   //-- DPS distribution
   {"SpTJets_Zexc2jet",            0,      0,           7,              4,             6,            4 ,               3    }, // 36
   {"SpTJets_Zinc2jet",            0,      0,           7,              4,             8,            4 ,               3    }, // 37
   {"SpT_Zexc2jet",                0,      0,          14,              3,             8,            3 ,               2    }, // 38
   {"SpT_Zinc2jet",                0,      0,          16,              3,             5,            3 ,               2    }, // 39
   {"dPhiJets_Zexc2jet",           0,      0,           4,              4,             4,            4 ,               3    }, // 40
   {"dPhiJets_Zinc2jet",           0,      0,           6,              4,             8,            4 ,               3    }, // 41
   {"PHI_Zexc2jet",                0,      0,           5,              5,             6,            4 ,               3    }, // 42
   {"PHI_Zinc2jet",                0,      0,          14,              4,            10,            4 ,               3    }, // 43
   {"PHI_T_Zexc2jet",              0,      0,           6,              6,             4,            6 ,               5    }, // 44
   {"PHI_T_Zinc2jet",              0,      0,           6,              6,             7,            5 ,               5    }, // 45
   {"SPhi_Zexc2jet",               0,      0,          18,              4,             9,            4 ,               3    }, // 46
   {"SPhi_Zinc2jet",               0,      0,           7,              4,             9,            4 ,               3    }, // 47

   {"SpTJets_LowPt_Zexc2jet",      0,      1,           9,             12,             7,            9 ,               5    }, // 48
   {"SpTJets_LowPt_Zinc2jet",      0,      1,           8,             12,             9,            8 ,               6    }, // 49
   {"SpT_LowPt_Zexc2jet",          0,      1,           6,             13,             4,            9 ,               7    }, // 50
   {"SpT_LowPt_Zinc2jet",          0,      1,           6,             12,            14,            9 ,               7    }, // 51
   {"dPhiJets_LowPt_Zexc2jet",     0,      0,           6,             13,             4,            9 ,               5    }, // 52
   {"dPhiJets_LowPt_Zinc2jet",     0,      0,           4,             12,             8,            9 ,               6    }, // 53
   {"PHI_LowPt_Zexc2jet",          0,      1,           7,              5,             4,            8 ,               3    }, // 54
   {"PHI_LowPt_Zinc2jet",          0,      1,           5,              5,             2,           15 ,               3    }, // 55
   {"PHI_T_LowPt_Zexc2jet",        0,      0,           3,              5,             7,            6 ,               4    }, // 56
   {"PHI_T_LowPt_Zinc2jet",        0,      0,           3,              4,             3,            6 ,               4    }, // 57
   {"SPhi_LowPt_Zexc2jet",         0,      0,          11,              7,             3,            6 ,               7    }, // 58
   {"SPhi_LowPt_Zinc2jet",         0,      0,           8,              7,             6,            6 ,               7    }, // 59
   // additional correlations  =----- NEED TP CHECK THE UNFOLDING PARAMTERS !!!!
   {"dEtaJets_Zinc2jet",           0,      0,           8,             10,             6,            6 ,               4    }, // 60
   {"dEtaJets_Zexc2jet",           0,      0,           8,             10,             6,            6 ,               3    }, // 61
   {"dEtaBosonJet_Zinc1",          0,      0,           8,             10,             6,            6 ,               8    }, // 62
   {"dEtaBosonJet_Zexc1",          0,      0,           8,             10,             6,            6 ,               8    }, // 63
//   {"dEtaFirstJetZ_Zinc2jet",      0,      0,           8,             10,             6,            6 ,               7    },
//   {"dEtaSecondJetZ_Zinc2jet",     0,      0,           8,             10,             6,            6 ,               7    }

}; 

TList *LISTOFVAROFINTERESTZJETS = new TList();

void makeLISTOFVAROFINTERESTZJETS(){
    for (int i(0); i < NVAROFINTERESTZJETS; i++){
        TObjString *name = new TObjString(VAROFINTERESTZJETS[i].name.c_str());
        LISTOFVAROFINTERESTZJETS->Add(name);
    }
}

//--- W Jets ------------------------------------

const int NVAROFINTERESTWJETS(55);
variableStruct VAROFINTERESTWJETS[] = {
    //--  Name  ------------------ log - decrease - Mu SVD kterm - Mu Bayes kterm - E SVD kterm - E Bayes kterm
    {"ZNGoodJets_Zexc",             1,      1,           4,              4,             4,            3,           7},

    //-- jet Pt distribution
    {"FirstJetPt_Zinc1jet",         1,      1,           9,              5,             9,            5,           7},
    {"FirstJetPt_2_Zinc1jet",       1,      1,          14,              6,            14,           10 ,          7},
    {"SecondJetPt_Zinc2jet",        1,      1,           8,              6,             8,            7,           7}, 
    {"ThirdJetPt_Zinc3jet",         1,      1,           5,              4,             5,            5,           7}, 
    {"FourthJetPt_Zinc4jet",        1,      1,           5,              3,             4,            4,           7}, 
    {"FifthJetPt_Zinc5jet",         1,      1,           4,              3,             3,            2,           7},

    {"FirstHighestJetPt_Zinc1jet",  1,      1,           9,              4,             9,            3,           7},
    {"FirstHighestJetPt_Zinc2jet",  1,      1,           9,              7,             9,            4,           7},
    {"FirstHighestJetPt_Zinc3jet",  1,      1,           3,              4,             5,            4,           7},
    {"FirstHighestJetPt_Zinc4jet",  1,      0,           3,              5,             3,            5,           7},
    {"FirstHighestJetPt_Zinc5jet",  1,      0,           3,              2,             2,            2,           7},

    {"SecondHighestJetPt_Zinc2jet", 1,      1,           8,              3,             8,            3,           7},
    {"SecondHighestJetPt_Zinc3jet", 1,      1,           5,              4,             6,            4,           7},
    {"SecondHighestJetPt_Zinc4jet", 1,      1,           3,              4,             3,            5,           7},
    {"SecondHighestJetPt_Zinc5jet", 1,      0,           3,              2,             2,            4,           7},

    {"ThirdHighestJetPt_Zinc3jet",  1,      1,           5,              2,             5,            4,           7},
    {"ThirdHighestJetPt_Zinc4jet",  1,      1,           4,              3,             5,            2,           7},
    {"ThirdHighestJetPt_Zinc5jet",  1,      1,           3,              3,             3,            3,           7},

    //-- jets HT distribution
    {"JetsHT_Zinc1jet",             1,      1,           8,              4,             9,            3,           7}, 
    {"JetsHT_Zinc2jet",             1,      1,          10,              4,            10,            4,           7},
    {"JetsHT_Zinc3jet",             1,      1,           7,              3,             7,            5,           7},
    {"JetsHT_Zinc4jet",             1,      1,           6,              3,             5,            9,           7},
    {"JetsHT_Zinc5jet",             1,      1,           3,              2,             4,            2,           7},

    //-- jet Eta distribution
    {"FirstJetEta_Zinc1jet",        0,      1,          13,              3,            15,            3,           7},
    {"SecondJetEta_Zinc2jet",       0,      1,          11,              5,            11,            4,           7}, 
    {"ThirdJetEta_Zinc3jet",        0,      1,          10,              4,             8,            4,           7}, 
    {"FourthJetEta_Zinc4jet",       0,      1,           5,              4,             3,            4,           7}, 
    {"FifthJetEta_Zinc5jet",        0,      1,           3,              2,             2,            2,           7}, 

    //-- Z Mass distribution
    //{"ZMass_Zinc0jet",              0,      1,          19,             17,             7,         36,           7},

    //-- Jets Mass distribution
    {"JetsMass_Zexc2jet",           0,      1,           9,              6,            10,            7,           7},
    {"JetsMass_Zinc2jet",           0,      1,           9,              7,             9,            6,           7},

    //-- DPS distribution
    {"SpTJets_Zexc2jet",            0,      0,           7,              4,             5,            4,           7},
    {"SpTJets_Zinc2jet",            0,      0,           7,              5,             5,            4,           7},
    {"SpT_Zexc2jet",                0,      0,          10,              4,             6,            4,           7},
    {"SpT_Zinc2jet",                0,      0,          11,              5,             6,            3,           7},
    {"dPhiJets_Zexc2jet",           0,      0,           7,              4,             5,            3,           3},
    {"dPhiJets_Zinc2jet",           0,      0,           7,              4,             8,            3,           3},
    {"PHI_Zexc2jet",                0,      0,           5,              5,             6,            6,           7},
    {"PHI_Zinc2jet",                0,      0,          14,              6,            10,            5,           7},
    {"PHI_T_Zexc2jet",              0,      0,           5,              7,             5,            6,           7},
    {"PHI_T_Zinc2jet",              0,      0,           5,              7,             4,            6,           7},
    {"SPhi_Zexc2jet",               0,      0,          18,              5,             9,            6,           7},
    {"SPhi_Zinc2jet",               0,      0,           7,              5,             9,            5,           7},

    {"SpTJets_LowPt_Zexc2jet",      0,      1,           4,              7,             6,           11,           7},
    {"SpTJets_LowPt_Zinc2jet",      0,      1,           4,              7,             9,            8,           7},
    {"SpT_LowPt_Zexc2jet",          0,      1,           4,              7,             3,            5,           7},
    {"SpT_LowPt_Zinc2jet",          0,      1,           6,              8,             8,            5,           7},
    {"dPhiJets_LowPt_Zexc2jet",     0,      0,           4,              6,             6,            7,           7},
    {"dPhiJets_LowPt_Zinc2jet",     0,      0,           4,              7,             2,            6,           7},
    {"PHI_LowPt_Zexc2jet",          0,      1,           7,              6,             4,           14,           7},
    {"PHI_LowPt_Zinc2jet",          0,      1,           5,              5,             2,           15,           7},
    {"PHI_T_LowPt_Zexc2jet",        0,      0,           3,              5,             7,            6,           7},
    {"PHI_T_LowPt_Zinc2jet",        0,      0,           3,              4,             3,            6,           7},
    {"SPhi_LowPt_Zexc2jet",         0,      0,          11,              7,             3,            6,           7},
    {"SPhi_LowPt_Zinc2jet",         0,      0,           8,              7,             6,            6,           7},
}; 

TList *LISTOFVAROFINTERESTWJETS = new TList();

void makeLISTOFVAROFINTERESTWJETS(){
    for (int i(0); i < NVAROFINTERESTWJETS; i++){
        TObjString *name = new TObjString(VAROFINTERESTWJETS[i].name.c_str());
        LISTOFVAROFINTERESTWJETS->Add(name);
    }
}
#endif

