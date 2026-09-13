/**
 * @file        utils/qobj.hpp
 * @brief       QObject utilities
 * @author      Justin Scott
 * @date        2026-09-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <common.hpp>

#include <utils/algorithm.hpp>
#include <utils/traits.hpp>

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
    QObject&    m_obj;      ///< Object we are blocking/unblocking
    bool        m_initial;  ///< Original state of the signal blocking
    };

///////////////////////////////////////////////////////////////////////////////
/// Object connection class that automatically closes itself
///
///////////////////////////////////////////////////////////////////////////////
class RaiiConnection
    {
public:
    /// Default constructor
    RaiiConnection () = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit constructor from a connection
    ///
    /// @param[in]  connection      Connection to manage
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit RaiiConnection (const QMetaObject::Connection& connection) :
        m_connection (connection)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit move constructor from a connection
    ///
    /// @param[in]  connection      Connection to manage
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit RaiiConnection (QMetaObject::Connection&& connection) :
        m_connection (std::move (connection))
        {}

    /// Default copy
    RaiiConnection (const RaiiConnection&) = default;
    RaiiConnection& operator= (const RaiiConnection&) = default;

    /// Default move
    RaiiConnection (RaiiConnection&&) = default;
    RaiiConnection& operator= (RaiiConnection&&) = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor, closes the connection
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~RaiiConnection ()
        {
        if (m_connection) { QObject::disconnect (m_connection); }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Implicit cast to unmanaged connection
    ///
    /// @return     Connection being managed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    implicit operator QMetaObject::Connection () const { return m_connection; }

private:
    QMetaObject::Connection m_connection;   ///< Connection we are managing

    };

///////////////////////////////////////////////////////////////////////////////
/// Class to inherit from if you want your class to recieve signals, but not
/// inherit from QObject. This ensures the connections are closed when the object
/// is destroyed
///
///////////////////////////////////////////////////////////////////////////////
class SignalClient
    {
public:
    /// Default constructor
    SignalClient () = default;

    /// Non-copyable
    SignalClient (const SignalClient&) = delete;
    SignalClient& operator= (const SignalClient&) = delete;

    ///////////////////////////////////////////////////////////////////////////////
    /// Connect a signal to a slot
    ///
    /// @tparam     Func1       Signal function type
    /// @tparam     Func2       Slot function type
    ///
    /// @param[in]  sender      QObject sending the message
    /// @param[in]  signal      Signal to connect to
    /// @param[in]  slot        Slot to receive the connection
    ///
    /// @remarks    slot        If slot is a member function of a class inheriting
    ///                         from the derived type, it is assumed to be from
    ///                         associated with this instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Func1, class Func2>
    void connect (const typename traits::memberFuncTraits<Func1>::class_t*  sender,
                  Func1                                                     signal,
                  Func2                                                     slot)
        {
        QMetaObject::Connection connection;

        if constexpr (std::is_member_function_pointer_v<Func2>)
            {
            using Class = typename traits::memberFuncTraits<Func2>::class_t;

            if constexpr (std::is_base_of_v<SignalClient, Class>)
                {
                auto* self = static_cast<Class*> (this);

                connection = QObject::connect (sender,
                                               signal,
                                               algorithm::bindMemFn (slot, *self));
                }
            else
                {
                static_assert (traits::always_false_v<Func1>, "Unbound member function");
                }
            }
        else
            {
            connection = QObject::connect (sender, signal, slot);
            }

        m_connections.emplace_back (std::move (connection));
        }
private:
    std::vector<RaiiConnection> m_connections;
    };

} // namespace utils::qobj
