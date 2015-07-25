#ifndef __fileNames__
#define __fileNames__

//-- directory of input root files --------------------
const string FILESDIRECTORY("HistoFilesAugust/");
//---------- lets add basic information on samples inot common struct -------------------------------------------
struct processInfoStruct{
    string filename;
    double NEvents, xsec, xsecFrac, xsecError;
    int color;
    string legend;
};

const processInfoStruct ProcessInfo[] = {
    //--  Name  ---------------------------- #events ---- xsec - branch - xsec error (%) -- color for plot -- name on legend
    {"Data_dR",                           1,          1.,      1,         1,             kBlack,      " Data"},
    {"WJetsALL_MIX_UNFOLDING_dR",         76102995.,  36864.,  1,         0.03,          kPink,       " WJets"},
    {"ZZ_dR",	                          9799908.,   17.654,  1,         0.04,          kOrange,     " ZZ"},    
    {"WZ_dR",	                          10000283.,  33.21,   1,         0.05,          kRed+1,      " WZ"},	
    {"WW_dR",	                          10000431.,  54.838,  1,         0.05,          kViolet+5,   " WW"},
    {"T_s_channel_dR",	                  259961.,    3.79,    1,         0.10,          kMagenta,    " Ts"},
    {"T_t_channel_dR",	                  3758227.,   56.4,    1,         0.10,          kMagenta,    " Tt"},
    {"T_tW_channel_dR",	                  497658.,    11.1,    1,         0.10,          kMagenta,    " Ttw"},
    {"Tbar_s_channel_dR",                 139974.,    1.76,    1,         0.10,          kMagenta,    " Tbars"},
    {"Tbar_t_channel_dR",                 1903681.,   30.7,    1,         0.10,          kMagenta,    " Tbart"},
    {"Tbar_tW_channel_dR",                493460.,    11.1,    1,         0.10,          kMagenta,    " TbartW"},
    {"TTJets_dR",		                  6923652.,   234.,    1,         0.10,          kBlue,       " TTJets"},  
    {"DYJets10to50_dR",	                  11707222.,  860.5,   1,         0.04,          kAzure-4,    " DY"},
    {"DYJetsToLL_MIX_50toInf_UNFOLDING_dR",      30459503.,  3531.8,  1,         0.04,          kAzure+10,   " DYJets"}, /// up to this line files are set for W+jet s and TTbar
    {"ZZJets2L2Nu_dR",		              954911.,    17.654,  0.04039,   0.04,          kSpring+5,   " ZZJets2L2Nu"},
    {"ZZJets4L_dR",		                  4807893.,   17.654,  0.010196,  0.04,          kOrange,     " ZZJets4L"},
    {"ZZJets2L2Q_dR",		              1936727.,   17.654,  0.14118,   0.04,          kOrange-3,   " ZZJets2L2Q"},
    {"WZJets3LNu_dR",		              1995334.,   33.21,   0.032887,  0.04,          kRed+1,      " WZJets3LNu"},
    {"WZJets2L2Q_dR",		              3215990.,   33.21,   0.068258,  0.04,          kPink-6,     " WZJets2L2Q"},
    {"WWJets2L2Nu_dR",		              1933235.,   54.838,  0.10608 ,  0.04,          kViolet+5,   " WWJets2L2Nu"},
    {"Top_dR",		                      1.,         1,       1,         0.04,          kMagenta,    " Single Top"},
    {"DYJetsToLL_FromTau_50toInf_UNFOLDING_dR",   30459503.,  3531.8,  1,         0.033,         kAzure+4,    " DYtautau"},
    {"DYJets10toInf3_dR",	              1.,         1,       1,         0.04,          kAzure+10,   " DY"},
    {"DataQCD_dR",	                      1.,         1,       1,         0.04,          kYellow,     " QCD"},
    {"WJetsALL_MIX_UNFOLDING_dR",         76102995.,  36864.,  1,         0.03,          kPink,       " WJets"},
    {"WJetsALL_MIX_dR",                   76102995.,  36864.,  1,         0.03,          kPink,       " WJets"}, // relative weight for mixed DY and WJ files are set inthe code
    {"WJetsALL_dR",                       76102995.,  36864.,  1,         0.03,          kPink,       " WJets"},
    {"DYJetsToLL_50toInf_UNFOLDING_dR",   30459503.,  3531.8,  1,         0.04,          kAzure+10,   " DYJets"}, /// up to this line files are set for W+jet s and TTbar
};

//--- first element must point to the data
//--- last element must point to the MC Signal
const int NFilesFull(28); 
const int NFILESWJETS(14); 
const int NFILESDYJETS(12);
const int FilesDYJets[NFILESDYJETS] = {0, 1, 21, 15, 20, 14, 16, 19, 18, 17, 11, 13};
const int NFILESTTBAR(9);
const int FilesTTbarWJets[NFILESTTBAR] = {0, 4, 3, 2, 23, 20, 22, 11, 24};
const int FilesTTbar[NFILESTTBAR] = {0, 4, 3, 2, 23, 20, 22, 11, 24};
const int DATAFILENAME(0);
const int DYMADGRAPHFILENAME(13); // Exclusive files
//const int DYMADGRAPHFILENAME(27); // Inclusive file

//-- file names ---------------------------------------
const string FILENAMESTTbar[NFILESTTBAR] = {
    "Data_dR",
    "ZZ_dR",
    "WZ_dR",
    "WW_dR",
    "WJetsALL_UNFOLDING_dR",
    "DataQCD_dR",
    "Top_dR",
    "TTJets_dR",
    "DYJets10toInf3_dR",
};
const int NFILESVJETS(14);
const string FILENAMES[NFILESVJETS] = {
    "Data_dR",
    "WJetsALL_MIX_dR",
    "DYJets_FromTau_UNFOLDING_dR_Inf3",
    "Top_dR",
    "ZZJets2L2Nu_dR",
    "ZZJets4L_dR",
    "ZZJets2L2Q_dR",
    "WZJets3LNu_dR",
    "WZJets2L2Q_dR",
    "WWJets2L2Nu_dR",
    "TTJets_dR",
    //"DYJets_UNFOLDING_NOLEPTON",
    //"DYJets_UNFOLDING_dR_Inf3",
    "DYJets_MIX_UNFOLDING_dR_Inf3",
    // "DY01234Jets_UNFOLDING_dR_Inf3",
    //"DYJets_PowhegNLO1Jet_GEN",
    "DYJets_PowhegZ2jMiNLO_dR_GEN_CernMOJ",
    //"DYJets_UNFOLDING_Sherpa",
    //"DY_Sherpa_1NLO4_HepMC_dR_Full_List",
    "DY_Sherpa_2NLO4_HepMC_dR_Full_ListALL"
}; 
//-----------------------------------------------------


const string DYPOWHEGFILENAME = FILENAMES[12];
const string DYSHERPAFILENAME = FILENAMES[13];
const string DYSHERPA2NLOFILENAME ="DY_Sherpa_2NLO4_HepMC_dR_Full_ListALL";

const string BGFILENAMES[9] = {
    FILENAMES[1],
    FILENAMES[2],
    FILENAMES[3],
    FILENAMES[4],
    FILENAMES[5],
    FILENAMES[6],
    FILENAMES[7],
    FILENAMES[8],
    FILENAMES[9],
};
const string BGFILENAMESGrouped[8] = {
    FILENAMES[1],
    FILENAMES[2],
    FILENAMES[3],
    FILENAMES[4],
    FILENAMES[5],
    FILENAMES[6],
    FILENAMES[7],
    FILENAMES[9],
};

const string GenMCFILENAMES[5] = {
    "DYJets_GEN_Z2_MPIoff",
    "DYJets_GEN_Z2Star",
    "DYJets_GEN_Z2Star_MPIoff",
    "DYJets_P8_ZmumJets_MPIon_GEN_Tune4C",
    "DYJets_PowhegZjjMiNLO_GEN"
};
const string DYPOWHEGZjjFILENAME = GenMCFILENAMES[4];
const string DYSHERPAZjFILENAME("DYJets_Sherpa_UNFOLDING_dR_Inf");

#endif

