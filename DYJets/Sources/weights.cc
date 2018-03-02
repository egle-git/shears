#include "weights.h"

namespace physics
{

weights_analyzer::weights_analyzer(util::job::info &info)
    : EvtWeights(info.reader, "EvtWeights"),
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
    TTreeReaderValue<int> InEvtCount(reader, "InEvtCount");

    while (reader.Next()) {
        if (EvtWeightSums.GetSize() > 0) {
            _weights_sum_in_chain += EvtWeightSums[0];
        }
        _primary_events_in_chain += *InEvtCount;
    }
}

void weights_analyzer::operator()()
{
    if (size() > 0) {
        _processed_weights_sum += at(0);
    }
    _processed_events++;
}

void weights_analyzer::write()
{
    // Fraction of events that was available in the chain
    double fraction_in_chain = double(_primary_events_in_chain) / _primary_events_total;

    // Fraction of events from the chain that was processed
    double fraction_of_chain;
    if (_weights_sum_in_chain != 0) {
        fraction_of_chain = _processed_weights_sum / _weights_sum_in_chain;
    } else {
        fraction_of_chain = double(_processed_events) / _events_in_chain;
    }

    // Fraction of events that was processed
    double fraction_processed = fraction_in_chain * fraction_of_chain;

    // Declare and fill info histogram
    declare("_job_info", "Job information", 4, 0, 4);
    histogram_type &job_info = get("_job_info");
    int bin = 1;

    job_info.GetXaxis()->SetBinLabel(bin, "fraction_processed");
    job_info.SetBinContent(bin, fraction_processed);

    bin++;

    job_info.GetXaxis()->SetBinLabel(bin, "lumi_processed");
    job_info.SetBinContent(bin, _lumi * fraction_processed);

    bin++;

    job_info.GetXaxis()->SetBinLabel(bin, "xsec_processed");
    job_info.SetBinContent(bin, _xsec * fraction_processed);

    bin++;

    job_info.GetXaxis()->SetBinLabel(bin, "weights_sum");
    job_info.SetBinContent(bin, _processed_weights_sum);

    using util::logging::info;
    info << "Available fraction of sample: " << fraction_in_chain << std::endl;
    info << "        ...of which was used: " << fraction_of_chain << std::endl;
    info << "Processed fraction of sample: " << fraction_processed << std::endl;
}
} // namespace physics
