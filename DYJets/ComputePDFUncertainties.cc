#include <iostream>
#include <sstream>
#include <vector>
#include <TH1.h>
#include <TGraphAsymmErrors.h>
#include <TFile.h>

using namespace std;

string fileName = "HistoFiles/DE_8TeV_DYJets_MIX_UNFOLDING_dR_5311_Inf3_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth_PDF_";
void getListOfHistos(vector<TH1D*>& histos, vector<TGraphAsymmErrors*>& graphs);

void ComputePDFUncertainties() {

    // get list of histos
    vector<TH1D*> histos;
    vector<TGraphAsymmErrors*> graphs; // graphs are actually not used but this is another possibility
    getListOfHistos(histos, graphs);
    //-------------------------------------------


    // build the pdf sets
    vector<pair<string, int> > pdfSets;
    //pdfSets.push_back(make_pair("NNPDF23_nlo_as_0118.LHgrid", 100));
    pdfSets.push_back(make_pair("MSTW2008nlo68cl.LHgrid", 40));
    pdfSets.push_back(make_pair("CT10nlo.LHgrid", 52));

    // run on every histograms
    for (int ii(0); ii < histos.size(); ++ii) {
        cout << ii << "  Histo name: " << histos[ii]->GetName() << endl;
        int nBins = histos[ii]->GetNbinsX();

        // run on every bin of each histogram
        for (int bin(0); bin <= nBins; ++bin) {
            //cout << "Bin number: " << bin << endl;

            double maxMeanPlusSigma(-999999999999);
            double minMeanMinusSigma(999999999999);

            // run on every pdf set for every bins of every histograms
            for (vector<pair<string, int> >::iterator ppdfSet = pdfSets.begin(); ppdfSet != pdfSets.end(); ++ppdfSet) {
                pair<string, int> pdfSet = (*ppdfSet); 
                //cout << "PDF set: " << pdfSet.first << endl;

                //-- Get mu_0: the value for the pdf-member 0 --
                TFile *pdfMembFile_0 = new TFile((fileName + pdfSet.first + "_0.root").c_str());
                TH1D *h_0 = (TH1D*) pdfMembFile_0->Get(histos[ii]->GetName());
                double mu_0 = h_0->GetBinContent(bin);
                pdfMembFile_0->Close();


                //------ Compute mean and sigma ------
                double mu(0);
                double tmpsigma2(0.);
                double tmpsigma2Bis(0.);

                // run on all the pdf member of the current pdf set
                for (int pdfMemb(1); pdfMemb <= pdfSet.second; ++pdfMemb) {
                    stringstream pdfMembFileName("");
                    pdfMembFileName << fileName << pdfSet.first << "_" << pdfMemb << ".root";
                    TFile *pdfMembFile = new TFile(pdfMembFileName.str().c_str());
                    TH1D *h = (TH1D*) pdfMembFile->Get(histos[ii]->GetName());
                    double content = h->GetBinContent(bin);
                    tmpsigma2 += content*content;
                    mu += content;
                    tmpsigma2Bis += (content - mu_0)*(content - mu_0);

                    pdfMembFile->Close();
                }
                // end of running over all members

                // compute the mean and sigma by dividing by the number of pdf members
                mu = mu*1./pdfSet.second;
                tmpsigma2 = tmpsigma2*1./pdfSet.second; 
                tmpsigma2Bis = tmpsigma2Bis*1./pdfSet.second; 

                //cout << "tmpsigma2: " << tmpsigma2 << endl;
                //cout << "tmpsigma2Bis: " << tmpsigma2Bis << endl;
                //cout << "mu: " << mu << endl;
                //cout << "mu_0: " << mu_0 << endl;
                //cout << "tmpsigma2 - mu*mu = " << tmpsigma2 - mu*mu << endl;
                double sigma2 = tmpsigma2 - mu*mu;
                double sigma = sqrt(sigma2);
                //cout << "sigma2: " << sigma2 << endl;
                //cout << "sigma: " << sigma << endl;

                maxMeanPlusSigma = max(maxMeanPlusSigma, mu + sigma);
                minMeanMinusSigma = min(minMeanMinusSigma, mu - sigma);
                //cout << "Tmp Max: " << maxMeanPlusSigma << endl;
                //cout << "Tmp Min: " << minMeanMinusSigma << endl;
            }
            // end of running over each pdf set

            //cout << "Max: " << maxMeanPlusSigma << endl;
            //cout << "Min: " << minMeanMinusSigma << endl;
            double belt = maxMeanPlusSigma - minMeanMinusSigma;
            double center = minMeanMinusSigma + 0.5 * belt;
            //graphs[ii]->SetPointEYhigh(bin, center + 0.5 * belt);
            histos[ii]->SetBinContent(bin, 1.);

            if (center > 0) histos[ii]->SetBinError(bin, (0.5 * belt)/center);
            else histos[ii]->SetBinError(bin, 0.);


        }
        // end of running on every bin

        string outputName = "PDFSyst_" + string(histos[ii]->GetName()) + ".root";
        TFile *outputFile =  new TFile(outputName.c_str(), "RECREATE");
        outputFile->cd();
        histos[ii]->Write();
        outputFile->Close();

    }
    // end of running on every histograms

}

void getListOfHistos(vector<TH1D*>& histos, vector<TGraphAsymmErrors*>& graphs) {

    // open a DY file just to get the histograms details such as the number of histos, 
    // their names, binning and so on...
    TFile *madgraph = new TFile("HistoFiles/DMu_8TeV_DYJets_MIX_UNFOLDING_dR_5311_Inf3_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth_PDF_NNPDF23_nlo_as_0118.LHgrid_0.root");
    unsigned short nHist = madgraph->GetListOfKeys()->GetEntries();

    // run on every histograms contained in this file, but do not consider any gen... histogram
    for (unsigned short i(0); i < nHist; i++) {
        string name = madgraph->GetListOfKeys()->At(i)->GetName();
        if (name.find("gen") != 0) continue;
        cout << name << endl;

        TH1D *tmp = (TH1D*) madgraph->Get(name.c_str());
        tmp->SetDirectory(0);
        tmp->Reset();

        // create a TGraphAsymmErrors with points at x = BinCenter and y = 1
        // and errors = 0.
        int bins = tmp->GetNbinsX();
        double *x = new double[bins];
        double *y = new double[bins];
        double *ex = new double[bins];
        double *ey = new double[bins];
        for (int j(0); j < bins; j++) {
            x[j] = tmp->GetXaxis()->GetBinCenter(j+1);
            y[j] = 1.;
            ex[j] = 0.;
            ey[j] = 0.;
        }
        TGraphAsymmErrors *gr = new TGraphAsymmErrors(bins, x, y, ex, ex, ey, ey);
        graphs.push_back(gr);
        histos.push_back(tmp);
        delete [] x;
        delete [] y;
        delete [] ex;
        delete [] ey;
    }
    madgraph->Close();

}
