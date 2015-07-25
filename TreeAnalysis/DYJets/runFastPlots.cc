{
    string srcdir = "Sources/";

    vector<string> sources;
    sources.push_back("getFilesAndHistograms");
    sources.push_back("functions");
    sources.push_back("writeFastPlotsTex");
    sources.push_back("unfoldingFunctions");
    sources.push_back("FastPlots");

    //--- Load shaared libraries ---
    unsigned int nSources = sources.size();
    for (unsigned int i(0); i < nSources; i++){
        cout << "Compiling " << srcdir + sources[i] << ".cc" << endl;
        gROOT->ProcessLine(string(".L " + srcdir + sources[i] + ".cc+").c_str());
    }

    //FastPlots("DMu", "FirstJetPt_Zinc1jet");
    //FastPlots("DMu", "FirstJetPt_2_Zinc1jet");
    FastPlots("DE", "SecondJetPt_2_Zinc2jet");
    FastPlots("DE", "JetsHT_2_Zinc1jet");
    FastPlots("DE", "JetsHT_2_Zinc2jet");
    //FastPlots("DE");
    //FastPlots("FirstJetPt_1_Zinc1jet");
    //FastPlots("FirstJetPt_2_Zinc1jet");
    //FastPlots("FirstJetPt_3_Zinc1jet");
    //FastPlots("FirstJetPt_4_Zinc1jet");

}
