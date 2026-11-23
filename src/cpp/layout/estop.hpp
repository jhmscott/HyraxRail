/**
 * @file        layout/estop.hpp
 * @brief       Controller that can cut power to all locomotives in an emergency
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QObject>

namespace layout
{
///////////////////////////////////////////////////////////////////////////////
/// Controller that can cut power to all locomotives in an emergency
///
///////////////////////////////////////////////////////////////////////////////
class EmergencyStopController : public QObject
    {
    Q_OBJECT
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
    void eStop (bool stop) { doEStop (stop); emit eStopped (stop, false); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the controller is in the emergency stop state
    ///
    /// @return     True if the emergency stop state is active
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool isEStopped () const = 0;

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals a n estop was requested
    ///
    /// @param[in]  stop        True if stop was requested, false if go was requested
    /// @param[in]  external    True if the stop was requested externally from this
    ///                         app, typically meaning it originated directly on the
    ///                         controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void eStopped (bool stop, bool external);

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Send the estop command to the controller
    ///
    /// @param[in]  stop    True to trigger an emergency stop
    ///                     False  to exit an emergency stop state
    ///                     (sometimes called a go command)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void doEStop (bool stop) = 0;
    };
}
