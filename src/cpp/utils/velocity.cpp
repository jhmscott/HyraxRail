/**
 * @file        utils/velocity.cpp
 * @brief       Class for storing a history of points and calculating the average velocity
 * @author      Justin Scott
 * @date        2026-10-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/velocity.hpp>

#include <algorithm>

namespace utils
{
VelocityCalculator::VelocityCalculator (size_t numPoints, double decay) :
    m_numPoints (numPoints),
    m_decay (decay)
    {}

void VelocityCalculator::addPoint (const QPointF& pt, ulong time)
    {
    m_points.push_front ({ pt, time });

    if (m_points.size () > m_numPoints)
        {
        m_points.pop_back ();
        }
    }

QVector2D VelocityCalculator::getVelocity () const
    {
    QVector2D   average{ 0.0f, 0.0f };
    double      divisor = 0.0;
    double      weight  = 1.0;

    if (m_points.size () >= 2)
        {
        for (auto it = m_points.begin (); it < m_points.end () - 1; ++it)
            {
            auto jt = it + 1;

            QVector2D vec = 1000 * (QVector2D{ it->point } - QVector2D { jt->point }) /
                            std::max (it->time - jt->time, 1UL);

            average += vec * weight;
            divisor += weight;
            weight  *= m_decay;
            }

        average /= divisor;
        }

    return average;
    }
}
