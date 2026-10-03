/**
 * @file        virtual/vestop.cpp
 * @brief       Software defined estop controller
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <layout/virtual/vestop.hpp>

namespace layout
{
void VirtualEmergencyStopController::eStop (bool stop)
    {
    if (stop == m_isEStopped)
        {
        // State not changed
        }
    else if (stop)
        {
        auto locos = m_locoController.getLocomotives ();

        m_savedSpeeds.clear ();
        m_savedSpeeds.reserve (locos.size ());

        for (auto& loco : locos)
            {
            m_savedSpeeds.push_back (std::make_pair (loco, loco.getSpeed ()));
            loco.setSpeed (0);
            }
        }
    else
        {
        for (auto& [loco, speed] : m_savedSpeeds)
            {
            loco.setSpeed (speed);
            }
        }

    m_isEStopped = stop;
    }

} // namespace layout
