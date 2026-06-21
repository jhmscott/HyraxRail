/**
 * @file        utils/poly.cpp
 * @brief       Complex polygon library. Polygon with holes
 * @author      Justin Scott
 * @date        2026-06-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ext/gpc.h>

#include <utils/poly.hpp>

namespace utils
{

namespace
{
// RAII vertex list
struct gpcVListManaged : std::vector<gpc_vertex>
    {
    ///////////////////////////////////////////////////////////////////////////////
    /// Convert to a non-owning vertex list
    ///
    /// @return     Vertex list representing this vector of vertices
    ///
    /// @remarks    As this is results in a non-owning copy, it's lifespan must
    ///             not be longer than the source VList
    ///
    ///////////////////////////////////////////////////////////////////////////////
    gpc_vertex_list toList () { return { static_cast<int> (size ()), data () }; }
    };

// RAII GPC polygon. First contour is assumed to be exterior
// All else are interior rings/holes
struct gpcPolygonManaged : std::vector<gpcVListManaged> {};

} // anpnymous namespace


///////////////////////////////////////////////////////////////////////////////
/// Convert a QPolygon to a GPC vertex list
///
/// @param[in]  pg      Polygon to convert
///
/// @return     Resulting vertex list
///
///////////////////////////////////////////////////////////////////////////////
static gpcVListManaged polyToGpc (const QPolygon& pg)
    {
    gpcVListManaged gpc;

    gpc.reserve (pg.size ());

    std::transform (pg.begin (),
                    pg.end (),
                    std::back_inserter (gpc),
                    [] (const QPoint& pt) -> gpc_vertex
                    { return { static_cast<double> (pt.x ()),
                               static_cast<double> (pt.y ()) }; });

    return gpc;
    }

///////////////////////////////////////////////////////////////////////////////
/// Convert a QPolygonF to a GPC vertex list
///
/// @param[in]  pg      Polygon to convert
///
/// @return     Resulting vertex list
///
///////////////////////////////////////////////////////////////////////////////
static gpcVListManaged polyToGpc (const QPolygonF& pg)
    {
    gpcVListManaged gpc;

    gpc.reserve (pg.size ());

    std::transform (pg.begin (),
                    pg.end (),
                    std::back_inserter (gpc),
                    [] (const QPointF& pt) -> gpc_vertex
                    { return { pt.x (), pt.y () }; });

    return gpc;
    }


///////////////////////////////////////////////////////////////////////////////
/// Convert a ComplexPolygon to a GPC Polygon
///
/// @tparam     floating    True if polygon coordinates are floating point
///
/// @param[in]  pg          Polygon to convert
/// @param[out] gpc         GPC polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating>
static void complexPolyToGpc (const ComplexPolygon<floating>&   pg,
                              gpc_polygon&                      gpc)
    {
    using Result = typename ComplexPolygon<floating>::Polygon;

    gpcPolygonManaged contours;

    contours.reserve (pg.interiorRings.size () + 1);

    contours.push_back (polyToGpc (pg.exteriorRing));

    std::transform (pg.interiorRings.begin (),
                    pg.interiorRings.end (),
                    std::back_inserter (contours),
                    QOverload<const Result&>::of (&polyToGpc));

    for (size_t ii = 0; ii < contours.size (); ++ii)
        {
        gpc_vertex_list contour = contours[ii].toList ();

        gpc_add_contour (&gpc, &contour, 0 == ii ? 0 : 1);
        }
    }


///////////////////////////////////////////////////////////////////////////////
/// Convert a GPC vertex list to a QPolygon
///
/// @param[in]  gpc     Source vertex list
/// @param[out] poly    Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
static void gpcToPoly (const gpc_vertex_list&   gpc,
                       QPolygon&                poly)
    {
    poly.clear ();
    poly.reserve (gpc.num_vertices);

    std::transform (gpc.vertex,
                    gpc.vertex + gpc.num_vertices,
                    std::back_inserter (poly),
                    [] (const gpc_vertex& pt) -> QPoint
                    { return { static_cast<int> (pt.x),
                               static_cast<int> (pt.y) }; });
    }


///////////////////////////////////////////////////////////////////////////////
/// Convert a GPC vertex list to a QPolygonF
///
/// @param[in]  gpc     Source vertex list
/// @param[out] poly    Resulting polygon
///
///////////////////////////////////////////////////////////////////////////////
static void gpcToPoly (const gpc_vertex_list&   gpc,
                       QPolygonF&               poly)
    {
    poly.clear ();
    poly.reserve (gpc.num_vertices);

    std::transform (gpc.vertex,
                    gpc.vertex + gpc.num_vertices,
                    std::back_inserter (poly),
                    [] (const gpc_vertex& pt) -> QPointF
                    { return { pt.x, pt.y }; });
    }


///////////////////////////////////////////////////////////////////////////////
/// Convert a GPC polygon to a complex polygon
///
/// @tparam     floating    True if polygon coordinates are floating point
///
/// @param[in]  gpc         Source GPC polygon
/// @param[out] pg          Resulting complex polygon
///
///////////////////////////////////////////////////////////////////////////////
template<bool floating>
static void gpcToComplexPoly (const gpc_polygon&        gpc,
                              ComplexPolygon<floating>& pg)
    {
    typename ComplexPolygon<floating>::Polygon temp;

    for (int ii = 0; ii < gpc.num_contours; ++ii)
        {
        gpcToPoly (gpc.contour[ii], temp);

        if (0 == gpc.hole[ii])
            {
            pg.exteriorRing = pg.exteriorRing.united (temp);
            }
        else
            {
            pg.interiorRings.push_back (std::move (temp));
            }
        }
    }

template<bool floating1, bool floating2>
ComplexPolygon<floating1 || floating2> operate (const ComplexPolygon<floating1>&    pg1,
                                                const ComplexPolygon<floating2>&    pg2,
                                                polyOp                              op)
    {
    using Result = ComplexPolygon<floating1 || floating2>;

    gpc_polygon gpc1{ 0 };
    gpc_polygon gpc2{ 0 };
    gpc_polygon gpcRes{ 0 };
    Result      res;

    complexPolyToGpc (pg1, gpc1);
    complexPolyToGpc (pg2, gpc2);

    gpc_polygon_clip (static_cast<gpc_op> (op),
                      &gpc1,
                      &gpc2,
                      &gpcRes);

    gpcToComplexPoly (gpcRes, res);

    gpc_free_polygon (&gpc1);
    gpc_free_polygon (&gpc2);
    gpc_free_polygon (&gpcRes);

    return res;
    }

// Explicit template instatiation
template ComplexPolygon<false>
operate<false, false> (const ComplexPolygon<false>& pg1,
                       const ComplexPolygon<false>& pg2,
                       polyOp                       op);

template ComplexPolygon<true>
operate<true, false>  (const ComplexPolygon<true>&  pg1,
                       const ComplexPolygon<false>& pg2,
                       polyOp                       op);


template ComplexPolygon<true>
operate<false, true>  (const ComplexPolygon<false>& pg1,
                       const ComplexPolygon<true>&  pg2,
                       polyOp                       op);

template ComplexPolygon<true>
operate<true, true>   (const ComplexPolygon<true>&  pg1,
                       const ComplexPolygon<true>&  pg2,
                       polyOp                       op);
}