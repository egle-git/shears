#ifndef HISTO_SET_H
#define HISTO_SET_H

#include <map>
#include <stdexcept>

#include <TH1D.h>

#include "GenH1D.h"

namespace util
{

class histo_set
{
  public:
    using histogram_type = GenH1D;

  private:
    std::map<std::string, histogram_type> _models;
    std::map<std::pair<std::string, std::string>, histogram_type> _histograms;

  public:
    /// \brief Constructor.
    explicit histo_set() = default;

    /// \brief Destructor.
    virtual ~histo_set() = default;

    /**
     * \brief Declares a new histogram.
     *
     * Histograms have to be declared before being filled. Histogram names must be unique.
     *
     * \param name The name of the histogram, forwarded to the histogram constructor as first
     *             argument
     * \param args Further arguments to forward to the histogram constructor.
     */
    template <class... Args> void declare(const std::string &name, const Args &... args)
    {
        if (_models.count(name) > 0) {
            throw std::logic_error("Histogram declared twice: " + name);
        }
        _models.emplace(name, histogram_type(name.c_str(), args...));
    }

    /**
     * \brief Fills an histogram.
     *
     * Fills an histogram named \c name, previously \ref declare "declared". The \c tag can be used
     * to distinguish between different version of the same histogram (eg using different
     * selections). Other arguments are forwarded to the \c Fill function of the histogram.
     */
    template <class... Args>
    void fill(const std::string &name, const std::string &tag, const Args &... args)
    {
        get(name, tag).Fill(args...);
    }

    /**
     * \brief Writes all histograms in the current (\c ROOT) directory.
     *
     * \note Empty histograms are never written.
     */
    void write();

    /**
     * \brief Retrives a string that combines the given name and tag.
     * \returns \c name if \c tag is empty, \c name_tag otherwise.
     */
    static std::string combined_name(const std::string &name, const std::string &tag);

  private:
    histogram_type &get(const std::string &name, const std::string &tag);
};
} // namespace util

#endif // HISTO_SET_H
