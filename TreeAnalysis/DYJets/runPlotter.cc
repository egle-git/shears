{
    string srcdir = "Sources/";

    vector<string> sources;
    sources.push_back("getFilesAndHistograms");
    sources.push_back("functions");
    sources.push_back("Plotter");
    sources.push_back("PlotterPAS");

    //--- Load shared libraries ---
    unsigned int nSources = sources.size();
    for (unsigned int i(0); i < nSources; i++){
        cout << "Compiling " << srcdir + sources[i] << ".cc" << endl;
        gROOT->ProcessLine(string(".L " + srcdir + sources[i] + ".cc+").c_str());
    }
    ostringstream fileBeingProcessed; fileBeingProcessed << __FILE__;
    cout << " Run plotter:  " << fileBeingProcessed.str()  << endl;
    if (fileBeingProcessed.str().find("DYJets") != string::npos) {
        //Plotter("Electrons", 30);
        PlotterPAS("Electrons");
        //Plotter("Muons", 30);
        PlotterPAS("Muons");

        //getStatistics("Electrons", 30);
        //getStatistics("Muons", 30);
        //getStatistics("Muons", 30,0,0,1,0,1);
        //getStatistics("Electrons", 30,0,0,1,0,1);

    }    
    else if (fileBeingProcessed.str().find("TTbar") != string::npos) {
        Plotter("SMuE",30,0,0,0,0,-1);
        //Plotter("SMuE",30,0,1,0,0,-1);
        //
        //PlotterPAS("SMuE",30,0, 0, 0, 30);
        //PlotterPAS("SMuE",30,0, 1, 0, 30 );

        //getStatistics("SMuE", 30);
        //getStatistics("SMuE", 30, 0, 0, 1, 0, 1);

        //getStatistics("SMuE", 30, 0, 0, 1, 0, 0, 0, 30);
        //getStatistics("SMuE", 30, 0, 0, 1, 0, 1, 0, 30);

    }
    else if (fileBeingProcessed.str().find("WJets") != string::npos) {
        //PlotterPAS("Muon",30,0);
        //PlotterPAS("Muon",30,1);
        //PlotterPAS("Muon",30,2);
        //PlotterPAS("Muon",30,3);
        getStatistics("SMu", 30);
    }
}
