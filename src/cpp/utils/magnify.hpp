/**
 * @file        utils/magnify.hpp
 * @brief       Magnification lens
 * @author      Justin Scott
 * @date        2026-06-22
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <utils/poly.hpp>

namespace utils
{

///////////////////////////////////////////////////////////////////////////////
/// Represents a magification lens
///
/// @tparam     floating        True if the polygon math is done in floating point
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating = false>
class Magnifier
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  poly        Lens polygon
    /// @param[in]  scale       Lens magnifcation
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Magnifier (BasicPolygonView<floating> poly, double scale) :
        m_lens (static_cast<BasicMultiPolygon<floating>> (poly)),
        m_scale (scale)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Magnify a polygon
    ///
    /// @param[in]  poly        Polygon to magnify
    ///
    /// @return     Magnified polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    BasicMultiPolygon<floating> magnify (BasicPolygonView<floating> poly) const
        {
        using Point = typename BasicMultiPolygon<floating>::Point;

        // Find the intersection with our magnfying lens
        BasicMultiPolygon<floating>  intersection    = m_lens & poly;
        Point                   center          = m_lens.boundingRect ().center ();

        // Magnify the intersection
        intersection.translate (-center.x (), -center.y ());
        intersection.expand (m_scale);
        intersection.translate (center);

        // clip it to the lens area and join it back to the original polygon
        intersection = intersection & m_lens;
        intersection = intersection | (poly - m_lens);

        return intersection;
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Get the lens polygon
    ///
    /// @return     Lens polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const auto& getPoly () const { return m_lens; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the lens magnicification factor
    ///
    /// @return     Magnification
    ///
    ///////////////////////////////////////////////////////////////////////////////
    double getScale () const { return m_scale; }

private:
    BasicMultiPolygon<floating> m_lens; ///< Magnification lens polygon
    double                      m_scale;///< Magnification scale factor
    };

}
