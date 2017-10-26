void splithist(double ratio=0.2){
    TVirtualPad* pmain = TVirtualPad::Pad();
    
    double h = 1. - pmain->GetTopMargin() - pmain->GetBottomMargin();
    
    double xlow = 0.; //gStyle->GetPadLeftMargin();
    double xhigh = 1.; // - gStyle->GetPadRightMargin();
    
    double ytop = 1.; //- gStyle->GetPadTopMargin();
    double ybot = 0.; //gStyle->GetPadBottomMargin();
    double ymid = pmain->GetBottomMargin() + ratio * h; //ybot + ratio * (ytop-ybot);
    
    
    double yp1bot = ymid;
    double yp2top = ymid;
    
    TPad* p1 = new TPad(TString(pmain->GetName()) + "_1", pmain->GetTitle(),
                        xlow, yp1bot, xhigh, ytop);
    p1->SetNumber(1);
    TPad* p2 = new TPad(TString(pmain->GetName()) + "_2", pmain->GetTitle(),
                        xlow, ybot, xhigh, yp2top);
    p2->SetNumber(2);
    p1->SetFillStyle(4000);
    p2->SetFillStyle(4000);
    
    double p1h = ytop - yp1bot;
    double p2h = yp2top - ybot;
    
    
    /*
     p1->SetTopMargin(pmain->GetTopMargin()/p1h);
     p1->SetBottomMargin((ymid-yp1bot)/p1h);
     p1->SetLeftMargin(pmain->GetLeftMargin());
     p1->SetRightMargin(pmain->GetRightMargin());
     
     p2->SetTopMargin((ymid-yp2top)/p2h);
     p2->SetBottomMargin(pmain->GetBottomMargin()/p2h);
     p2->SetLeftMargin(pmain->GetLeftMargin());
     p2->SetRightMargin(pmain->GetRightMargin());
     */
    p1->SetTopMargin(0.11);
    p1->SetBottomMargin(0.);
    p1->SetRightMargin(0.02);
    
    p2->SetTopMargin(0.);
    p2->SetBottomMargin(0.3);
    p2->SetRightMargin(0.02);
    
    
    
    p2->Draw();
    p1->Draw();
    pmain->Modified();
    
    p1->cd();
}
void fixsplithist(TH1* htop, TH1* hbot){
    TVirtualPad* pmain = TVirtualPad::Pad()->GetCanvas();
    if(!pmain) return;
    TVirtualPad* p1 = pmain->cd(1);
    TVirtualPad* p2 = pmain->cd(2);
    
    if(!p1 || !p2) return;
    
    double scale = p1->GetHNDC() / p2->GetHNDC();
    
    double s = htop->GetYaxis()->GetLabelSize() * scale;
    double ss = htop->GetYaxis()->GetLabelSize();
    hbot->GetYaxis()->SetLabelSize(s);
    htop->GetYaxis()->SetLabelSize(ss);
    s = htop->GetYaxis()->GetTitleSize() * scale;
    hbot->GetYaxis()->SetTitleSize(s);
    
    hbot->GetYaxis()->SetTitleOffset(0.5);
    htop->GetYaxis()->SetTitleOffset(0.5);
    
    s = htop->GetYaxis()->GetLabelOffset() * scale;
    
    s = htop->GetXaxis()->GetLabelSize() * scale;
    hbot->GetXaxis()->SetLabelSize(s);
    htop->GetXaxis()->SetLabelSize(0.);
    s = htop->GetXaxis()->GetTitleSize() * scale;
    hbot->GetXaxis()->SetTitleSize(s);
    
    
    
    s = htop->GetXaxis()->GetLabelOffset() * scale;
    hbot->GetXaxis()->SetLabelOffset(s);
    
    
    hbot->GetYaxis()->SetNdivisions(5);
}
void colorIt(TH1D *hMidA, int kCyan ){
	hMidA->SetMarkerColor(kCyan);
	hMidA->SetLineColor(kCyan);
	hMidA->SetMarkerStyle(20);//24
    hMidA->SetMarkerSize(0.7);
	hMidA->SetFillColor(kCyan);
	hMidA->SetFillStyle(0);
    hMidA->GetXaxis()->SetLabelSize(0.05);
	hMidA->GetXaxis()->SetTitleSize(0.06);
    hMidA->GetYaxis()->SetTitleSize(0.05);
	hMidA->GetYaxis()->SetTitleOffset(0.9);
	hMidA->GetYaxis()->SetLabelSize(0.05);
	hMidA->SetTitleFont(42, "XYZ");
    hMidA->SetLabelFont(42, "XYZ");
}
void colorBG(TH1D *hMidBG, int kBG ){
    hMidBG->SetLineWidth(0.0);
    hMidBG->SetMarkerColor(kBG);
    hMidBG->SetFillColor(kBG);
    hMidBG->SetLineColor(kBG);
}

void      macro_fit_pull( )
{
   // gStyle->SetOptStat(kFALSE);
    gStyle->SetOptTitle(kFALSE);
    gROOT->SetBatch(kTRUE);
    
    // TH1::SetDefaultSumw2();
    //  TH2::SetDefaultSumw2();
	
    
    char name[1000];
    
    for(int find=0;find<7;find++){

    sprintf(name,"histos_note_1605_md/pull_all_inc_ljet_pt_y_fb___ismu_1_ZPTweightcorr_0_%d.root",find);

    TFile *_file0 = TFile::Open(name);
    TCanvas *c1 = (TCanvas*)_file0->Get("c6");
    TH1D *pull = (TH1D*)c1->GetPrimitive("h_pull_all");

        TLegend *leg10 = new TLegend(0.25,0.0953719,0.680511,0.25385);
        leg10->SetBorderSize(0);
        leg10->SetFillStyle(0);
        leg10->SetFillColor(kWhite);
        leg10->SetTextSize(0.028);
        double range[]={0,0.5,1,1.5,2,2.5,3.2,4.7};
        sprintf(name,"jet Y range [%.1lf,%.1lf]",range[find],range[find+1]);
        leg10->AddEntry(pull, name,"");
        //   leg10->AddEntry(hhist_diff_DE_RunD, "DE 2012D/MinBias 2012C","l");
        // leg10->AddEntry(hhist_diff_DE_jet, "Jet 2012A/MinBias 2012C","l");

        TCanvas *c2 = new TCanvas("c2","", 700, 900);
        c2->cd();
        pull->Draw();
        pull->SetXTitle("pull");
        pull->SetYTitle("Entries");
        pull->GetYaxis()->SetTitleOffset(0.9);
        pull->Fit("gaus");
        leg10->Draw("same");
        sprintf(name,"pull_all_inc_ljet_pt_y_fb___ismu_1_ZPTweightcorr_0_%d.pdf",find);
        c2->Print(name);
        
    }
    
    TFile *_file1 = TFile::Open("histos_note_1605_md/pull_all_inc_ljet_pt_y_fb___ismu_1_ZPTweightcorr_0.root");
    TCanvas *c11 = (TCanvas*)_file1->Get("c66");
    TH1D *pull = (TH1D*)c11->GetPrimitive("h_pull_all_all");
    
    
    TLegend *leg10 = new TLegend(0.25,0.0953719,0.680511,0.25385);
    leg10->SetBorderSize(0);
    leg10->SetFillStyle(0);
    leg10->SetFillColor(kWhite);
    leg10->SetTextSize(0.028);
    double range[]={0,0.5,1,1.5,2,2.5,3.2,4.7};
    leg10->AddEntry(pull, "all Y ranges combined","");

    TCanvas *c22 = new TCanvas("c2","", 700, 900);
    c22->cd();
    pull->Draw();
    pull->SetXTitle("pull");
    pull->SetYTitle("Entries");
    pull->GetYaxis()->SetTitleOffset(0.9);
    pull->Fit("gaus");
    leg10->Draw("same");
    c22->Print("pull_all_inc_ljet_pt_y_fb___ismu_1_ZPTweightcorr_0.pdf");

    
}


