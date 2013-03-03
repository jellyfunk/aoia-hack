#ifndef DIMENSIONSUMMARYFORMATTER_H
#define DIMENSIONSUMMARYFORMATTER_H

#include "DataModel.h"

namespace aoia { namespace sv {

    /**
     * \brief
     * This class generates a HTML table with a toon summary for the supplied summary model.
     */
    class DimensionSummaryFormatter
    {
    public:
        DimensionSummaryFormatter(DataModelPtr model, std::tstring const& dimensionName);

        std::tstring toString() const;

    private:
        DataModelPtr m_model;
        std::tstring m_dimensionName;
    };

}}  // namespace

#endif // DIMENSIONSUMMARYFORMATTER_H
