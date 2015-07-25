{
    string srcdir = "Sources/";
    vector<string> sources;
    sources.push_back("getFilesAndHistograms");
    sources.push_back("functions");
    sources.push_back("writeFastPlotsTex");
    sources.push_back("unfoldingFunctions");
    sources.push_back("FinalUnfoldAndCombination");

    //--- Load shared libraries ---
    unsigned int nSources = sources.size();
    for (unsigned int i(0); i < nSources; i++){
        cout << "Compiling " << srcdir + sources[i] << ".cc" << endl;
        gROOT->ProcessLine(string(".L " + srcdir + sources[i] + ".cc++").c_str());
    }

    FinalUnfoldAndCombination(0, 1);
    //  FinalUnfoldAndCombination(21, 40);
    //  FinalUnfoldAndCombination(41, 62);
    //  FinalUnfoldAndCombination(41, 62);

}
