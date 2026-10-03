/**
 * @file        virtual/vestop.hpp
 * @brief       Software defined estop controller
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/estop.hpp>
#include <layout/locomotive.hpp>
#include <layout/virtual/base.hpp>

namespace layout
{
///////////////////////////////////////////////////////////////////////////////
/// Software defined estop controller
///
///////////////////////////////////////////////////////////////////////////////
class VirtualEmergencyStopController :
    public EmergencyStopController,
    public VirtualControllerBase
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  locoController      Controller to control
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit VirtualEmergencyStopController (LocomotiveController& locoController) :
        m_locoController (locoController)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Trigger or leave an emergency stop state. This typically stops all locomotives
    /// by cutting track power
    ///
    /// @param[in]  stop    True to trigger an emergency stop
    ///                     False  to exit an emergency stop state
    ///                     (sometimes called a go command)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void eStop (bool stop);

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the controller is in the emergency stop state
    ///
    /// @return     True if the emergency stop state is active
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool isEStopped () const override { return m_isEStopped; }

private:
    using speedPair = std::pair<layout::Locomotive, int8_t>;

    std::vector<speedPair>  m_savedSpeeds;          ///< Speeds recorded from last e-stop
    LocomotiveController&   m_locoController;       ///< Locomotive controller
    bool                    m_isEStopped = false;   ///< Emergency stop state
    };

} // namespace layout