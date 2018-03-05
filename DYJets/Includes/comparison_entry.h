#ifndef COMPARISON_ENTRY
#define COMPARISON_ENTRY

#include <memory>
#include <set>
#include <string>

#include "mc_group.h"
#include "options.h"

class TFile;
class TH1;
class THStack;

namespace data
{

class sample;

/// \brief Describes an entry in reco-level comparison plots.
class comparison_entry
{
  public:
    /// \brief Destructor.
    virtual ~comparison_entry() = default;

    /// \brief Adds the name of all available histograms to \c histos
    virtual void add_histograms(std::set<std::string> &histos) = 0;

    /**
     * \brief Draws the histogram with the given \c name on the current canvas.
     *
     * The histogram shall be normalized according to \c lumi.
     */
    virtual void draw(const std::string &name, double lumi, bool same = false) = 0;

    /**
     * \brief Returns a representation of the histogram with the given \c name suitable for making
     *        ratio plots.
     *
     * The returned histogram shall be normalized according to \c lumi.
     */
    virtual std::unique_ptr<TH1> get(const std::string &name, double lumi) = 0;

    /// \brief Discards any internal state bound to the last histogram.
    virtual void reset_drawing_state() = 0;
};

/// \brief Describes a Monte-Carlo entry in reco-level comparison plots.
class mc_comparison_entry : public comparison_entry
{
    std::vector<mc_group> _groups;
    std::unique_ptr<THStack> _stack;

  public:
    explicit mc_comparison_entry(const util::options &opt, const std::string &input_dir);

    /// \brief Destructor.
    virtual ~mc_comparison_entry() = default;

    virtual void add_histograms(std::set<std::string> &histos) override;
    virtual void draw(const std::string &name, double lumi, bool same = false) override;
    virtual std::unique_ptr<TH1> get(const std::string &name, double lumi) override;
    virtual void reset_drawing_state() override;

  private:
    void create_stack(const std::string &name, double lumi);
};

/// \brief Describes a data entry in reco-level comparison plots.
class data_comparison_entry : public comparison_entry
{
    std::shared_ptr<TFile> _file;
    std::shared_ptr<TH1> _histo;

    double _frac, _wsum, _lumi, _xsec;

  public:
    /**
     * \brief Constructor.
     */
    explicit data_comparison_entry(const std::string &analyzer_name,
                                   const sample &sample,
                                   const std::string &input_dir,
                                   bool required = true);

    /// \brief Destructor.
    virtual ~data_comparison_entry() = default;

    virtual void add_histograms(std::set<std::string> &histos) override;
    virtual void draw(const std::string &name, double lumi, bool same = false) override;
    virtual std::unique_ptr<TH1> get(const std::string &name, double lumi) override;
    virtual void reset_drawing_state() override;

    double wsum() const { return _wsum; }
    double xsec() const { return _xsec; }
    double lumi() const { return _lumi * _frac; }

  private:
    void create_histo(const std::string &name, double lumi);
};
} // namespace data

#endif // COMPARISON_ENTRY
