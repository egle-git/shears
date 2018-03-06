#include "weights.h"

#include <iostream>

namespace physics
{

weights_analyzer::weights_analyzer(util::job::info &info)
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

void weights_analyzer::operator()()
{
    if (weights_count() > 0) {
        _global_weight = weight_at(0);
        _processed_weights_sum += weight_at(0);
    } else {
        _global_weight = 1;
    }
    _processed_events++;
}

void weights_analyzer::write()
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
    declare("_job_info", "Job information", 4, 0, 4);
    histogram_type &job_info = get("_job_info");

    job_info.GetXaxis()->SetBinLabel(1, "fraction_processed"); // For data
    job_info.SetBinContent(1, fraction_processed);

    job_info.GetXaxis()->SetBinLabel(2, "weights_sum"); // For MC
    job_info.SetBinContent(2, _weights_sum);

    // Declare and fill info histogram
    declare("_job_info_average", "Job information", 4, 0, 4);
    histogram_type &job_info_average = get("_job_info_average");
    job_info_average.SetBit(TH1::kIsAverage);

    job_info_average.GetXaxis()->SetBinLabel(1, "lumi"); // For data
    job_info_average.SetBinContent(1, _lumi);

    job_info_average.GetXaxis()->SetBinLabel(2, "xsec"); // For MC
    job_info_average.SetBinContent(2, _xsec);

    if (isdata()) {
        util::logging::info << "Processed fraction of sample: " << fraction_processed << std::endl;
    }
}
} // namespace physics
