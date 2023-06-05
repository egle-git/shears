#ifndef SIMPLE_SHIFT_UNC_ESTIMATOR_H
#define SIMPLE_SHIFT_UNC_ESTIMATOR_H

#include <string>
#include <vector>

#include "histo_set.h"
#include "histo_set2D.h"
#include "matched.h"
#include "weights.h"

#include "TUnfoldBinning.h"

namespace physics
{


// tool to produce the histograms with systematic variation w.r.t. nominal histogram
// usage
// 1) register the histogram name(s) to be varied according to the systematic uncertainty +-1sigma ("add_systHistName")
// 2) for each event, register the information about the systematic uncertianty - unc. name and weight values ("add_systInfo")
// 3) fill the histograms with the alternative weights for each histogram name & uncertainty source ("fill_allSystHist")
// 4) clear the systematic information at the end of the every event
// 5) after the loop, you will get the histograms filled with alternative weights in the output .root file

class simpleShiftUncEstimator {
public:

  simpleShiftUncEstimator(physics::weights &weights, util::histo_set &histo_set, util::histo_set2D &histo_set2D);

  // register TUnfoldBinning object (used in "fill_systHist")
  void set_TUnfoldBinning(TUnfoldBinning* trueBinning, TUnfoldBinning* recoBinning);

  // save era information
  void set_era(int era);

  // register histogram name to calculate the systematic variation
  // e.g. mass_wide_range_inc0jet
  void add_systHistName(std::string systHistName);

  // register the systematic variation information event by event
  // type: systematic uncertainty type e.g. pileup
  // weight_nom/plus/minus: event weight for nominal/+1sigma/-1sigma case
  void add_systInfo(std::string type, double weight_nom, double weight_plus, double weight_minus);

  // fill all histograms with systematic variation (for each histogram & systematic source)
  void fill_allSystHist( const std::string &name,
                         const util::matched<std::string> &tags,
                         const util::matched<double> &value);

  // clear the vector containing the systematic weight information
  // should be called at the end of the every event (to avoid mixture of the information between events)
  void clear_systInfo();

private:
  physics::weights& _weights;
  util::histo_set& _histo_set;
  util::histo_set2D& _histo_set2D;
  
  vector<std::string> _vec_systHistName;

  TUnfoldBinning* _trueBinning;
  TUnfoldBinning* _recoBinning;

  int _era;

  struct SystInfo {
    std::string type;
    double weight_nom;
    double weight_plus;
    double weight_minus;
  };

  std::vector<SystInfo> _vec_systInfo;

  // return true if the given histogram name is matched with the registered name
  // need this function to produce the histograms with alternative weights only for the registered histograms
  bool match_systHistName(const std::string &name, const util::matched<std::string> &tags);

  // fill the histograms with alternative weights
  // from boson_jets_analyzer::fill_unfolded
  // this should be updated as well if boson_jets_analyzer::fill_unfolded is updated
  void fill_systHist( const SystInfo systInfo,
                      std::string shift, // "plus", "minus"
                      const std::string &name,
                      const util::matched<std::string> &tags,
                      const util::matched<double> &value);

};

} // -- end of physics namespace

#endif // SIMPLE_SHIFT_UNC_ESTIMATOR_H