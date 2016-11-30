#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <vector>

runMakeJECTable()
{
  string filename = "Spring16_25nsV8G_DATA/Spring16_25nsV8_DATA_Uncertainty_AK4PFchs.txt"; 
  std::ifstream infile(filename.c_str());
  cout << filename << endl;
  if (infile) cout << "OK" << endl;
  else cout << "Not OK" << endl;
  
  std::vector<std::string> lines;
  std::string line;
  while ( std::getline(infile, line) ) {
    if ( !line.empty() )
        lines.push_back(line);
  }
  
  for(int l0 = 0; l0<lines.size(); l0++){
    
    std::stringstream ss(lines[l0]);
    std::vector<std::string> words;
    std::string word = " ";
    while (ss >> word) {
      words.push_back(word);
    }
    
    int Bins = words.size()/3; 
    //cout<<"Number of Entries: "<<words.size()<<endl; 
    
    std::string etaLow, etaHigh, ptLow, ptHigh, eff, effErr; 
    etaLow = words[0]; 
    etaHigh = words[1]; 
    effErr = "0."; 
    
    for(int w0 = 1; w0<Bins-1; w0++){
    
      ptLow = words[w0*3]; 
      ptHigh = words[w0*3+3];
      eff = words[w0*3+1]; 
      cout<<etaLow<<" "<<etaHigh<<" "<<ptLow<<" "<<ptHigh<<" "<<eff<<" "<<effErr<<" "<<effErr<<endl; 
    }
  }
    
    
    
  
  
  
  
  
}
