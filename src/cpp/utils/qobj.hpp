/**
 * @file        utils/qobj.hpp
 * @brief       QObject utilities
 * @author      Justin Scott
 * @date        2026-09-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QObject>

/// QObject utilities
namespace utils::qobj
{
///////////////////////////////////////////////////////////////////////////////
/// Scoped object to enable/disable signals for a scope. Like QSignalBlocker,
/// but can also unblock signals for a scope
///
///////////////////////////////////////////////////////////////////////////////
class SignalGuard
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  obj     Object to enable/disable signals on
    /// @param[in]  block   True to block signals for this object's lifetime
    ///                     False to enable signals
    ///
    ///////////////////////////////////////////////////////////////////////////////
    SignalGuard (QObject& obj, bool block) :
        m_obj (obj),
        m_initial (obj.signalsBlocked ())
        { obj.blockSignals (block); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor; Restore ther initial state
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~SignalGuard () { m_obj.blockSignals (m_initial); }
private:
    QObject&    m_obj;
    bool        m_initial;
    };

} // namespace utils::qobj
