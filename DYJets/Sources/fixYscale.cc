#include <iostream>
#include <limits>
#include "TVirtualPad.h"
#include "TList.h"
#include "TH1.h"
#include "TClass.h"
#include "TLegend.h"
#include "math.h"

void fixYscale(double linfact, double logfact, double logMaxRange){

  if(logfact <= 0) logfact = linfact;
  
  if(gPad==0) return;

  double ymax = -std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();
  double yposmin = std::numeric_limits<double>::max();
  std::vector<TH1*> hs;
  
  TLegend* tl = 0;

  //std::cout << "Pad name: " << gPad->GetName() << "\n";
  TListIter it(gPad->GetListOfPrimitives());
  while(it.Next()){
    TObject* obj = *it;
    if(TClass(obj->ClassName()).InheritsFrom("TLegend")){
      std::cout << "Found a legend with object name " << obj->GetName() << "\n";
      tl = dynamic_cast<TLegend*>(obj);
    }
    if(!TClass(obj->ClassName()).InheritsFrom("TH1")) continue;
    TH1* h = dynamic_cast<TH1*>(obj);
    if(h==0) continue;
    for(int ibin = 1; ibin <= h->GetXaxis()->GetNbins(); ++ibin){
      double  y = h->GetBinContent(ibin);
      double yerr = h->GetBinError(ibin); 
      if(y + yerr > ymax) ymax = y + yerr;
      if(y - yerr < ymin) ymin = y - yerr;
      if(y > 0 && y < yposmin) yposmin = y;
      hs.push_back(h);
    }
  }
  
  //std::cout << "ymin = " << ymin
  //	    << "  ymax = " << ymax;
  
  if(gPad->GetLogy()){
    if(ymin <= 0) ymin = yposmin; //yposmin / 10.;
    ymin *= pow(ymax/ymin, -(logfact-1)/2.);
    double a = pow(ymax/ymin, logfact);
    ymax = ymin * a;
    if(logMaxRange > 0 && ymax/ymin > logMaxRange){
      ymin = ymax / logMaxRange;
    }
    if(ymax < ymin/10) ymax = 15*ymin;
  } else{
    bool add_ymin_margin = true;
    if(ymin>0) {
      ymin = 0.001*(ymax-ymin); //starts at slightly higher than 0 to prevent the display of the label
      add_ymin_margin = false; 
    }
    //    ymax *= linfact;
    //ymin *= linfact;
    ymax += (linfact - 1) * (ymax-ymin);
    if(add_ymin_margin) ymin -= (linfact - 1) * (ymax-ymin);
  }
  
  std::cout << "  ->  axis min = " << ymin
	  << "  axis max = " << ymax << std::endl;

  //Prevent overlapping with the legend by zooming the y-axis.
  //Only legend place on top of the plot is handled.
  bool istopleg =  false;
  if(tl && (1 - std::max(tl->GetY1NDC(), tl->GetY2NDC())) < std::min(tl->GetY1NDC(), tl->GetY2NDC())){
    istopleg = true;
  }
  if(tl) std::cout << "Top legend: " << (istopleg ? "yes" : "no") << "\n";

  unsigned i = 0;
  if(hs.size() > i){
    TH1* h = (TH1*) hs[i];
    h->GetYaxis()->SetRangeUser(ymin, ymax);
  }
  gPad->Paint();

    
  if(istopleg){
    double ymax_in_leg_area = - std::numeric_limits<double>::max();
    for(auto h: hs){
//      std::cout << ">>> tl->GetX1() = "  <<  tl->GetX1() << "\n";
//      std::cout << ">>> tl->GetX2() = "  <<  tl->GetX2() << "\n";
//      std::cout << ">>> tl->GetX1NDC() = "  <<  tl->GetX1NDC() << "\n";
//      std::cout << ">>> tl->GetX2NDC() = "  <<  tl->GetX2NDC() << "\n";
      int lb = h->GetXaxis()->FindBin(tl->GetX1());
      int ub = h->GetXaxis()->FindBin(tl->GetX2());
      //      std::cout << ">>> lb, ub = "  <<  lb << ", " << ub << "\n";
      if(lb > ub) std::swap(lb, ub);
      for(int ibin = lb ; ibin <= ub; ++ibin){ 
	double y = h->GetBinContent(ibin) + h->GetBinError(ibin); 
	//	std::cout << "ibin = " << ibin << " y = " << h->GetBinContent(ibin) + h->GetBinError(ibin) << "\n";
	if(y > ymax_in_leg_area) ymax_in_leg_area = y; 
      }
    }
    double y_leg_min = std::min(tl->GetY1(), tl->GetY2());
    
    //    std::cout << "ymax_in_leg_area = " << ymax_in_leg_area 
    //	      << ", y_leg_min = " << y_leg_min << "\n";
    
    //std::cout << "---> " << gPad->GetLogy() << "\n";

    if(y_leg_min < ymax_in_leg_area){
      //double oldymax = ymax;
      //if(gPad->GetLogy()){
      //ymax = ymin * pow(ymax/ymin, log(ymax_in_leg_area/ymin)/log(y_leg_min/ymin));
      //} else{
	ymax =  ymin +  ( ymax - ymin) * (ymax_in_leg_area - ymin) / (y_leg_min -  ymin);
	//}
      //double margin = 0.03;
      //ymax += margin * (ymax - ymin);
      //std::cout << "ymax: " << oldymax << " -> " << ymax << "\n";
    }
  }
  //for(unsigned i = 0; i < hs.size(); ++i){
  i = 0;
  if(hs.size() > i){
    TH1* h = (TH1*) hs[i];
    h->GetYaxis()->SetRangeUser(ymin, ymax);
  }
  gPad->Paint();
}
