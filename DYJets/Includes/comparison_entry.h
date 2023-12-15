#ifndef COMPARISON_ENTRY
#define COMPARISON_ENTRY

#include <memory>
#include <set>
#include <string>

#include <THStack.h>
#include <TList.h>

#include "mc_group.h"
#include "options.h"

class TAxis;
class TFile;
class TH1;
class TLegend;

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

    /// \brief Adds the current entry to the given legend.
    virtual void add_to_legend(TLegend &legend,
                               const std::string &plotname,
                               double lumi,
                               std::string option) = 0;

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

    /// \brief Returns the integral of the histogram
    virtual double integral(const std::string &name, double lumi) = 0;

    /**
     * \brief Returns a pointer to the \c TAxis corresponding to the horizontal
     *        axis.
     *
     * In case the axis doesn't make sense, \c nullptr shall be returned. The
     * pointer may not be invalidated until \ref reset_drawing_state is called.
     */
    virtual TAxis *get_x_axis(const std::string &/* name */, double /* lumi */)
    { return nullptr; }

    virtual TAxis *get_y_axis(const std::string &/* name */, double /* lumi */)
    { return nullptr; }

    /// \brief Discards any internal state bound to the last histogram.
    virtual void reset_drawing_state() = 0;
};

/// \brief Describes a Monte-Carlo entry in reco-level comparison plots.
class mc_comparison_entry : public comparison_entry
{
    std::vector<mc_group> _groups;
    std::unique_ptr<THStack> _stack;
    std::vector<std::string> _legend;

  public:
    /**
     * \brief Constructor.
     * \param opt The \c options object (to read MC grouping from)
     * \param analyzer_name The analyzer name
     * \param input_dir The location of histogram files
     */
    explicit mc_comparison_entry(const util::options &opt,
                                 const std::string &analyzer_name,
                                 const std::string &input_dir,
                                 bool keep_signal = true,
                                 bool keep_background = true,
                                 const std::string &excluded_groups = "");

    /// \brief Destructor.
    virtual ~mc_comparison_entry() = default;

    virtual void add_histograms(std::set<std::string> &histos) override;
    virtual void add_to_legend(TLegend &legend, const std::string &plotname, double lumi, std::string option="f") override;
    virtual void draw(const std::string &name, double lumi, bool same = false) override;
    virtual std::unique_ptr<TH1> get(const std::string &name, double lumi) override;
    virtual double integral(const std::string &name, double lumi) override;
    virtual TAxis *get_x_axis(const std::string &name, double lumi) override;
    virtual TAxis *get_y_axis(const std::string &name, double lumi) override;
    virtual void reset_drawing_state() override;

    /// \brief Retrieves a histogram for a specified process from the group.
    std::unique_ptr<TH1> get_single_proc(const std::string &name, const std::string &legend) {
        std::unique_ptr<TH1> res = nullptr;
        for (data::mc_group &group : _groups) {
            if (legend == group.legend())
                res = group.get(name);
        }
        if (res == nullptr)
            util::logging::warn << "No histogram for " << legend << " found." << std::endl;
        return res;
    }

    /// \brief Returns the vector of mc groups
    std::vector<mc_group> groups() { return _groups; }

    /// \brief Returns the legend of mc group
    std::vector<std::string> legend() { return _legend; }

    /// \brief Retrieves a list of all the histograms in the stack.
    TList* get_all_hists(const std::string &name, double lumi) {
        if (_stack == nullptr) {
            create_stack(name, lumi);
            if (_stack == nullptr) {
                return nullptr;
            }
        }
        return _stack->GetHists();
    }

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
     * \param analyzer_name The analyzer name
     * \param sample The sample to describe
     * \param input_dir The location of histogram files
     */
    explicit data_comparison_entry(const std::string &analyzer_name,
                                   const sample &sample,
                                   const std::string &input_dir);

    /// \brief Destructor.
    virtual ~data_comparison_entry() = default;

    virtual void add_histograms(std::set<std::string> &histos) override;
    virtual void add_to_legend(TLegend &legend, const std::string &plotname, double lumi, std::string option="lp") override;
    virtual void draw(const std::string &name, double lumi, bool same = false) override;
    virtual std::unique_ptr<TH1> get(const std::string &name, double lumi) override;
    virtual double integral(const std::string &name, double lumi) override;
    virtual TAxis *get_x_axis(const std::string &name, double lumi) override;
    virtual TAxis *get_y_axis(const std::string &name, double lumi) override;
    virtual void reset_drawing_state() override;

    /// \brief Returns the sum of event weights (for MC)
    double wsum() const { return _wsum; }

    /// \brief Returns the cross section (for MC)
    double xsec() const { return _xsec; }

    /// \brief Returns the processed luminosity (for data)
    double lumi() const { return _lumi * _frac; }

  private:
    void create_histo(const std::string &name, double lumi);
};
} // namespace data

#endif // COMPARISON_ENTRY
