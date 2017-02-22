#include <iostream>
#include <limits>
#include "TVirtualPad.h"
#include "TList.h"
#include "TH1.h"
#include "TClass.h"
#include "math.h"

void fixYscale(double linfact, double logfact, double logMaxRange){

  if(logfact <= 0) logfact = linfact;
  
  if(gPad==0) return;

  double ymax = -std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();
  double yposmin = std::numeric_limits<double>::max();
  std::vector<long> hs;

  TListIter it(gPad->GetListOfPrimitives());
  while(it.Next()){
    TObject* obj = *it;
    if(!TClass(obj->ClassName()).InheritsFrom("TH1")) continue;
    TH1* h = dynamic_cast<TH1*>(obj);
    if(h==0) continue;
    for(int ibin = 1; ibin <= h->GetXaxis()->GetNbins(); ++ibin){
      double  y = h->GetBinContent(ibin);
      if(y > ymax) ymax = y;
      if(y < ymin) ymin = y;
      if(y > 0 && y < yposmin) yposmin = y;
      hs.push_back((long)h);
    }
  }
  
  std::cout << "ymin = " << ymin
	    << "  ymax = " << ymax;
  
  if(gPad->GetLogy()){
    if(ymin <= 0) ymin = yposmin; //yposmin / 10.;
    double a = pow(ymax/ymin, logfact);
    ymax = ymin * a;
    if(logMaxRange > 0 && ymax/ymin > logMaxRange){
      ymin = ymax / logMaxRange;
    }
    if(ymax < ymin/10) ymax = 15*ymin;
  } else{
    if(ymin>0) ymin = 0;
    ymax *= linfact;
    ymin *= linfact;
  }
  
  std::cout << "  ->  axis min = " << ymin
            << "  axis max = " << ymax << std::endl;
  
  //for(unsigned i = 0; i < hs.size(); ++i){
  unsigned i = 0;
  if(hs.size() > i){
    TH1* h = (TH1*) hs[i];
    h->GetYaxis()->SetRangeUser(ymin, ymax);
  }
  gPad->Paint();
}
