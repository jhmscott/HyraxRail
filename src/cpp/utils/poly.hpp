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
#include <QSpan>

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

// Forward declarations
template<bool floating>
class MultiPolygon;

template<bool floating>
class PolygonView;

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
                       std::bind (std::mem_fn (&Polygon::translate),
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
                        std::bind (std::mem_fn (&Polygon::translated),
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
                             std::bind (std::mem_fn (&Polygon::containsPoint),
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
                            std::mem_fn (&Polygon::isEmpty)));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this polygon is not empty
    ///
    /// @return     True if not empty
    ///
    ///////////////////////////////////////////////////////////////////////////////
    operator bool () const { return not empty (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Subtract one polygon from another
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to subtract
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator- (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Subtract one polygon from another
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to subtract
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator- (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Subtract one polygon from another
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to subtract
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator- (const PolygonView<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the intersection of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to intersect
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator& (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the intersection of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to intersect
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator& (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the intersection of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to intersect
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator& (const PolygonView<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the exclusive or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to XOR with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator^ (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the exclusive or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to XOR with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator^ (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the exclusive or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to XOR with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto  operator^ (const PolygonView<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the union or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to unite with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto  operator| (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the union or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to unite with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator| (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the union or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to unite with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator| (const PolygonView<otherFloating>& other) const;
    };

// Floating point complex polygon
using ComplexPolygonF = ComplexPolygon<true>;


///////////////////////////////////////////////////////////////////////////////
/// Set of polygons with holes
///
/// @tparam     floating        True if coordinates are floating point
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating = false>
class MultiPolygon : public std::vector<ComplexPolygon<floating>>
    {
public:
    using SubPolygon    = ComplexPolygon<floating>;
    using Point         = typename SubPolygon::Point;

    // using std::vector<SubPolygon>::vector<SubPolygon>;

    ///////////////////////////////////////////////////////////////////////////////
    /// Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MultiPolygon () = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Allow implicit copies from Complex Polygons
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit MultiPolygon (const SubPolygon& poly)
        { push_back (poly); }

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
        return std::any_of (Base::begin (),
                            Base::end (),
                            std::bind (std::mem_fn (&SubPolygon::containsPoint),
                                       std::placeholders::_1,
                                       pt,
                                       fill));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Subtract one polygon from another
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to subtract
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator- (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Subtract one polygon from another
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to subtract
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator- (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Subtract one polygon from another
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to subtract
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator- (const PolygonView<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the intersection of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to intersect
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator& (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the intersection of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to intersect
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator& (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the intersection of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to intersect
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator& (const PolygonView<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the exclusive or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to XOR with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator^ (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the exclusive or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to XOR with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator^ (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the exclusive or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to XOR with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto  operator^ (const PolygonView<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the union or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to unite with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto  operator| (const ComplexPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the union or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to unite with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator| (const MultiPolygon<otherFloating>& other) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Take the union or of two polygons
    ///
    /// @tparam     otherFloating   Floating point flag for other polygon
    ///
    /// @param[in]  other       Polygon to unite with
    ///
    /// @return     Resulting polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<bool otherFloating>
    auto operator| (const PolygonView<otherFloating>& other) const;

private:
    using Base = std::vector<ComplexPolygon<floating>>;
    };

// Floating point multi-polygon
using MultiPolygonF = MultiPolygon<true>;

///////////////////////////////////////////////////////////////////////////////
/// Non-owning read only copy of a polygon. Meant to be used in place of a
/// const MultiPolygon<>& when you want the interface to accept ComplexPolygon<>s
/// as well.
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating = false>
class PolygonView : public QSpan<const ComplexPolygon<floating>>
    {
public:
    using Multi = typename MultiPolygon<floating>;
    using Sub   = typename Multi::SubPolygon;

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor. Creates a reference to a complex polygon
    ///
    /// @param[in]  poly        Complex polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit PolygonView (const Sub& poly) :
        Base (&poly, 1)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor. Creates a reference to a multi-polygon
    ///
    /// @param[in]  poly        Multi polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit PolygonView (const Multi& poly) :
        Base (poly)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Explicit cast operator, allowing this to be static_cast<>ed to a MultiPolygon
    ///
    /// @return     Copy of the polygon this references
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit operator Multi () const
        {
        Multi poly;

        poly.reserve (Base::size ());

        std::copy (Base::begin (),
                   Base::end (),
                   std::back_inserter (poly));

        return poly;
        }

private:
    using Base = QSpan<const ComplexPolygon<floating>>;
    };

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
MultiPolygon<floating1 || floating2> operate (PolygonView<floating1>    pg1,
                                              PolygonView<floating2>    pg2,
                                              polyOp                    op);

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
auto operator- (PolygonView<floating1>  pg1,
                PolygonView<floating2>  pg2)
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
auto operator& (PolygonView<floating1>  pg1,
                PolygonView<floating2>  pg2)
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
auto operator^ (PolygonView<floating1>  pg1,
                PolygonView<floating2>  pg2)
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
auto operator| (PolygonView<floating1>  pg1,
                PolygonView<floating2>  pg2)
    {
    return operate (pg1, pg2, POLY_UNION);
    }


///////////////////////////////////////////////////////////////////////////////
/// OPERATOR IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////


template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator-(const ComplexPolygon<otherFloating>& other) const
    {
    return PolygonView{ *this } - PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator-(const MultiPolygon<otherFloating>& other) const
    {
    return PolygonView{ *this } - PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator-(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } - PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator&(const ComplexPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } & PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator&(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } & PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator&(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } & PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator^(const ComplexPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } ^ PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator^(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } ^ PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator^(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } ^ PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator|(const ComplexPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } | PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator|(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } | PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto ComplexPolygon<floating>::operator|(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } | PolygonView{ other };
    }


template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator-(const ComplexPolygon<otherFloating>& other) const
    {
    return PolygonView{ *this } - PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator-(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } - PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator-(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } - PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator&(const ComplexPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } & PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator&(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } & PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator&(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } & PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator^(const ComplexPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } ^ PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator^(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } ^ PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator^(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } ^ PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator|(const ComplexPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } | PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator|(const MultiPolygon<otherFloating>&other) const
    {
    return PolygonView{ *this } | PolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto MultiPolygon<floating>::operator|(const PolygonView<otherFloating>&other) const
    {
    return PolygonView{ *this } | PolygonView{ other };
    }

} // namespace utils