#include "simpleShiftUncEstimator.h"

namespace physics
{

void simpleShiftUncEstimator::add_systHistName(std::string sysHistName) {
  _vec_systHistName.push_back( sysHistName );
}

void simpleShiftUncEstimator::add_systInfo(std::string type, double weight_nom, double weight_plus, double weight_minus) {
  SystInfo info;
  info.type = type;
  info.weight_nom = weight_nom;
  info.weight_plus = weight_plus;
  info.weight_minus = weight_minus;
  _vec_systInfo.push_back( info );
}

simpleShiftUncEstimator::simpleShiftUncEstimator(physics::weights &weights, 
                                                 util::histo_set &histo_set, 
                                                 util::histo_set2D &histo_set2D):
_weights(weights),
_histo_set(histo_set),
_histo_set2D(histo_set2D) {}

void simpleShiftUncEstimator::fill_allSystHist( const std::string &name,
                                                const util::matched<std::string> &tags,
                                                const util::matched<double> &value) {

  if( !match_systHistName(name, tags) ) return;

  for(SystInfo systInfo : _vec_systInfo ) {
    fill_systHist(systInfo, "plus",  name, tags, value);
    fill_systHist(systInfo, "minus", name, tags, value);
  }
}

bool simpleShiftUncEstimator::match_systHistName(const std::string &name, const util::matched<std::string> &tags) {

  // if tag doesn't exist for both gen and reco: reject
  if( !tags.gen && !tags.rec ) return false;

  std::string histName_rec, histName_gen;
  if( tags.rec ) histName_rec = name + "_" + *tags.rec;
  if( tags.gen ) histName_gen = name + "_" + *tags.gen;

  // return true when either gen or reco name is matched to the given syst histogram name
  bool isFound = false;
  for( auto systHistName : _vec_systHistName ) {
    if( systHistName == histName_gen || systHistName == histName_rec) {
      isFound = true;
      break;
    }
  }

  return isFound;
}

// from boson_jets_analyzer::fill_unfolded
// this should be updated as well if boson_jets_analyzer::fill_unfolded is updated
void simpleShiftUncEstimator::fill_systHist( const SystInfo systInfo,
                                             std::string shift, // "plus", "minus"
                                             const std::string &name,
                                             const util::matched<std::string> &tags,
                                             const util::matched<double> &value)
{
  // additional tag for systematic variation histogram
  std::string tag_syst = "_" + systInfo.type + "_" + shift;

  // global weight for systematic variation
  double weight_global = 0;
  if( shift == "plus" ) 
    weight_global = _weights.global_weight() * (systInfo.weight_plus / systInfo.weight_nom);
  else if( shift == "minus" )
    weight_global = _weights.global_weight() * (systInfo.weight_minus / systInfo.weight_nom);
  else {
    throw std::runtime_error("shift type should be plus or minus!");
    return;
  }

  // Fill 1D distributions
  if( tags.rec && value.rec )
    _histo_set.fill(name, *tags.rec + tag_syst, *value.rec, weight_global);
  
  if( tags.gen && value.gen )
    _histo_set.fill(name, *tags.gen + tag_syst + "-gen", *value.gen, _weights.gen_weight());

  // Fill response matrix
  if( tags.rec && tags.gen && value.rec && value.gen && tags.rec == tags.gen ) {
    // both gen and reco exist and have the same tag
    // First fill both gen and reco with the global_weight
    _histo_set2D.fill(name,
                     *tags.rec + tag_syst + "-matrix",
                     *value.rec,
                     *value.gen, weight_global);
    // Now fill again subtracting the global weight from gen weight
    // And placing the event in the reco underflow bin 
    // as explained in the TUnfold manual
    // https://www.desy.de/~sschmitt/TUnfold/tunfold_manual_v17.9.pdf, page 10
    _histo_set2D.fill(name,
                     *tags.rec + tag_syst + "-matrix",
                     -10000.0,// underflow bin
                     *value.gen, _weights.gen_weight()-weight_global);
  }
  else{
      if( tags.rec && value.rec ) {
          // there is no gen corresponding to the reco event
          // reco event gets global_weight
          _histo_set2D.fill(name,
                           *tags.rec + tag_syst + "-matrix",
                           *value.rec,
                           -10000.0, weight_global);
      }
      if( tags.gen && value.gen ) {
          // there is no reco event
          // gen event gets gen_weight
          _histo_set2D.fill(name,
                           *tags.gen + tag_syst + "-matrix",
                           -10000.0,
                           *value.gen, _weights.gen_weight());
    }
  }
}

void simpleShiftUncEstimator::clear_systInfo() {
  _vec_systInfo.clear();
}

} // namespace physics