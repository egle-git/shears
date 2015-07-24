{
    string srcdir = "Sources/";
    vector<string> sources;
    sources.push_back("getFilesAndHistograms");
    sources.push_back("functions");
    //sources.push_back("plottingFunctions");
    sources.push_back("writeFastPlotsTex");
    sources.push_back("unfoldingFunctions");
    sources.push_back("makeZJetsPlots");
    sources.push_back("runPlotting3RatiosCommon");

    //--- Load shared libraries ---
    unsigned int nSources = sources.size();
    for (unsigned int i(0); i < nSources; i++){
        cout << "Compiling " << srcdir + sources[i] << ".cc" << endl;
        gROOT->ProcessLine(string(".L " + srcdir + sources[i] + ".cc+").c_str());
    }

    bool doMuon = 1;
    runPlotting3RatiosCommon(doMuon, 0, 55);
    //runPlotting3RatiosCommon(doMuon, 0, 45);
    //runPlotting3RatiosCommon(doMuon, 46, 60);

}
