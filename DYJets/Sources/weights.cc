#include "weights.h"

#include <iostream>

#include <TVectorD.h>

namespace physics
{

weights::weights(util::job::info &info)
    : EvtWeights(info.reader, "EvtWeights"),
      _ismc(info.catalog.primary_events() <= 0), // FIXME Improve ?
      _primary_events_total(info.catalog.primary_events()),
      _events_in_chain(info.reader.GetEntries(true)),
      _xsec(info.catalog.xsec()),
      _lumi(info.catalog.lumi())
{
    if (info.chains.isbaobab()) {
        throw std::runtime_error("This analysis cannot be run on baobabs.");
    }

    TTreeReader reader(info.chains.bonzai_header().get());
    TTreeReaderArray<double> EvtWeightSums(reader, "EvtWeightSums");
    TTreeReaderArray<double> InEvtWeightSums(reader, "InEvtWeightSums");
    TTreeReaderValue<int> InEvtCount(reader, "InEvtCount");

    while (reader.Next()) {
        if (EvtWeightSums.GetSize() > 0) {
            _weights_sum_in_chain += EvtWeightSums[0];
            _weights_sum += InEvtWeightSums[0];
        }
        _primary_events_in_chain += *InEvtCount;
    }
}

void weights::process_event()
{
    if (weights_count() > 0) {
        _global_weight = weight_at(0);
        _processed_weights_sum += weight_at(0);
    } else {
        _global_weight = 1;
    }
    _processed_events++;
}

void weights::write(util::histo_set *histos)
{
    double fraction_processed = 1;
    if (ismc()) {
        fraction_processed *= _processed_weights_sum / _weights_sum_in_chain;
    } else {
        // Fraction of all events that was present in chain
        fraction_processed *= double(_primary_events_in_chain) / _primary_events_total;
        // Fraction of events in the chain that was processed
        fraction_processed *= double(_processed_events) / _events_in_chain;
    }

    // Declare and fill summed info histogram
    histos->declare("_job_info", "Job information", 4, 0, 4);
    util::histo_set::histogram_type &job_info = histos->get("_job_info");

    job_info.GetXaxis()->SetBinLabel(1, "fraction_processed"); // For data
    job_info.SetBinContent(1, fraction_processed);

    job_info.GetXaxis()->SetBinLabel(2, "weights_sum"); // For MC
    job_info.SetBinContent(2, _weights_sum);

    // Declare and fill info vector
    // We use TVectorD because hadd won't sum them, as it should be for sample lumi and xsec
    // (TH1::kSetAverage is broken in hadd)
    TVectorD job_info_average(2);
    job_info_average[0] = _lumi; // For data
    job_info_average[1] = _xsec; // For MC
    job_info_average.Write("_job_info_average");

    if (isdata()) {
        util::logging::info << "Processed fraction of sample: " << fraction_processed << std::endl;
    }
}
} // namespace physics
