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
#include <limits>
#include <vector>

namespace utils
{

/// Polygon operations
/// Keep in sync with gpc_op
enum polyOp
    {
    POLY_DIFF,  ///< Subtract a polygon
    POLY_INT,   ///< Take the intersection of two polygon
    POLY_XOR,   ///< Exclusive or of two polygon
    POLY_UNION  ///< Take the union of two polygon
    };

// Forward declarations

template<bool floating>
class BasicMultiPolygon;

template<bool floating>
class BasicPolygonView;

///////////////////////////////////////////////////////////////////////////////
/// Complex polygon class. Polygon with holes
///
/// @tparam     floating        True to use floating point for coordinates
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating>
class BasicComplexPolygon
    {
public:

    using Polygon   = std::conditional_t<floating, QPolygonF, QPolygon>;///< Polygon ring component
    using Point     = std::conditional_t<floating, QPointF, QPoint>;    ///< Point within polygon
    using Coordinate= std::conditional_t<floating, qreal, int>;         ///< Point component
    using Rect      = std::conditional_t<floating, QRectF, QRect>;      ///< Rectangle type
    using RingList  = std::vector<Polygon>;                             ///< List of rings

    Polygon     exteriorRing;   ///< Main, exterior polygon ring
    RingList    interiorRings;  ///< Interior rings/holes

    // Default constructor
    BasicComplexPolygon () = default;

    // Default copying
    BasicComplexPolygon (const BasicComplexPolygon&) = default;
    BasicComplexPolygon& operator= (const BasicComplexPolygon&) = default;

    // Default moving
    BasicComplexPolygon (BasicComplexPolygon&&) = default;
    BasicComplexPolygon& operator= (BasicComplexPolygon&&) = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor from simply polygon and optional list of holes
    ///
    /// @param[in]  exterior        Main polygon ring
    /// @param[in]  interior        (optional) List of holes
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit BasicComplexPolygon (const Polygon& exterior, const RingList& interior = {}) :
        exteriorRing (exterior),
        interiorRings (interior)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy constructor from point list
    ///
    /// @param[in]  pts         Points making up exterior polygon ring
    /// @param[in]  num         Number of points
    ///
    ///////////////////////////////////////////////////////////////////////////////
    BasicComplexPolygon (const Point* pts, size_t num)
        {
        exteriorRing.reserve (num);
        std::copy (pts,
                   pts + num,
                   std::back_inserter (exteriorRing));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor from point list
    ///
    /// @param[in]  pts         Points making up exterior polygon ring
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit BasicComplexPolygon (const QList<Point>& pts) :
        BasicComplexPolygon (pts.data (), pts.size ())
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor from point list
    ///
    /// @tparam     N           Number of points
    ///
    /// @param[in]  pts         Points making up exterior polygon ring
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<size_t N>
    implicit BasicComplexPolygon (const Point (&pts)[N]) :
        BasicComplexPolygon (pts, N)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Initializer list cnstructor
    ///
    /// @param[in]  pts     List of points
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit BasicComplexPolygon (std::initializer_list<Point> pts)
        {
        exteriorRing.reserve (pts.size ());
        std::copy (pts.begin (),
                   pts.end (),
                   std::back_inserter (exteriorRing));
        }

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
                       std::bind (std::mem_fn (QOverload<const Point&>::of (&Polygon::translate)),
                                  std::placeholders::_1,
                                  offset));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Translate this polygon
    ///
    /// @param[in]  x       X-offset
    /// @param[in]  y       Y-offset
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translate (Coordinate x, Coordinate y) { translate (Point{ x, y }); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a translated copy of this polygon
    ///
    /// @param[in]  offset      Amount to offset polygon by
    ///
    /// @return     Translated polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]]
    BasicComplexPolygon translated (const QPoint& offset) const
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

        return BasicComplexPolygon{ newExterior, newInterior };
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a translated copy of this polygon
    ///
    /// @param[in]  x       X-offset
    /// @param[in]  y       Y-offset
    ///
    /// @return     Translated polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]]
    BasicComplexPolygon translated (Coordinate x, Coordinate y) const { return translated (Point{ x, y }); }

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
    /// Rotate this polygon by an angle
    ///
    /// @param[in]  angle       Angle in degrees [0,360]
    ///
    /// @remarks    Polygon is rotated about (0,0)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rotate (double angle);

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a rotated copy of this polygon
    ///
    /// @param[in]  angle       Angle in degrees [0,360]
    ///
    /// @return     Rotated polygon
    ///
    /// @remarks    Polygon is rotated about (0,0)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] BasicComplexPolygon rotated (double angle) const
        {
        BasicComplexPolygon copy = *this;
        copy.rotate (angle);

        return copy;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Expand/contract this polygon
    ///
    /// @param[in]      scale       Scale factor    <BR>
    ///                             > 1.0 : Expand  <BR>
    ///                             < 1.0 : Contract<BR>
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void expand (double scale);

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an expanded/contracted copy of this polygon
    ///
    /// @param[in]      scale       Scale factor    <BR>
    ///                             > 1.0 : Expand  <BR>
    ///                             < 1.0 : Contract<BR>
    ///
    /// @return         Expanded polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] BasicComplexPolygon expanded (double scale) const
        {
        BasicComplexPolygon copy = *this;
        copy.expand (scale);

        return copy;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the polygon's bounding rect
    ///
    /// @return     Polygon rectangular extents
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Rect boundingRect () const { return exteriorRing.boundingRect (); }

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
    auto operator- (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator- (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator- (const BasicPolygonView<otherFloating>& other) const;

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
    auto operator& (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator& (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator& (const BasicPolygonView<otherFloating>& other) const;

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
    auto operator^ (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator^ (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto  operator^ (const BasicPolygonView<otherFloating>& other) const;

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
    auto  operator| (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator| (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator| (const BasicPolygonView<otherFloating>& other) const;
    };

// Floating point complex polygon
using ComplexPolygonF = BasicComplexPolygon<true>;

// Integer complex polygon
using ComplexPolygon = BasicComplexPolygon<false>;

///////////////////////////////////////////////////////////////////////////////
/// Set of polygons with holes
///
/// @tparam     floating        True if coordinates are floating point
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating>
class BasicMultiPolygon : public std::vector<BasicComplexPolygon<floating>>
    {
public:
    using SubPolygon    = BasicComplexPolygon<floating>;    ///< Polygon within the collection
    using Point         = typename SubPolygon::Point;       ///< Point within a polygon
    using Rect          = typename SubPolygon::Rect;        ///< Rectangle type
    using Coordinate    = typename SubPolygon::Coordinate;  ///< Point coordinate type

    // using std::vector<SubPolygon>::vector<SubPolygon>;

    ///////////////////////////////////////////////////////////////////////////////
    /// Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    BasicMultiPolygon () = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Allow implicit copies from Complex Polygons
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit BasicMultiPolygon (const SubPolygon& poly)
        { Base::push_back (poly); }

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
    /// Rotate this polygon by an angle
    ///
    /// @param[in]  angle       Angle in degrees [0,360]
    ///
    /// @remarks    Polygon is rotated about (0,0)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rotate (double angle)
        {
        for (SubPolygon& poly : *this)
            {
            poly.rotate (angle);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a rotated copy of this polygon
    ///
    /// @param[in]  angle       Angle in degrees [0,360]
    ///
    /// @return     Rotated polygon
    ///
    /// @remarks    Polygon is rotated about (0,0)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] BasicMultiPolygon rotated (double angle) const
        {
        BasicMultiPolygon copy = *this;
        copy.rotate (angle);
        return copy;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Expand/contract this polygon
    ///
    /// @param[in]      scale       Scale factor    <BR>
    ///                             > 1.0 : Expand  <BR>
    ///                             < 1.0 : Contract<BR>
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void expand (double scale)
        {
        for (SubPolygon& poly : *this)
            {
            poly.expand (scale);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an expanded/contracted copy of this polygon
    ///
    /// @param[in]      scale       Scale factor    <BR>
    ///                             > 1.0 : Expand  <BR>
    ///                             < 1.0 : Contract<BR>
    ///
    /// @return         Expanded polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] BasicMultiPolygon expanded (double scale) const
        {
        BasicMultiPolygon copy = *this;
        copy.expand (scale);
        return copy;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the bounding rect of this polygon
    ///
    /// @return         Polygon's rectangular extents
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Rect boundingRect () const
        {
        Coordinate  top     = std::numeric_limits<Coordinate>::max ();
        Coordinate  left    = std::numeric_limits<Coordinate>::max ();
        Coordinate  bottom  = std::numeric_limits<Coordinate>::lowest ();
        Coordinate  right   = std::numeric_limits<Coordinate>::lowest ();

        for (const SubPolygon& poly : *this)
            {
            Rect rect = poly.boundingRect ();

            top     = std::min (top,    rect.top ());
            left    = std::min (left,   rect.left ());
            bottom  = std::max (bottom, rect.bottom ());
            right   = std::max (right,  rect.right ());
            }

        return Rect{ left, top, right - left, bottom - top};
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Add an offset this polygon
    ///
    /// @param[in]  offset      Polygon offset
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translate (const Point& offset)
        {
        for (SubPolygon& poly : *this)
            {
            poly.translate (offset);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Add an offset this polygon
    ///
    /// @param[in]  x           X offset
    /// @param[in]  y           Y offset
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translate (Coordinate x, Coordinate y) { translate (Point{ x, y }); }

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
    auto operator- (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator- (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator- (const BasicPolygonView<otherFloating>& other) const;

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
    auto operator& (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator& (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator& (const BasicPolygonView<otherFloating>& other) const;

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
    auto operator^ (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator^ (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto  operator^ (const BasicPolygonView<otherFloating>& other) const;

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
    auto  operator| (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator| (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator| (const BasicPolygonView<otherFloating>& other) const;

private:
    using Base = std::vector<BasicComplexPolygon<floating>>;    ///< Base vector type
    };

// Floating point multi-polygon
using MultiPolygonF = BasicMultiPolygon<true>;

// Integer multi-polygon
using MultiPolygon  = BasicMultiPolygon<false>;

///////////////////////////////////////////////////////////////////////////////
/// Non-owning read only copy of a polygon. Meant to be used in place of a
/// const MultiPolygon<>& when you want the interface to accept ComplexPolygon<>s
/// as well.
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating>
class BasicPolygonView : public QSpan<const BasicComplexPolygon<floating>>
    {
public:
    using Multi = BasicMultiPolygon<floating>;  ///< Multi-polygon type this can be created from
    using Sub   = typename Multi::SubPolygon;   ///< Single-polygon type this can be created from

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor. Creates a reference to a complex polygon
    ///
    /// @param[in]  poly        Complex polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit BasicPolygonView (const BasicComplexPolygon<floating>& poly) :
        Base (&poly, 1)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit copy constructor. Creates a reference to a multi-polygon
    ///
    /// @param[in]  poly        Multi polygon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit BasicPolygonView (const BasicMultiPolygon<floating>& poly) :
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
    auto operator- (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator- (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator& (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator& (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto operator^ (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator^ (const BasicMultiPolygon<otherFloating>& other) const;

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
    auto  operator| (const BasicComplexPolygon<otherFloating>& other) const;

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
    auto operator| (const BasicMultiPolygon<otherFloating>& other) const;

private:
    using Base = QSpan<const BasicComplexPolygon<floating>>;
    };

using PolygonViewF = BasicPolygonView<true>;

using PolygonView  = BasicPolygonView<false>;

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
BasicMultiPolygon<floating1 || floating2> operate (BasicPolygonView<floating1>  pg1,
                                                   BasicPolygonView<floating2>  pg2,
                                                   polyOp                       op);

// Transformation functions for QPolygon and QPolygonF
namespace poly
{

///////////////////////////////////////////////////////////////////////////////
/// Rotate a polygon
///
/// @param[in,out]  poly        Polygon to rotate
/// @param[in]      angle       Angle to rotate by in degress [0,360]
///
///////////////////////////////////////////////////////////////////////////////
void rotate (QPolygonF& poly, double angle);

///////////////////////////////////////////////////////////////////////////////
/// Rotate a polygon
///
/// @param[in,out]  poly        Polygon to rotate
/// @param[in]      angle       Angle to rotate by in degress [0,360]
///
///////////////////////////////////////////////////////////////////////////////
void rotate (QPolygon& poly, double angle);

///////////////////////////////////////////////////////////////////////////////
/// Expand or contract a polygon
///
/// @param[in,out]  poly        Polygon to expand/contract
/// @param[in]      scale       Scale factor
///                             > 1.0 : Expand
///                             < 1.0 : Contract
///
///////////////////////////////////////////////////////////////////////////////
void expand (QPolygonF& poly, double scale);

///////////////////////////////////////////////////////////////////////////////
/// Expand or contract a polygon
///
/// @param[in,out]  poly        Polygon to expand/contract
/// @param[in]      scale       Scale factor
///                             > 1.0 : Expand
///                             < 1.0 : Contract
///
///////////////////////////////////////////////////////////////////////////////
void expand (QPolygon& poly, double scale);

///////////////////////////////////////////////////////////////////////////////
/// Create a polygon from a rect with rounded corners
///
/// @param[in]  rect        Rect to create polygon for
/// @param[in]  rx          X radius of rounding
/// @param[in]  ry          Y radius of rounding
///
/// @return     Polygon of rounded rect
///
///////////////////////////////////////////////////////////////////////////////
QPolygon roundedRect (const QRect& rect, int rx, int ry);

///////////////////////////////////////////////////////////////////////////////
/// Create a polygon from a rect with rounded corners
///
/// @param[in]  rect        Rect to create polygon for
/// @param[in]  rx          X radius of rounding
/// @param[in]  ry          Y radius of rounding
///
/// @return     Polygon of rounded rect
///
///////////////////////////////////////////////////////////////////////////////
QPolygonF roundedRect (const QRectF& rect, qreal rx, qreal ry);

///////////////////////////////////////////////////////////////////////////////
/// Create a polygon for a circle
///
/// @param[in]  center      Circle center point
/// @param[in]  radius      Radius of circle
///
/// @return     Circle polygon
///
///////////////////////////////////////////////////////////////////////////////
QPolygonF circle (const QPointF& center, qreal radius);

} // namespace poly




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
auto operator- (BasicPolygonView<floating1>  pg1,
                BasicPolygonView<floating2>  pg2)
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
auto operator& (BasicPolygonView<floating1>  pg1,
                BasicPolygonView<floating2>  pg2)
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
auto operator^ (BasicPolygonView<floating1>  pg1,
                BasicPolygonView<floating2>  pg2)
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
auto operator| (BasicPolygonView<floating1>  pg1,
                BasicPolygonView<floating2>  pg2)
    {
    return operate (pg1, pg2, POLY_UNION);
    }


///////////////////////////////////////////////////////////////////////////////
/// OPERATOR IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////


template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator-(const BasicComplexPolygon<otherFloating>& other) const
    {
    return BasicPolygonView{ *this } - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator-(const BasicMultiPolygon<otherFloating>& other) const
    {
    return BasicPolygonView{ *this } - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator-(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator&(const BasicComplexPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator&(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator&(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator^(const BasicComplexPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator^(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator^(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator|(const BasicComplexPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } | BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator|(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } | BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicComplexPolygon<floating>::operator|(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } | BasicPolygonView{ other };
    }


template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator-(const BasicComplexPolygon<otherFloating>& other) const
    {
    return BasicPolygonView{ *this } - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator-(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator-(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator&(const BasicComplexPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator&(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator&(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator^(const BasicComplexPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator^(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator^(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator|(const BasicComplexPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } | BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator|(const BasicMultiPolygon<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } | BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicMultiPolygon<floating>::operator|(const BasicPolygonView<otherFloating>&other) const
    {
    return BasicPolygonView{ *this } | BasicPolygonView{ other };
    }

template<bool floating>
inline void BasicComplexPolygon<floating>::rotate (double angle)
    {
    poly::rotate (exteriorRing, angle);

    for (Polygon& poly : interiorRings)
        {
        poly::rotate (poly, angle);
        }
    }

template<bool floating>
inline void BasicComplexPolygon<floating>::expand (double scale)
    {
    poly::expand (exteriorRing, scale);

    for (Polygon& poly : interiorRings)
        {
        poly::expand (poly, scale);
        }
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator-(const BasicComplexPolygon<otherFloating>& other) const
    {
    return *this - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator-(const BasicMultiPolygon<otherFloating>& other) const
    {
    return *this - BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator&(const BasicComplexPolygon<otherFloating>& other) const
    {
    return *this & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator&(const BasicMultiPolygon<otherFloating>& other) const
    {
    return *this & BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator^(const BasicComplexPolygon<otherFloating>& other) const
    {
    return *this ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator^(const BasicMultiPolygon<otherFloating>& other) const
    {
    return *this ^ BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator|(const BasicComplexPolygon<otherFloating>& other) const
    {
    return *this | BasicPolygonView{ other };
    }

template<bool floating>
template<bool otherFloating>
inline auto BasicPolygonView<floating>::operator|(const BasicMultiPolygon<otherFloating>& other) const
    {
    return *this | BasicPolygonView{ other };
    }

} // namespace utils