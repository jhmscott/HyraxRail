/**
 * @file        automation/item.hpp
 * @brief       Represents an item that can be triggered automatically
 * @author      Justin Scott
 * @date        2026-06-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/actuator.hpp>
#include <layout/route.hpp>

#include <QObject>

#include <bitset>

namespace control
{

///////////////////////////////////////////////////////////////////////////////
/// Wrapper for an item that can be automated
///
///////////////////////////////////////////////////////////////////////////////
class AutomationItem : public QObject
    {
    Q_OBJECT
public:
    // What type of automation this is
    // Keep this in sync with the variant index
    enum class type
        {
        ACTUATOR,   ///< layout::Actuator
        ROUTE       ///< layout:::Route
        };

    enum action
        {
        ACTION_SET,     ///< Set/turn on
        ACTION_UNSET,   ///< Unset/turn off
        ACTION_TOGGLE,  ///< Toggle the current state

        NUM_ACTIONS     ///< Delimiter only
        };

    // used to represent a list of supported actions
    using actions = std::bitset<NUM_ACTIONS>;

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two automation items
    ///
    /// @param[in]  other       Automation item to compare to
    ///
    /// @return     True if wrapping the same item
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool operator== (const AutomationItem& other) const { return m_item == other.m_item; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two automation items
    ///
    /// @param[in]  other       Automation item to compare to
    ///
    /// @return     True if wrapping different items
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool operator!= (const AutomationItem& other) const { return not (*this == other); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationItem () = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy constructor from actuator
    ///
    /// @param[in]  actuator        Actuator to copy
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationItem (const layout::Actuator& actuator);

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy constructor from route
    ///
    /// @param[in]  route       Route to copy
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationItem (const layout::Route& route);

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy constructor
    ///
    /// @param[in]  other       Item to copy
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationItem (const AutomationItem& other);

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy assignment operator
    ///
    /// @param[in]  other       Item to copy
    ///
    /// @return     Reference to this
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationItem& operator= (const AutomationItem& other)
        {
        if (this != &other) { m_item = other.m_item; connectSignals (); }

        return *this;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the type of automation item this is
    ///
    /// @return     Item type
    ///
    ///////////////////////////////////////////////////////////////////////////////
    type getType () const { return static_cast<type> (m_item.index ()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the actions supported by this item
    ///
    /// @return     Supported actions
    ///
    ///////////////////////////////////////////////////////////////////////////////
    actions getActions () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Perform an action on the item
    ///
    /// @param[in]  action      Action to execute
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void doAction (action action);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the actuator this wraps, or std::nullopt if not an actuator
    ///
    /// @return     Actuator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::optional<layout::Actuator> getActuator () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the route this wrap, or std::nullopt if not an actuator
    ///
    /// @return     Route
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::optional<layout::Route> getRoute () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the user defined name of this item
    ///
    /// @return     Item name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string name () const;

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals the component this is wrapping is being destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyed ();

private:
    std::variant<layout::Actuator, layout::Route> m_item;   ///< Underlying item being automated

    void connectSignals ();
    };


///////////////////////////////////////////////////////////////////////////////
/// Get the translated text for an automation action
///
/// @param[in]  action      Action to get text for
///
/// @return     Translated text
///
///////////////////////////////////////////////////////////////////////////////
inline QString actionText (AutomationItem::action action)
    {
    const QString ACTIONS[] =
        {
        QObject::tr ("Set"),
        QObject::tr ("Unset"),
        QObject::tr ("Toggle")
        };
    ASSERT_ARRAY_LENGTH (ACTIONS, AutomationItem::NUM_ACTIONS);

    return ACTIONS[action];
    }
}