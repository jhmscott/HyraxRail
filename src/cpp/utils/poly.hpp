/**
 * @file        utils/poly.hpp
 * @brief       Complex polygon library. Polygon with holes
 * @author      Justin Scott
 * @date        2026-06-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <common.hpp>

#include <QPolygon>

#include <functional>
#include <vector>

namespace utils
{

// Kepp in sync with gpc_op
enum polyOp
    {
    POLY_DIFF,  ///< Subtract a polygon
    POLY_INT,   ///< Take the intersection of two polygon
    POLY_XOR,   ///< Exclusive or of two polygon
    POLY_UNION  ///< Take the union of two polygon
    };


///////////////////////////////////////////////////////////////////////////////
/// Complex polygon class. Polygon with holes
///
/// @tparam     floating        True to use floating point for coordinates
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating = false>
class ComplexPolygon
    {
public:

    using Polygon   = std::conditional_t<floating, QPolygonF, QPolygon>;
    using Point     = std::conditional_t<floating, QPointF, QPoint>;
    using Coordinate= std::conditional_t<floating, qreal, int>;
    using RingList  = std::vector<Polygon>;

    Polygon     exteriorRing;   ///< Main, exterior polygon ring
    RingList    interiorRings;  ///< Interior rings/holes

    // Default constructor
    ComplexPolygon () = default;

    // Default copying
    ComplexPolygon (const ComplexPolygon&) = default;
    ComplexPolygon& operator= (const ComplexPolygon&) = default;

    // Default moving
    ComplexPolygon (ComplexPolygon&&) = default;
    ComplexPolygon& operator= (ComplexPolygon&&) = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit, copy constructor
    ///
    /// @param[in]  exterior        Main polygon ring
    /// @param[in]  interior        List of holes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit ComplexPolygon (const Polygon& exterior, const RingList& interior = {}) :
        exteriorRing (exterior),
        interiorRings (interior)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Translate this polygon
    ///
    /// @param[in]  offset      Ammount to offset polygon by
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translate (const Point& offset)
        {
        exteriorRing.translate (offset);
        std::for_each (interiorRings.begin (),
                       interiorRings.end (),
                       std::bind (&Polygon::translate,
                                   std::placeholders::_1,
                                   offset));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Translate this polygon
    ///
    /// @param[in]  x       X-offset
    /// @param[in]  x       Y-offset
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translate (Coordinate x, Coordinate y) { translate (Point{ x, y }); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a translated copy of this polygon
    ///
    /// @param[in]  offset      Ammount to offset polygon by
    ///
    /// @return     Translated polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]]
    ComplexPolygon translated (const QPoint& offset) const
        {
        Polygon     newExterior = exteriorRing.translated (offset);
        RingList    newInterior;

        newInterior.reserve (interiorRings.size ());

        std::transform (interiorRings.begin (),
                        interiorRings.end (),
                        std::back_inserter (newInterior),
                        std::bind (&Polygon::translated,
                                    std::placeholders::_1,
                                    offset));

        return ComplexPolygon{ newExterior, newInterior };
        }
    ///////////////////////////////////////////////////////////////////////////////
    /// Create a translated copy of this polygon
    ///
    /// @param[in]  x       X-offset
    /// @param[in]  x       Y-offset
    ///
    /// @return     Translated polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]]
    ComplexPolygon translated (Coordinate x, Coordinate y) const { return translated (Point{ x, y }); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if a point falls within this polygon
    ///
    /// @param[in]  pt      Point to check
    /// @param[in]  fill    Polygon fill rule
    ///
    /// @return     true if pt is within polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool containsPoint (const Point& pt, Qt::FillRule fill) const
        {
        return exteriorRing.containsPoint (pt, fill) &&
               std::none_of (interiorRings.begin (),
                             interiorRings.end (),
                             std::bind (&Polygon::containsPoint,
                                         std::placeholders::_1,
                                         pt,
                                         fill));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this polygon is empty
    ///
    /// @return     True if empty
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool empty () const
        {
        return exteriorRing.isEmpty() &&
              (interiorRings.empty () ||
               std::all_of (interiorRings.begin (),
                            interiorRings.end (),
                            std::mem_fn (&Polygon::isEmpty ())));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this polygon is not empty
    ///
    /// @return     True if not empty
    ///
    ///////////////////////////////////////////////////////////////////////////////
    operator bool () const { return not empty (); }
    };

// Floating point polygon
using ComplexPolygonF = ComplexPolygon<true>;

///////////////////////////////////////////////////////////////////////////////
/// Perform an operation on two polygon
///
/// @tparam     floating1       Polygon 1 floating point flag
/// @tparam     floating2       Polygon 2 floating point flag
///
/// @param[in]  pg1             Polygon 1
/// @param[in]  pg2             Polygon 2
/// @param[in]  op              Operation to perform
///
/// @return     Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating1, bool floating2>
ComplexPolygon<floating1 || floating2> operate (const ComplexPolygon<floating1>&    pg1,
                                                const ComplexPolygon<floating2>&    pg2,
                                                polyOp                              op);

///////////////////////////////////////////////////////////////////////////////
/// Subtract one polygon from another
///
/// @tparam     floating1       Polygon 1 floating point flag
/// @tparam     floating2       Polygon 2 floating point flag
///
/// @param[in]  pg1             Polygon 1
/// @param[in]  pg2             Polygon 2
///
/// @return     Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating1, bool floating2>
auto operator- (const ComplexPolygon<floating1>& pg1,
                const ComplexPolygon<floating2>& pg2)
    {
    return operate (pg1, pg2, POLY_DIFF);
    }

///////////////////////////////////////////////////////////////////////////////
/// Take the intersection of two polygons
///
/// @tparam     floating1       Polygon 1 floating point flag
/// @tparam     floating2       Polygon 2 floating point flag
///
/// @param[in]  pg1             Polygon 1
/// @param[in]  pg2             Polygon 2
///
/// @return     Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating1, bool floating2>
auto operator& (const ComplexPolygon<floating1>& pg1,
                const ComplexPolygon<floating2>& pg2)
    {
    return operate (pg1, pg2, POLY_INT);
    }

///////////////////////////////////////////////////////////////////////////////
/// Take the exclusive OR of two polygons
///
/// @tparam     floating1       Polygon 1 floating point flag
/// @tparam     floating2       Polygon 2 floating point flag
///
/// @param[in]  pg1             Polygon 1
/// @param[in]  pg2             Polygon 2
///
/// @return     Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating1, bool floating2>
auto operator^ (const ComplexPolygon<floating1>& pg1,
                const ComplexPolygon<floating2>& pg2)
    {
    return operate (pg1, pg2, POLY_XOR);
    }

///////////////////////////////////////////////////////////////////////////////
/// Take the union of two polygons
///
/// @tparam     floating1       Polygon 1 floating point flag
/// @tparam     floating2       Polygon 2 floating point flag
///
/// @param[in]  pg1             Polygon 1
/// @param[in]  pg2             Polygon 2
///
/// @return     Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating1, bool floating2>
auto operator| (const ComplexPolygon<floating1>& pg1,
                const ComplexPolygon<floating2>& pg2)
    {
    return operate (pg1, pg2, POLY_UNION);
    }

} // namespace utils