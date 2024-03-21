#include <TFile.h>
#include <TH1D.h>
#include <TString.h>
#include <TVectorD.h>

using namespace std;

TH1D* make_density(TH1D* h);
TH1D* make_normal_from_density(TH1D* h);
double linear_interpolation(double x, double x0, double x1, double y0, double y1);
double get_m(double x, double x0, double x1, double y, double y0, double y1);
double cubic_hermite_spline_interpolation(double x, double x0, double x1, double y0, double y1, double m0, double m1);
TH1D* linear_interpolation(double x, double x0, double x1, TH1D* y0, TH1D* y1, TString name="histo");
TH1D* get_m(double x, double x0, double x1, TH1D* y, TH1D* y0, TH1D* y1, TString name="histo_m");
TH1D* cubic_hermite_spline_interpolation(double x, double x0, double x1, TH1D* y0, TH1D* y1, TH1D* m0, TH1D* m1, TString name="histo");

void InterpolateHists(int alt=0)
{
    TFile *f_in = new TFile("dyjets-Fakes.root", "READ");

    TH1D *h_elPt_leading                = (TH1D*)(f_in->Get("elPt_leading_inc0jet"));
    TH1D *h_elPt_leading_40_3000        = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass40_3000"));
    TH1D *h_elPt_leading_76_106_read    = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass76_106"));
    TH1D *h_elPt_leading_40_50          = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass40_50"));
    TH1D *h_elPt_leading_50_76          = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass50_76"));
    TH1D *h_elPt_leading_106_170        = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass106_170"));
    TH1D *h_elPt_leading_170_350        = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass170_350"));
    TH1D *h_elPt_leading_350_1000       = (TH1D*)(f_in->Get("elPt_leading_inc0jet_mass350_1000"));
    TH1D *h_elPt_subleading             = (TH1D*)(f_in->Get("elPt_subleading_inc0jet"));
    TH1D *h_elPt_subleading_40_3000     = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass40_3000"));
    TH1D *h_elPt_subleading_76_106_read = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass76_106"));
    TH1D *h_elPt_subleading_40_50       = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass40_50"));
    TH1D *h_elPt_subleading_50_76       = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass50_76"));
    TH1D *h_elPt_subleading_106_170     = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass106_170"));
    TH1D *h_elPt_subleading_170_350     = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass170_350"));
    TH1D *h_elPt_subleading_350_1000    = (TH1D*)(f_in->Get("elPt_subleading_inc0jet_mass350_1000"));
    TH1D *h_pt             = (TH1D*)(f_in->Get("pt_inc0jet"));
    TH1D *h_pt_40_3000     = (TH1D*)(f_in->Get("pt_inc0jet_mass40_3000"));
    TH1D *h_pt_76_106_read = (TH1D*)(f_in->Get("pt_inc0jet_mass76_106"));
    TH1D *h_pt_40_50       = (TH1D*)(f_in->Get("pt_inc0jet_mass40_50"));
    TH1D *h_pt_50_76       = (TH1D*)(f_in->Get("pt_inc0jet_mass50_76"));
    TH1D *h_pt_106_170     = (TH1D*)(f_in->Get("pt_inc0jet_mass106_170"));
    TH1D *h_pt_170_350     = (TH1D*)(f_in->Get("pt_inc0jet_mass170_350"));
    TH1D *h_pt_350_1000    = (TH1D*)(f_in->Get("pt_inc0jet_mass350_1000"));
    TH1D *h_mll_over_ptsub             = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet"));
    TH1D *h_mll_over_ptsub_40_3000     = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass40_3000"));
    TH1D *h_mll_over_ptsub_76_106_read = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass76_106"));
    TH1D *h_mll_over_ptsub_40_50       = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass40_50"));
    TH1D *h_mll_over_ptsub_50_76       = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass50_76"));
    TH1D *h_mll_over_ptsub_106_170     = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass106_170"));
    TH1D *h_mll_over_ptsub_170_350     = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass170_350"));
    TH1D *h_mll_over_ptsub_350_1000    = (TH1D*)(f_in->Get("mll_over_ptsub_inc0jet_mass350_1000"));
    TH1D *h_rapidity               = (TH1D*)(f_in->Get("rapidity_inc0jet"));
    TH1D *h_rapidity_40_3000       = (TH1D*)(f_in->Get("rapidity_inc0jet_mass40_3000"));
    TH1D *h_rapidity_76_106_read   = (TH1D*)(f_in->Get("rapidity_inc0jet_mass76_106"));
    TH1D *h_rapidity_40_50         = (TH1D*)(f_in->Get("rapidity_inc0jet_mass40_50"));
    TH1D *h_rapidity_50_76         = (TH1D*)(f_in->Get("rapidity_inc0jet_mass50_76"));
    TH1D *h_rapidity_106_170       = (TH1D*)(f_in->Get("rapidity_inc0jet_mass106_170"));
    TH1D *h_rapidity_170_350       = (TH1D*)(f_in->Get("rapidity_inc0jet_mass170_350"));
    TH1D *h_rapidity_350_1000      = (TH1D*)(f_in->Get("rapidity_inc0jet_mass350_1000"));
    TH1D *h_mass_read = (TH1D*)(f_in->Get("mass_wide_range_inc0jet"));

    h_elPt_leading               ->SetDirectory(0);
    h_elPt_leading_40_3000       ->SetDirectory(0);
    h_elPt_leading_76_106_read   ->SetDirectory(0);
    h_elPt_leading_40_50         ->SetDirectory(0);
    h_elPt_leading_50_76         ->SetDirectory(0);
    h_elPt_leading_106_170       ->SetDirectory(0);
    h_elPt_leading_170_350       ->SetDirectory(0);
    h_elPt_leading_350_1000      ->SetDirectory(0);
    h_elPt_subleading            ->SetDirectory(0);
    h_elPt_subleading_40_3000    ->SetDirectory(0);
    h_elPt_subleading_76_106_read->SetDirectory(0);
    h_elPt_subleading_40_50      ->SetDirectory(0);
    h_elPt_subleading_50_76      ->SetDirectory(0);
    h_elPt_subleading_106_170    ->SetDirectory(0);
    h_elPt_subleading_170_350    ->SetDirectory(0);
    h_elPt_subleading_350_1000   ->SetDirectory(0);
    h_pt                         ->SetDirectory(0);
    h_pt_40_3000                 ->SetDirectory(0);
    h_pt_76_106_read             ->SetDirectory(0);
    h_pt_40_50                   ->SetDirectory(0);
    h_pt_50_76                   ->SetDirectory(0);
    h_pt_106_170                 ->SetDirectory(0);
    h_pt_170_350                 ->SetDirectory(0);
    h_pt_350_1000                ->SetDirectory(0);
    h_mll_over_ptsub             ->SetDirectory(0);
    h_mll_over_ptsub_40_3000     ->SetDirectory(0);
    h_mll_over_ptsub_76_106_read ->SetDirectory(0);
    h_mll_over_ptsub_40_50       ->SetDirectory(0);
    h_mll_over_ptsub_50_76       ->SetDirectory(0);
    h_mll_over_ptsub_106_170     ->SetDirectory(0);
    h_mll_over_ptsub_170_350     ->SetDirectory(0);
    h_mll_over_ptsub_350_1000    ->SetDirectory(0);
    h_rapidity                   ->SetDirectory(0);
    h_rapidity_40_3000           ->SetDirectory(0);
    h_rapidity_76_106_read       ->SetDirectory(0);
    h_rapidity_40_50             ->SetDirectory(0);
    h_rapidity_50_76             ->SetDirectory(0);
    h_rapidity_106_170           ->SetDirectory(0);
    h_rapidity_170_350           ->SetDirectory(0);
    h_rapidity_350_1000          ->SetDirectory(0);
    h_mass_read                  ->SetDirectory(0);

    // INTERPOLATING MASS HISTO
    TH1D *h_mass_density = make_density(h_mass_read);
    TH1D *h_mass_test = (TH1D*)h_mass_density->Clone("h_mass_test");
    h_mass_test->SetDirectory(0);
    double y_avg_40_50    = 0;
    double y_avg_50_76    = 0;
    double y_avg_106_170  = 0;
    double y_avg_170_350  = 0;
    double y_avg_350_1000 = 0;
    double x_avg_40_50    = (40.0  +   50.0) / 2;
    double x_avg_50_76    = (50.0  +   76.0) / 2;
    double x_avg_76_106   = (76.0  +  106.0) / 2;
    double x_avg_106_170  = (106.0 +  170.0) / 2;
    double x_avg_170_350  = (170.0 +  350.0) / 2;
    double x_avg_350_1000 = (350.0 + 1000.0) / 2;

    int bin_count = 0;
    for (int i=h_mass_density->FindBin(40.01); i<=h_mass_density->FindBin(49.9); i++)
    {
        y_avg_40_50 += h_mass_density->GetBinContent(i);
        bin_count++;
    }
    y_avg_40_50 /= bin_count;
    for (int i=h_mass_density->FindBin(40.01); i<=h_mass_density->FindBin(49.9); i++)
        h_mass_test->SetBinContent(i, y_avg_40_50);

    bin_count = 0;
    for (int i=h_mass_density->FindBin(50.01); i<=h_mass_density->FindBin(75.9); i++)
    {
        y_avg_50_76 += h_mass_density->GetBinContent(i);
        bin_count++;
    }
    y_avg_50_76 /= bin_count;
    for (int i=h_mass_density->FindBin(50.01); i<=h_mass_density->FindBin(75.9); i++)
        h_mass_test->SetBinContent(i, y_avg_50_76);


    bin_count = 0;
    for (int i=h_mass_density->FindBin(106.01); i<=h_mass_density->FindBin(169.9); i++)
    {
        y_avg_106_170 += h_mass_density->GetBinContent(i);
        bin_count++;
    }
    y_avg_106_170 /= bin_count;
    for (int i=h_mass_density->FindBin(106.01); i<=h_mass_density->FindBin(169.9); i++)
        h_mass_test->SetBinContent(i, y_avg_106_170);


    bin_count = 0;
    for (int i=h_mass_density->FindBin(170.01); i<=h_mass_density->FindBin(349.9); i++)
    {
        y_avg_170_350 += h_mass_density->GetBinContent(i);
        bin_count++;
    }
    y_avg_170_350 /= bin_count;
    for (int i=h_mass_density->FindBin(170.01); i<=h_mass_density->FindBin(349.9); i++)
        h_mass_test->SetBinContent(i, y_avg_170_350);

    bin_count = 0;
    for (int i=h_mass_density->FindBin(350.01); i<=h_mass_density->FindBin(999.9); i++)
    {
        y_avg_350_1000 += h_mass_density->GetBinContent(i);
        bin_count++;
    }
    y_avg_350_1000 /= bin_count;
    for (int i=h_mass_density->FindBin(350.01); i<=h_mass_density->FindBin(999.9); i++)
        h_mass_test->SetBinContent(i, y_avg_350_1000);

    for (int i=h_mass_density->FindBin(76.01); i<=h_mass_density->FindBin(105.9); i++)
    {
        double x = h_mass_density->GetBinCenter(i);
        double y = 0;
        if (!alt) y = linear_interpolation(x, x_avg_50_76, x_avg_106_170, y_avg_50_76, y_avg_106_170);
        else
        {
            double m_50_76   = get_m(x_avg_50_76  , x_avg_40_50, x_avg_106_170, y_avg_50_76  , y_avg_40_50, y_avg_106_170);
            double m_106_170 = get_m(x_avg_106_170, x_avg_50_76, x_avg_170_350, y_avg_106_170, y_avg_50_76, y_avg_170_350);
            y = cubic_hermite_spline_interpolation(x, x_avg_50_76, x_avg_106_170, y_avg_50_76, y_avg_106_170, m_50_76, m_106_170);
        }
        h_mass_density->SetBinContent(i, y);
        h_mass_test->SetBinContent(i, y);
        h_mass_density->SetBinError(i, y*h_mass_density->GetBinWidth(i)); // 100% error
    }
    TH1D *h_mass = make_normal_from_density(h_mass_density);
    // TH1D *h_mass = h_mass_density;

    // INTERPOLATING OTHER HISTOS
    h_elPt_leading_40_50   ->Scale(1/(  50.0- 40.0));
    h_elPt_leading_50_76   ->Scale(1/(  76.0- 50.0));
    h_elPt_leading_106_170 ->Scale(1/( 170.0-106.0));
    h_elPt_leading_170_350 ->Scale(1/( 350.0-170.0));
    h_elPt_leading_350_1000->Scale(1/(1000.0-350.0));
    h_elPt_subleading_40_50   ->Scale(1/(  50.0- 40.0));
    h_elPt_subleading_50_76   ->Scale(1/(  76.0- 50.0));
    h_elPt_subleading_106_170 ->Scale(1/( 170.0-106.0));
    h_elPt_subleading_170_350 ->Scale(1/( 350.0-170.0));
    h_elPt_subleading_350_1000->Scale(1/(1000.0-350.0));
    h_pt_40_50   ->Scale(1/(  50.0- 40.0));
    h_pt_50_76   ->Scale(1/(  76.0- 50.0));
    h_pt_106_170 ->Scale(1/( 170.0-106.0));
    h_pt_170_350 ->Scale(1/( 350.0-170.0));
    h_pt_350_1000->Scale(1/(1000.0-350.0));
    h_mll_over_ptsub_40_50   ->Scale(1/(  50.0- 40.0));
    h_mll_over_ptsub_50_76   ->Scale(1/(  76.0- 50.0));
    h_mll_over_ptsub_106_170 ->Scale(1/( 170.0-106.0));
    h_mll_over_ptsub_170_350 ->Scale(1/( 350.0-170.0));
    h_mll_over_ptsub_350_1000->Scale(1/(1000.0-350.0));
    h_rapidity_40_50   ->Scale(1/(  50.0- 40.0));
    h_rapidity_50_76   ->Scale(1/(  76.0- 50.0));
    h_rapidity_106_170 ->Scale(1/( 170.0-106.0));
    h_rapidity_170_350 ->Scale(1/( 350.0-170.0));
    h_rapidity_350_1000->Scale(1/(1000.0-350.0));

    // Linear interpolation:
    TH1D *h_elPt_leading_76_106;
    TH1D *h_elPt_subleading_76_106;
    TH1D *h_pt_76_106;
    TH1D *h_mll_over_ptsub_76_106;
    TH1D *h_rapidity_76_106;
    if (!alt)
    {
        h_elPt_leading_76_106 = linear_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_elPt_leading_50_76, h_elPt_leading_106_170, "elPt_leading_inc0jet_mass76_106");
        h_elPt_subleading_76_106 = linear_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_elPt_subleading_50_76, h_elPt_subleading_106_170, "elPt_subleading_inc0jet_mass76_106");
        h_pt_76_106 = linear_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_pt_50_76, h_pt_106_170, "pt_inc0jet_mass76_106");
        h_mll_over_ptsub_76_106 = linear_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_mll_over_ptsub_50_76, h_mll_over_ptsub_106_170, "mll_over_ptsub_inc0jet_mass76_106");
        h_rapidity_76_106 = linear_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_rapidity_50_76, h_rapidity_106_170, "rapidity_inc0jet_mass76_106");
    }
    else 
    {
        TH1D* h_m_50_76   = get_m(x_avg_50_76  , x_avg_40_50, x_avg_106_170, h_elPt_leading_50_76  , h_elPt_leading_40_50, h_elPt_leading_106_170, "m_50_76"  );
        TH1D* h_m_106_170 = get_m(x_avg_106_170, x_avg_50_76, x_avg_170_350, h_elPt_leading_106_170, h_elPt_leading_50_76, h_elPt_leading_170_350, "m_106_170");
        h_elPt_leading_76_106 = cubic_hermite_spline_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_elPt_leading_50_76, h_elPt_leading_106_170, h_m_50_76, h_m_106_170, "elPt_leading_inc0jet_mass76_106");

        h_m_50_76   = get_m(x_avg_50_76  , x_avg_40_50, x_avg_106_170, h_elPt_subleading_50_76  , h_elPt_subleading_40_50, h_elPt_subleading_106_170, "m_50_76"  );
        h_m_106_170 = get_m(x_avg_106_170, x_avg_50_76, x_avg_170_350, h_elPt_subleading_106_170, h_elPt_subleading_50_76, h_elPt_subleading_170_350, "m_106_170");
        h_elPt_subleading_76_106 = cubic_hermite_spline_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_elPt_subleading_50_76, h_elPt_subleading_106_170, h_m_50_76, h_m_106_170, "elPt_subleading_inc0jet_mass76_106");

        h_m_50_76   = get_m(x_avg_50_76  , x_avg_40_50, x_avg_106_170, h_pt_50_76  , h_pt_40_50, h_pt_106_170, "m_50_76"  );
        h_m_106_170 = get_m(x_avg_106_170, x_avg_50_76, x_avg_170_350, h_pt_106_170, h_pt_50_76, h_pt_170_350, "m_106_170");
        h_pt_76_106 = cubic_hermite_spline_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_pt_50_76, h_pt_106_170, h_m_50_76, h_m_106_170, "pt_inc0jet_mass76_106");

        h_m_50_76   = get_m(x_avg_50_76  , x_avg_40_50, x_avg_106_170, h_mll_over_ptsub_50_76  , h_mll_over_ptsub_40_50, h_mll_over_ptsub_106_170, "m_50_76"  );
        h_m_106_170 = get_m(x_avg_106_170, x_avg_50_76, x_avg_170_350, h_mll_over_ptsub_106_170, h_mll_over_ptsub_50_76, h_mll_over_ptsub_170_350, "m_106_170");
        h_mll_over_ptsub_76_106 = cubic_hermite_spline_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_mll_over_ptsub_50_76, h_mll_over_ptsub_106_170, h_m_50_76, h_m_106_170, "mll_over_ptsub_inc0jet_mass76_106");

        h_m_50_76   = get_m(x_avg_50_76  , x_avg_40_50, x_avg_106_170, h_rapidity_50_76  , h_rapidity_40_50, h_rapidity_106_170, "m_50_76"  );
        h_m_106_170 = get_m(x_avg_106_170, x_avg_50_76, x_avg_170_350, h_rapidity_106_170, h_rapidity_50_76, h_rapidity_170_350, "m_106_170");
        h_rapidity_76_106 = cubic_hermite_spline_interpolation(x_avg_76_106, x_avg_50_76, x_avg_106_170, h_rapidity_50_76, h_rapidity_106_170, h_m_50_76, h_m_106_170, "rapidity_inc0jet_mass76_106");
    }
    // Scale density back to simple histo
    h_elPt_leading_76_106   ->Scale((106.0-76.0)); 
    h_elPt_subleading_76_106->Scale((106.0-76.0)); 
    h_pt_76_106             ->Scale((106.0-76.0)); 
    h_mll_over_ptsub_76_106 ->Scale((106.0-76.0)); 
    h_rapidity_76_106       ->Scale((106.0-76.0));

    // Preparing inclusive pT and rapidity histos
    h_elPt_leading_40_3000->Add(h_elPt_leading_76_106_read, -1);
    h_elPt_leading_40_3000->Add(h_elPt_leading_76_106);
    h_elPt_leading->Add(h_elPt_leading_76_106_read, -1);
    h_elPt_leading->Add(h_elPt_leading_76_106);
    h_elPt_subleading_40_3000->Add(h_elPt_subleading_76_106_read, -1);
    h_elPt_subleading_40_3000->Add(h_elPt_subleading_76_106);
    h_elPt_subleading->Add(h_elPt_subleading_76_106_read, -1);
    h_elPt_subleading->Add(h_elPt_subleading_76_106);
    h_pt_40_3000->Add(h_pt_76_106_read, -1);
    h_pt_40_3000->Add(h_pt_76_106);
    h_pt->Add(h_pt_76_106_read, -1);
    h_pt->Add(h_pt_76_106);
    h_mll_over_ptsub_40_3000->Add(h_mll_over_ptsub_76_106_read, -1);
    h_mll_over_ptsub_40_3000->Add(h_mll_over_ptsub_76_106);
    h_mll_over_ptsub->Add(h_mll_over_ptsub_76_106_read, -1);
    h_mll_over_ptsub->Add(h_mll_over_ptsub_76_106);
    h_rapidity_40_3000->Add(h_rapidity_76_106_read, -1);
    h_rapidity_40_3000->Add(h_rapidity_76_106);
    h_rapidity->Add(h_rapidity_76_106_read, -1);
    h_rapidity->Add(h_rapidity_76_106);

    // Making the output file
    TFile *f_out = new TFile(TString("dyjets-FakesInt")+TString(alt?"_alt":"")+TString(".root"), "RECREATE");
    TVectorD job_info_average(2);
    job_info_average[0] = -1;
    job_info_average[1] = 1;
    job_info_average.Write("_job_info_average");

    TIter nextKey(f_in->GetListOfKeys());
    TKey* key = nullptr;
    while ((key = dynamic_cast<TKey*>(nextKey())))
    {
        // Saving interpolated plots
        if (strcmp(key->GetName(), "elPt_leading_inc0jet") == 0) {
            f_out->WriteTObject(h_elPt_leading, h_elPt_leading->GetName());
            h_elPt_leading->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "elPt_leading_inc0jet_mass40_3000") == 0) {
            f_out->WriteTObject(h_elPt_leading_40_3000, h_elPt_leading_40_3000->GetName());
            h_elPt_leading_40_3000->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "elPt_leading_inc0jet_mass76_106") == 0) {
            f_out->WriteTObject(h_elPt_leading_76_106, h_elPt_leading_76_106->GetName());
            h_elPt_leading_76_106->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "elPt_subleading_inc0jet") == 0) {
            f_out->WriteTObject(h_elPt_subleading, h_elPt_subleading->GetName());
            h_elPt_subleading->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "elPt_subleading_inc0jet_mass40_3000") == 0) {
            f_out->WriteTObject(h_elPt_subleading_40_3000, h_elPt_subleading_40_3000->GetName());
            h_elPt_subleading_40_3000->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "elPt_subleading_inc0jet_mass76_106") == 0) {
            f_out->WriteTObject(h_elPt_subleading_76_106, h_elPt_subleading_76_106->GetName());
            h_elPt_subleading_76_106->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "pt_inc0jet") == 0) {
            f_out->WriteTObject(h_pt, h_pt->GetName());
            h_pt->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "pt_inc0jet_mass40_3000") == 0) {
            f_out->WriteTObject(h_pt_40_3000, h_pt_40_3000->GetName());
            h_pt_40_3000->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "pt_inc0jet_mass76_106") == 0) {
            f_out->WriteTObject(h_pt_76_106, h_pt_76_106->GetName());
            h_pt_76_106->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "mll_over_ptsub_inc0jet") == 0) {
            f_out->WriteTObject(h_mll_over_ptsub, h_mll_over_ptsub->GetName());
            h_mll_over_ptsub->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "mll_over_ptsub_inc0jet_mass40_3000") == 0) {
            f_out->WriteTObject(h_mll_over_ptsub_40_3000, h_mll_over_ptsub_40_3000->GetName());
            h_mll_over_ptsub_40_3000->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "mll_over_ptsub_inc0jet_mass76_106") == 0) {
            f_out->WriteTObject(h_mll_over_ptsub_76_106, h_mll_over_ptsub_76_106->GetName());
            h_mll_over_ptsub_76_106->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "rapidity_inc0jet") == 0) {
            f_out->WriteTObject(h_rapidity, h_rapidity->GetName());
            h_rapidity->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "rapidity_inc0jet_mass40_3000") == 0) {
            f_out->WriteTObject(h_rapidity_40_3000, h_rapidity_40_3000->GetName());
            h_rapidity_40_3000->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "rapidity_inc0jet_mass76_106") == 0) {
            f_out->WriteTObject(h_rapidity_76_106, h_rapidity_76_106->GetName());
            h_rapidity_76_106->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "mass_wide_range_inc0jet") == 0) {
            f_out->WriteTObject(h_mass, h_mass->GetName());
            h_mass->SetDirectory(0);
            continue;
        }
        if (strcmp(key->GetName(), "_job_info_average") == 0) {
            // f_out->WriteTObject(key, "_job_info_average");
            continue;
        }

        // Read the object from the input file
        TObject* obj = key->ReadObj();

        // Write the object to the output file
        f_out->WriteTObject(obj, obj->GetName());

        delete obj;
    }
    f_in->Close();
    f_out->Close();
    cout << "Output written in dyjets-FakesInt" << (alt?"_alt":"") << ".root" << endl;

    // Drawing
    TCanvas *c_elPt_leading = new TCanvas("fakes_elPt_leading_inc0jet_interp", "fakes_elPt_leading_inc0jet_interp", 800, 800);
    h_elPt_leading->SetStats(0);
    h_elPt_leading->SetFillColor(805);
    h_elPt_leading->SetLineColor(1);
    h_elPt_leading->SetMinimum(0);
    h_elPt_leading->Draw("BAR");
    h_elPt_leading->Draw("same e");
    c_elPt_leading->SetLogx();
    c_elPt_leading->SetGridx();
    c_elPt_leading->SetGridy();
    h_elPt_leading->GetXaxis()->SetRangeUser(2,1000);
    h_elPt_leading->GetXaxis()->SetNoExponent();
    h_elPt_leading->GetXaxis()->SetMoreLogLabels();
    cout << "Interpolated inclusive elPt_leading integral: " << h_elPt_leading->Integral() << endl;

    TCanvas *c_elPt_leading_76_106 = new TCanvas("fakes_elPt_leading_inc0jet_mass76_106_interp", "fakes_elPt_leading_inc0jet_mass76_106_interp", 800, 800);
    h_elPt_leading_76_106->SetStats(0);
    h_elPt_leading_76_106->SetFillColor(805);
    h_elPt_leading_76_106->SetLineColor(1);
    h_elPt_leading_76_106->SetMinimum(0);
    h_elPt_leading_76_106->Draw("BAR");
    h_elPt_leading_76_106->Draw("same e");
    c_elPt_leading_76_106->SetLogx();
    c_elPt_leading_76_106->SetGridx();
    c_elPt_leading_76_106->SetGridy();
    h_elPt_leading_76_106->GetXaxis()->SetRangeUser(2,1000);
    h_elPt_leading_76_106->GetXaxis()->SetNoExponent();
    h_elPt_leading_76_106->GetXaxis()->SetMoreLogLabels();
    cout << "Interpolated elPt_leading_76_106 integral: " << h_elPt_leading_76_106->Integral() << endl;

    TCanvas *c_elPt_subleading = new TCanvas("fakes_elPt_subleading_inc0jet_interp", "fakes_elPt_subleadinginc0jet_interp", 800, 800);
    h_elPt_subleading->SetStats(0);
    h_elPt_subleading->SetFillColor(805);
    h_elPt_subleading->SetLineColor(1);
    h_elPt_subleading->SetMinimum(0);
    h_elPt_subleading->Draw("BAR");
    h_elPt_subleading->Draw("same e");
    c_elPt_subleading->SetLogx();
    c_elPt_subleading->SetGridx();
    c_elPt_subleading->SetGridy();
    h_elPt_subleading->GetXaxis()->SetRangeUser(2,1000);
    h_elPt_subleading->GetXaxis()->SetNoExponent();
    h_elPt_subleading->GetXaxis()->SetMoreLogLabels();
    cout << "Interpolated inclusive elPt_subleading integral: " << h_elPt_subleading->Integral() << endl;

    TCanvas *c_elPt_subleading_76_106 = new TCanvas("fakes_elPt_subleading_inc0jet_mass76_106_interp", "fakes_elPt_subleading_inc0jet_mass76_106_interp", 800, 800);
    h_elPt_subleading_76_106->SetStats(0);
    h_elPt_subleading_76_106->SetFillColor(805);
    h_elPt_subleading_76_106->SetLineColor(1);
    h_elPt_subleading_76_106->SetMinimum(0);
    h_elPt_subleading_76_106->Draw("BAR");
    h_elPt_subleading_76_106->Draw("same e");
    c_elPt_subleading_76_106->SetLogx();
    c_elPt_subleading_76_106->SetGridx();
    c_elPt_subleading_76_106->SetGridy();
    h_elPt_subleading_76_106->GetXaxis()->SetRangeUser(2,1000);
    h_elPt_subleading_76_106->GetXaxis()->SetNoExponent();
    h_elPt_subleading_76_106->GetXaxis()->SetMoreLogLabels();
    cout << "Interpolated subleading_76_106 integral: " << h_elPt_subleading_76_106->Integral() << endl;

    TCanvas *c_pt = new TCanvas("fakes_pt_inc0jet_interp", "fakes_pt_inc0jet_interp", 800, 800);
    h_pt->SetStats(0);
    h_pt->SetFillColor(805);
    h_pt->SetLineColor(1);
    h_pt->SetMinimum(0);
    h_pt->Draw("BAR");
    h_pt->Draw("same e");
    c_pt->SetLogx();
    c_pt->SetGridx();
    c_pt->SetGridy();
    h_pt->GetXaxis()->SetRangeUser(2,1000);
    h_pt->GetXaxis()->SetNoExponent();
    h_pt->GetXaxis()->SetMoreLogLabels();
    cout << "Interpolated inclusive pT integral: " << h_pt->Integral() << endl;

    TCanvas *c_pt_76_106 = new TCanvas("fakes_pt_inc0jet_mass76_106_interp", "fakes_pt_inc0jet_mass76_106_interp", 800, 800);
    h_pt_76_106->SetStats(0);
    h_pt_76_106->SetFillColor(805);
    h_pt_76_106->SetLineColor(1);
    h_pt_76_106->SetMinimum(0);
    h_pt_76_106->Draw("BAR");
    h_pt_76_106->Draw("same e");
    c_pt_76_106->SetLogx();
    c_pt_76_106->SetGridx();
    c_pt_76_106->SetGridy();
    h_pt_76_106->GetXaxis()->SetRangeUser(2,1000);
    h_pt_76_106->GetXaxis()->SetNoExponent();
    h_pt_76_106->GetXaxis()->SetMoreLogLabels();
    cout << "Interpolated pt_76_106 integral: " << h_pt_76_106->Integral() << endl;

    TCanvas *c_rapidity = new TCanvas("fakes_rapidity_inc0jet_interp", "fakes_rapidity_inc0jet_interp", 800, 800);
    h_rapidity->SetStats(0);
    h_rapidity->SetFillColor(805);
    h_rapidity->SetLineColor(1);
    h_rapidity->SetMinimum(0);
    h_rapidity->Draw("BAR");
    h_rapidity->Draw("same e");
    c_rapidity->SetGridx();
    c_rapidity->SetGridy();
    cout << "Interpolated inclusive rapidity integral: " << h_rapidity->Integral() << endl;

    TCanvas *c_rapidity_76_106 = new TCanvas("fakes_rapidity_inc0jet_mass76_106_interp", "fakes_rapidity_inc0jet_mass76_106_interp", 800, 800);
    h_rapidity_76_106->SetStats(0);
    h_rapidity_76_106->SetFillColor(805);
    h_rapidity_76_106->SetLineColor(1);
    h_rapidity_76_106->SetMinimum(0);
    h_rapidity_76_106->Draw("BAR");
    h_rapidity_76_106->Draw("same e");
    c_rapidity_76_106->SetGridx();
    c_rapidity_76_106->SetGridy();
    cout << "Interpolated rapidity_76_106 integral: " << h_rapidity_76_106->Integral() << endl;

    TCanvas *c_mass = new TCanvas("fakes_mass_wide_range_inc0jet_interp", "fakes_mass_wide_range_inc0jet_interp", 800, 800);
    h_mass->SetStats(0);
    h_mass->SetFillColor(805);
    h_mass->SetLineColor(1);
    h_mass->SetMinimum(0);
    h_mass->Draw("BAR");
    h_mass->Draw("same e");
    c_mass->SetLogx();
    c_mass->SetGridx();
    c_mass->SetGridy();
    h_mass->GetXaxis()->SetNoExponent();
    h_mass->GetXaxis()->SetMoreLogLabels();
    cout << "Mass integral in the interpolated region: " << h_mass->Integral(h_mass->FindBin(76.01), h_mass->FindBin(105.9)) << endl;
    cout << "Mass integral full range: " << h_mass->Integral() << endl;

    TCanvas *c_mass_test = new TCanvas("TEST_mass_interp", "TEST_mass_interp", 800, 800);
    h_mass_test->SetStats(0);
    h_mass_test->SetFillColor(805);
    h_mass_test->SetLineColor(1);
    h_mass_test->SetMinimum(0);
    h_mass_test->Draw("BAR");
    h_mass_test->Draw("same e");
    TF1* f_test = new TF1("f_test", "([3]-[2])/([1]-[0])*x + ([2]-([3]-[2])/([1]-[0])*[0])", 56, 170);
    f_test->SetParameter(0, x_avg_50_76);
    f_test->SetParameter(1, x_avg_106_170);
    f_test->SetParameter(2, y_avg_50_76);
    f_test->SetParameter(3, y_avg_106_170);
    f_test->Draw("same");
    c_mass_test->SetLogx();
    c_mass_test->SetGridx();
    c_mass_test->SetGridy();
    h_mass_test->GetXaxis()->SetNoExponent();
    h_mass_test->GetXaxis()->SetMoreLogLabels();

    c_elPt_leading->SaveAs(TString("fakes_elPt_leading_inc0jet_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_elPt_leading_76_106->SaveAs(TString("fakes_elPt_leading_inc0jet_mass76_106_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_elPt_subleading->SaveAs(TString("fakes_elPt_subleading_inc0jet_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_elPt_subleading_76_106->SaveAs(TString("fakes_elPt_subleading_inc0jet_mass76_106_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_pt->SaveAs(TString("fakes_pt_inc0jet_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_pt_76_106->SaveAs(TString("fakes_pt_inc0jet_mass76_106_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_rapidity->SaveAs(TString("fakes_rapidity_inc0jet_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_rapidity_76_106->SaveAs(TString("fakes_rapidity_inc0jet_mass76_106_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_mass->SaveAs(TString("fakes_mass_wide_range_inc0jet_interp")+TString(alt?"_alt":"")+TString(".png"));
    c_mass_test->SaveAs(TString("TEST_mass_interp")+TString(alt?"_alt":"")+TString(".png"));
}

TH1D* make_density(TH1D* h)
{
    TH1D *h_out = (TH1D*)(h->Clone(TString(h->GetName())+TString("_density")));
    for (int i=1; i<=h->GetNbinsX(); i++)
    {
        h_out->SetBinContent(i, h_out->GetBinContent(i)/h_out->GetBinWidth(i));
    }
    return h_out;
}

TH1D* make_normal_from_density(TH1D* h)
{
    TH1D *h_out = (TH1D*)(h->Clone(TString(h->GetName()).ReplaceAll("_density", "")));
    for (int i=1; i<=h->GetNbinsX(); i++)
    {
        h_out->SetBinContent(i, h_out->GetBinContent(i)*h_out->GetBinWidth(i));
    }
    return h_out;
}

double linear_interpolation(double x, double x0, double x1, double y0, double y1)
{
    return (y0 - y1) * (x1 - x) / (x1 - x0) + y1;
}

double get_m(double x, double x0, double x1, double y, double y0, double y1)
{
    return ((y1-y)/(x1-x) + (y-y0)/(x-x0)) / 2;
}

double cubic_hermite_spline_interpolation(double x, double x0, double x1, double y0, double y1, double m0, double m1)
{
    double t = (x - x0) / (x1 - x0);
    double t2 = t * t;
    double t3 = t2 * t;
    double h00 = (2 * t3 - 3 * t2 + 1);
    double h10 = (t3 - 2 * t2 + t);
    double h01 = (-2 * t3 + 3 * t2);
    double h11 = (t3 - t2);
    return h00 * y0 + h10 * (x1 - x0) * m0 + h01 * y1 + h11 * (x1 - x0) * m1;
}

TH1D* linear_interpolation(double x, double x0, double x1, TH1D* y0, TH1D* y1, TString name)
{
    // y = (y0-y1)*(x1-x)/(x1-x0) + y1
    TH1D *y = (TH1D*)(y0->Clone(name)); // y0
    y->Add(y1, -1); // -y1
    y->Scale((x1 - x) / (x1 - x0)); // *(x1-x)/(x1-x0)
    y->Add(y1); // +y1
    return y;
}

TH1D* get_m(double x, double x0, double x1, TH1D* y, TH1D* y0, TH1D* y1, TString name)
{
    // m = ((y1-y)/(x1-x) + (y-y0)/(x-x0)) / 2
    TH1D *m = (TH1D*)(y1->Clone(name)); // y1
    m->Add(y, -1); // -y
    m->Scale(1/(x1-x)); // /(x1-x)
    TH1D *temp = (TH1D*)(y->Clone("temp")); // y
    temp->Add(y0, -1); // -y0
    temp->Scale(1/(x-x0)); // /(x-x0)
    m->Add(temp); //  +(y-y0)/(x-x0)
    m->Scale(1/2); // /2

    return m;
}

TH1D* cubic_hermite_spline_interpolation(double x, double x0, double x1, TH1D* y0, TH1D* y1, TH1D* m0, TH1D* m1, TString name)
{
    double t = (x - x0) / (x1 - x0);
    double t2 = t * t;
    double t3 = t2 * t;
    double h00 = (2 * t3 - 3 * t2 + 1);
    double h10 = (t3 - 2 * t2 + t);
    double h01 = (-2 * t3 + 3 * t2);
    double h11 = (t3 - t2);
    // y = h00*y0 + h10*(x1-x0)*m0 + h01*y1 + h11*(x1-x0)*m1
    TH1D *y = (TH1D*)(y0->Clone(name)); // y0
    y->Scale(h00); // *h00
    y->Add(m0, h10*(x1-x0)); // +h10*(x1-x0)*m0
    y->Add(y1, h01); // + h01*y1
    y->Add(m1, h11*(x1-x0)); // +h11*(x1-x0)*m1
    return y;
}
