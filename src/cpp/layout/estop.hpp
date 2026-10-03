/**
 * @file        layout/estop.hpp
 * @brief       Controller that can cut power to all locomotives in an emergency
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

namespace layout
{
///////////////////////////////////////////////////////////////////////////////
/// Controller that can cut power to all locomotives in an emergency
///
///////////////////////////////////////////////////////////////////////////////
class EmergencyStopController
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Trigger or leave an emergency stop state. This typically stops all locomotives
    /// by cutting track power
    ///
    /// @param[in]  stop    True to trigger an emergency stop
    ///                     False  to exit an emergency stop state
    ///                     (sometimes called a go command)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void eStop (bool stop) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the controller is in the emergency stop state
    ///
    /// @return     True if the emergency stop state is active
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool isEStopped () const = 0;
    };
}