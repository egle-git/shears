// -- class TTXSec
// -- usage
// TTXSec ttXSec;
// ttXSec.GetXSec(); // -- central cross section for TT->2l2nu
// // -- get the uncertainty
// Double_t absUnc_scaleVar_up = ttXSec.GetUnc("absUnc_scaleVar_up"); 
// Double_t relUnc_scaleVar_up = ttXSec.GetUnc("relUnc_scaleVar_up"); 

// // -- get the value for h->Scale() to change the defualt distribution for the systematic variation
// Double_t histScale_scaleVar_up = ttXSec.GetHistScale("scaleVar_up");
// TH1D* h_TT_scaleVar_up = (TH1D*)h_TT->Clone();
// h_TT_scaleVar_up->Scale( histScale_scaleVar_up );

class TTXSec {
public:
  TTXSec() { Init(); }

  Double_t GetXSec() const { return xSec_; }

  // -- value for h->Scale() to change the cross section for each variation
  // -- type: scaleVar_up, scaleVar_down
  Double_t GetHistScale(TString type) {
    auto iter = map_histScale.find(type);
    if( iter == map_histScale.end() )
      throw std::invalid_argument("[TTXSec::GetHistScale] type = " + type + " is not supported");

    return iter->second;
  }

  // -- type: absUnc_(uncTag) or relUnc_(uncTag)
  // -- uncTag: scaleVar_up, scaleVar_down
  Double_t GetUnc(TString type) const { 
    auto iter = map_unc_.find(type);
    if( iter == map_unc_.end() )
      throw std::invalid_argument("[TTXSec::GetUnc] type = " + type + " is not supported");

    return iter->second;
  }

private:
  Double_t xSec_;
  std::map<TString, Double_t> map_unc_;
  std::map<TString, Double_t> map_histScale;

  void Init() {
    xSec_ = 88.51; // -- TT -> 2l2nu

    // -- https://twiki.cern.ch/twiki/bin/view/LHCPhysics/TtbarNNLO
    Double_t xSec_inc = 833.9; // -- TT inclusive cross section
    Double_t absUnc_scaleVar_xSec_inc_up = 20.5; // -- scale unc. up variation
    Double_t absUnc_scaleVar_xSec_inc_dn = 30.0; // -- scale unc. down variation

    RegisterUnc_usingIncXSecInfo(xSec_inc, absUnc_scaleVar_xSec_inc_up, "scaleVar_up",   "up");
    RegisterUnc_usingIncXSecInfo(xSec_inc, absUnc_scaleVar_xSec_inc_dn, "scaleVar_down", "down");


    // -- relative uncertainty: same for inclusive & exclusive cross section
    // Double_t relUnc_scaleVar_up = absUnc_scaleVar_xSec_inc_up / xSec_inc;
    // Double_t relUnc_scaleVar_dn = absUnc_scaleVar_xSec_inc_dn / xSec_inc;

    // map_unc_.insert( std::make_pair("relUnc_scaleVar_up",   relUnc_scaleVar_up) );
    // map_unc_.insert( std::make_pair("relUnc_scaleVar_down", relUnc_scaleVar_dn) );

    // map_unc_.insert( std::make_pair("absUnc_scaleVar_up",   xSec_*relUnc_scaleVar_up) );
    // map_unc_.insert( std::make_pair("absUnc_scaleVar_down", xSec_*relUnc_scaleVar_dn) );

    // // -- scale used for h->Scale() to change the tt distribution for the systematic variation
    // Double_t scale_scaleVar_up = 1.0 + relUnc_scaleVar_up;
    // Double_t scale_scaleVar_dn = 1.0 - relUnc_scaleVar_dn;

    // map_histScale.insert( std::make_pair("scaleVar_up",   scale_scaleVar_up) );
    // map_histScale.insert( std::make_pair("scaleVar_down", scale_scaleVar_dn) );
  }

  // -- varType: up, down, both
  // ----> used for histScale
  void RegisterUnc_usingIncXSecInfo(Double_t xSec_inc, Double_t absUnc, TString uncType, TString varType) {
    map_unc_.insert( std::make_pair("absUnc_"+uncType, absUnc) );

    // -- relative uncertainty: same for inclusive & exclusive cross section
    Double_t relUnc = absUnc / xSec_inc;
    map_unc_.insert( std::make_pair("relUnc_"+uncType, relUnc) );

    if( varType == "up" )
      map_histScale.insert( std::make_pair(uncType, 1.0 + relUnc) ); // -- 1 + relUnc
    else if( varType == "down" )
      map_histScale.insert( std::make_pair(uncType, 1.0 - relUnc) ); // -- 1 - relUnc
    else if( varType == "both" ) { // -- same both direction with "up" and "down" tags
      map_histScale.insert( std::make_pair(uncType+"_up",   1.0 + relUnc) );
      map_histScale.insert( std::make_pair(uncType+"_down", 1.0 - relUnc) );
    }
    else
      throw std::invalid_argument("[TTXSec::RegisterUnc_usingIncXSecInfo] varType = " + varType + " is not supported");
  }

};
