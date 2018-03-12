#include "standalone_LumiReWeighting.h"

#include <iostream>
#include <vector>

#include <TFile.h>
#include <TH1.h>
#include <TROOT.h>

// Set the pileup numbers here
double Data_2016_75Bin[75] = {
    6.54008e-06, 2.29383e-05, 6.32223e-05, 8.55796e-05, 0.000122592, 0.000164214, 0.000191738,
    0.000353072, 0.000965735, 0.00215544,  0.00484612,  0.00986199,  0.0165083,   0.0240058,
    0.0321661,   0.0407818,   0.0481844,   0.0532395,   0.0561219,   0.0575573,   0.058412,
    0.0588587,   0.0583078,   0.056491,    0.0537587,   0.0504445,   0.0466722,   0.0425747,
    0.0383287,   0.0340574,   0.0298202,   0.0256705,   0.0216922,   0.0179859,   0.0146378,
    0.011698,    0.00917774,  0.00705846,  0.00530639,  0.00388441,  0.00275715,  0.00189013,
    0.00124723,  0.000790064, 0.000479456, 0.00027833,  0.000154405, 8.18145e-05, 4.14101e-05,
    2.00435e-05, 9.30691e-06, 4.17785e-06, 1.84606e-06, 8.3504e-07,  4.1498e-07,  2.45829e-07,
    1.77914e-07, 1.48846e-07, 1.33924e-07, 1.23839e-07, 1.1526e-07,  1.07079e-07, 9.89863e-08,
    9.09467e-08, 8.30145e-08, 7.52676e-08, 6.77837e-08, 6.06312e-08, 5.38664e-08, 4.75325e-08,
    4.16595e-08, 3.62653e-08, 3.1356e-08,  2.69281e-08, 2.29691e-08};
double Data_2016_75Bin_up[75] = {
    6.36738e-06, 1.791e-05,   5.96778e-05, 7.48627e-05, 0.000111141, 0.000147297, 0.000174383,
    0.000244988, 0.000641801, 0.00145964,  0.00313022,  0.00664234,  0.0120019,   0.0184484,
    0.0254731,   0.0331281,   0.0408693,   0.0471696,   0.051383,    0.0537788,   0.0550037,
    0.0557688,   0.0561817,   0.0557359,   0.0541758,   0.0517754,   0.0488409,   0.0454911,
    0.0418221,   0.0379803,   0.0340885,   0.0302104,   0.0263853,   0.0226707,   0.0191466,
    0.0158944,   0.012975,    0.0104179,   0.00822324,  0.00637088,  0.00483131,  0.00357382,
    0.00256928,  0.00178905,  0.0012031,   0.00077952,  0.000485761, 0.000290742, 0.000166985,
    9.1982e-05,  4.85913e-05, 2.46338e-05, 1.20087e-05, 5.65663e-06, 2.60325e-06, 1.19912e-06,
    5.7966e-07,  3.15854e-07, 2.05819e-07, 1.59276e-07, 1.37772e-07, 1.25666e-07, 1.16875e-07,
    1.09147e-07, 1.01708e-07, 9.43372e-08, 8.7012e-08,  7.97785e-08, 7.27011e-08, 6.58448e-08,
    5.92677e-08, 5.30186e-08, 4.71359e-08, 4.16474e-08, 3.65712e-08};
double Data_2016_75Bin_dn[75] = {
    6.78636e-06, 2.95397e-05, 6.66311e-05, 9.81548e-05, 0.000137303, 0.000181169, 0.000223348,
    0.000555878, 0.00144232,  0.00333402,  0.0075878,   0.0142298,   0.022196,    0.0309112,
    0.0403801,   0.0490472,   0.0551515,   0.0586526,   0.060356,    0.061318,    0.0618025,
    0.0611204,   0.0589958,   0.0558722,   0.0521085,   0.0478427,   0.0432573,   0.0385556,
    0.0338554,   0.0292184,   0.024725,    0.0204915,   0.0166341,   0.0132328,   0.0103171,
    0.00787363,  0.00586471,  0.00424535,  0.00297214,  0.00200314,  0.00129458,  0.000799764,
    0.000471201, 0.000264323, 0.00014102,  7.15224e-05, 3.44959e-05, 1.5851e-05,  6.97461e-06,
    2.97629e-06, 1.26951e-06, 5.76548e-07, 3.06621e-07, 2.03465e-07, 1.62507e-07, 1.43476e-07,
    1.31689e-07, 1.22086e-07, 1.13043e-07, 1.04117e-07, 9.52529e-08, 8.6516e-08,  7.80011e-08,
    6.98015e-08, 6.19982e-08, 5.46564e-08, 4.78247e-08, 4.15349e-08, 3.58033e-08, 3.06327e-08,
    2.60136e-08, 2.19265e-08, 1.83438e-08, 1.52323e-08, 1.25545e-08};
double MC_2016_75Bin[75] = {
    1.78653e-05, 2.56602e-05, 5.27857e-05, 8.88954e-05, 0.000109362, 0.000140973, 0.000240998,
    0.00071209,  0.00130121,  0.00245255,  0.00502589,  0.00919534,  0.0146697,   0.0204126,
    0.0267586,   0.0337697,   0.0401478,   0.0450159,   0.0490577,   0.0524855,   0.0548159,
    0.0559937,   0.0554468,   0.0537687,   0.0512055,   0.0476713,   0.0435312,   0.0393107,
    0.0349812,   0.0307413,   0.0272425,   0.0237115,   0.0208329,   0.0182459,   0.0160712,
    0.0142498,   0.012804,    0.011571,    0.010547,    0.00959489,  0.00891718,  0.00829292,
    0.0076195,   0.0069806,   0.0062025,   0.00546581,  0.00484127,  0.00407168,  0.00337681,
    0.00269893,  0.00212473,  0.00160208,  0.00117884,  0.000859662, 0.000569085, 0.000365431,
    0.000243565, 0.00015688,  9.88128e-05, 6.53783e-05, 3.73924e-05, 2.61382e-05, 2.0307e-05,
    1.73032e-05, 1.435e-05,   1.36486e-05, 1.35555e-05, 1.37491e-05, 1.34255e-05, 1.33987e-05,
    1.34061e-05, 1.34211e-05, 1.34177e-05, 1.32959e-05, 1.33287e-05};

standalone_LumiReWeighting::standalone_LumiReWeighting(int year, int mode, int nBin)
{

    // Add the pileup distributions to the map
    pileupMap.insert(std::pair<std::string, double *>("Data_2016_75Bin", Data_2016_75Bin));
    pileupMap.insert(std::pair<std::string, double *>("Data_2016_75Bin_up", Data_2016_75Bin_up));
    pileupMap.insert(std::pair<std::string, double *>("Data_2016_75Bin_dn", Data_2016_75Bin_dn));
    pileupMap.insert(std::pair<std::string, double *>("MC_2016_75Bin", MC_2016_75Bin));

    std::vector<double> MC_distr;
    std::vector<double> Lumi_distr;

    MC_distr.clear();
    Lumi_distr.clear();

    printf("standalone_LumiReWeighting Mode = %d\n", mode);

    char tmpName[50];
    char tmpNameMC[50];
    snprintf(tmpNameMC, 50, "%d_%dBin", year, nBin);
    switch (mode) {
    case 0:
        snprintf(tmpName, 50, "%d_%dBin", year, nBin);
        break;
    case 1:
        snprintf(tmpName, 50, "%d_%dBin_up", year, nBin);
        break;
    case -1:
        snprintf(tmpName, 50, "%d_%dBin_dn", year, nBin);
        break;
    default:
        printf("Mode for standalone lumi reweighting is incorrect\n");
        abort();
        break;
    }
    std::string dataName = "Data_";
    dataName += tmpName;
    std::string mcName = "MC_";
    mcName += tmpNameMC;

    std::cout << "standalone_LumiReWeighting dataName = " << dataName << std::endl;
    std::cout << "standalone_LumiReWeighting mcName = " << mcName << std::endl;

    for (int i = 0; i < nBin; ++i) {
        Lumi_distr.push_back(pileupMap[dataName][i]);
        MC_distr.push_back(pileupMap[mcName][i]);
    }

    // first, check they are the same size...
    if (MC_distr.size() != Lumi_distr.size()) {
        std::cout << "MC_distr.size() = " << MC_distr.size() << std::endl;
        std::cout << "Lumi_distr.size() = " << Lumi_distr.size() << std::endl;
        std::cerr << "ERROR:standalone_LumiReWeighting: input vectors have different sizes. "
                     "Quitting... \n";
    }

    weights_ =
        new TH1D(Form("luminumer_%d", mode), Form("luminumer_%d", mode), nBin, 0., double(nBin));

    weights_->SetBit(TH1::kIsAverage);

    TH1D *den =
        new TH1D(Form("lumidenom_%d", mode), Form("lumidenom_%d", mode), nBin, 0., double(nBin));

    den->SetBit(TH1::kIsAverage);

    for (int ibin = 1; ibin < nBin + 1; ++ibin) {
        weights_->SetBinContent(ibin, Lumi_distr[ibin - 1]);
        den->SetBinContent(ibin, MC_distr[ibin - 1]);
    }

    // check integrals, make sure things are normalized

    printf("Normalizing Data PU: %F\n", weights_->Integral());
    weights_->Scale(1.0 / weights_->Integral());
    printf("Normalizing MC PU: %F\n", den->Integral());
    den->Scale(1.0 / den->Integral());

    printf("Data norm = %F  |||  MC norm = %F\n", weights_->Integral(), den->Integral());

    // std::cout << "      Data Input        MC Input         tmp Input" << std::endl;
    // for(int ibin = 1; ibin<nBin+1; ++ibin){
    //  std::cout <<ibin-1 << "     " << weights_->GetBinContent(ibin) <<"       "<<
    //  den->GetBinContent(ibin) << "         "<<tmp->GetBinContent(ibin)<<std::endl;
    //}

    weights_->Divide(den);

    //  weights_->Scale(1/weights_->Integral());//Bugra Bilin, added this to normalize weigts.

    // std::cout << "Reweighting: Computed Weights per In-Time Nint " << std::endl;

    // for(int ibin = 1; ibin<nBin+1; ++ibin){
    // std::cout << "   " << ibin-1 << " " << weights_->GetBinContent(ibin) << std::endl;
    //}

    //   weightOOT_init();
    // std::cout << "=======================================================================" <<
    // std::endl;
}

standalone_LumiReWeighting::~standalone_LumiReWeighting() {}

double standalone_LumiReWeighting::weight(int npv)
{
    int bin = weights_->GetXaxis()->FindBin(npv);
    return weights_->GetBinContent(bin);
}
