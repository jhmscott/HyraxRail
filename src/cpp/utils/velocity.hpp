/**
 * @file        utils/velocity.hpp
 * @brief       Class for storing a history of points and calculating the average velocity
 * @author      Justin Scott
 * @date        2026-10-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QPoint>
#include <QVector2D>

#include <deque>

namespace utils
{

//////////////////////////////////////////////////////////////////////////////
/// Class for storing a history of points and calculating the average velocity.
/// Options are provided in the constructor for the number of points to store/
/// calculate from. The average is weighted towards more recent points, with
/// a configurable weight
///
//////////////////////////////////////////////////////////////////////////////
class VelocityCalculator
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  numPoints   Size of history
    /// @param[in]  decay       The weighting decay [0.0,1.0]
    ///                         How much less the next point is weighted than the
    ///                         last. 1.0 weighs all points equally. 0.0 only
    ///                         considers the most recent point
    ///
    //////////////////////////////////////////////////////////////////////////////
    VelocityCalculator (size_t numPoints, double decay);

    //////////////////////////////////////////////////////////////////////////////
    /// Clear all points from the calculation
    ///
    //////////////////////////////////////////////////////////////////////////////
    void clear () { m_points.clear (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Add a point to be considered in the calculation
    ///
    /// @param[in]  pt      Point to add
    /// @param[in]  time    Timestamp of point, in milliseconds
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addPoint (const QPointF& pt, ulong time);

    //////////////////////////////////////////////////////////////////////////////
    /// Calculate the velocity
    ///
    /// @return     Forwarded weighted average velocity
    ///
    //////////////////////////////////////////////////////////////////////////////
    QVector2D getVelocity () const;
private:
    /// Point and timestamp
    struct timePoint
        {
        QPointF point;  ///< Point
        ulong   time;   ///< Timestamp
        };

    std::deque<timePoint>   m_points;   ///< Point history
    const size_t            m_numPoints;///< Maximum number of points to consider
    const double            m_decay;    ///< Decay factor
                                        ///  @see utils::VelocityCalculator::VelocityCalculator()
    };

} // namespace utils
