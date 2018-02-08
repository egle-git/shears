#ifndef CATALOG_H
#define CATALOG_H

#include <memory>
#include <string>

class TChain;

class catalog
{
    std::shared_ptr<TChain> _event_chain;
    std::shared_ptr<TChain> _bonzai_header_chain;
    std::shared_ptr<TChain> _bit_fields_chain;

    double _lumi, _xsec;

public:
    explicit catalog(const std::string &filename,
                     const std::string &bonzaiDir,
                     int maxFiles = -1);
    virtual ~catalog();

    std::shared_ptr<TChain> event_chain() { return _event_chain; }
    std::shared_ptr<TChain> bonzai_header_chain() { return _bonzai_header_chain; }
    std::shared_ptr<TChain> bit_fields_chain() { return _bit_fields_chain; }

    double lumi() const { return _lumi; }
    double xsec() const { return _xsec; }
};

#endif // CATALOG_H
