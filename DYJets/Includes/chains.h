#ifndef CHAINS_H
#define CHAINS_H

#include <memory>
#include <string>
#include <vector>

class TChain;

class chains
{
    std::shared_ptr<TChain> _events;
    std::shared_ptr<TChain> _bonzai_header;
    std::shared_ptr<TChain> _bit_fields;

  public:
    /// \brief Constructor.
    explicit chains(const std::vector<std::string> &files);

    /// \brief Returns a \c TChain pointing to event data.
    std::shared_ptr<TChain> events() { return _events; }

    /// \brief Returns a \c TChain pointing to bonzai header data.
    std::shared_ptr<TChain> bonzai_header() { return _bonzai_header; }

    /// \brief Returns a \c TChain pointing to bit fields header data.
    std::shared_ptr<TChain> bit_fields() { return _bit_fields; }

    /// \brief Returns true if the bonzai header was not found.
    bool isbaobab() const { return _bonzai_header == nullptr; }

    /// \brief Returns true if the bonzai header was found.
    bool isbonzai() const { return _bonzai_header != nullptr; }
};

#endif // CHAINS_H
