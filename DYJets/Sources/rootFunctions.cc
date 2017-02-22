#include "TLatex.h"
#include "TAxis.h"
#include "TH1.h"
#include "TVirtualPad.h"
#include "TLine.h"
#include "TCanvas.h"

void draw_axis_labels( TAxis* a){
   TLatex* txt_ = new TLatex();
   // draw axis labels
   //  float xlab_ = 65;
   txt_->SetTextFont(a->GetLabelFont());
   txt_->SetTextSize(a->GetLabelSize());
   txt_->SetTextAngle(0);


   
   TH1* h = (TH1*) a->GetParent();
   TAxis* ax = h->GetXaxis();
   TAxis* ay = h->GetYaxis();

   int iV = (a == ax) ? 1 : 0;   

   if( iV==1 ){
     txt_->SetTextAlign(23);
   } else {
     txt_->SetTextAlign(32);
   }

   TVirtualPad* pad = TVirtualPad::Pad();
   

   double ymin = pad->GetUymin();
   double ymax = pad->GetUymax();
   double xmin = pad->GetUxmin();//ax->GetBinLowEdge(ax->GetFirst());
   double xmax = pad->GetUxmax();//ax->GetBinUpEdge(ax->GetLast());
   double yoffset = (ax->GetLabelOffset() + 0.01)*(ymax-ymin);
   double xoffset = (ay->GetLabelOffset() + 0.01)*(xmax-xmin);
   
   //   TAxis* a = (iV == 1)? ax : ay;
   
   int imin = a->GetFirst();
   int imax = a->GetLast();
   
   double tickY0 = gPad->GetUymin();
   double tickY1 = tickY0  + 0.02*(gPad->GetUymax() - tickY0);
   TLine l;

   for(int ii=imin-1; ii<=imax; ++ii){
     double val = (ii==imin-1) ?
       a->GetBinLowEdge(imin)
       : a->GetBinUpEdge(ii);
     float x_, y_;
     if( iV==1 ){ //x-axis
       x_ = val;
       y_ = ymin - yoffset;
     } else {
       y_ = val;
       x_ = xmin - xoffset;
     }
     txt_->SetText(x_,y_,Form("%-2.0f",val));

     l.DrawLine(val, tickY0, val, tickY1);

     txt_->DrawClone();
   }
}

