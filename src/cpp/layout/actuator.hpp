/**
 * @file        layout/actautor.hpp
 * @brief       Provides an interface for an actuator/
 *              switching device used in a model train layout
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/base.hpp>

#include <string>

namespace layout
{
// Symbol to use for the actuator in the UI
enum actuatorIcon
    {
    ICON_TURNOUT_LEFT,          ///< Straight track with left turnout
    ICON_TURNOUT_RIGHT,         ///< Straight track with right turnout


    NUM_TRUE_ICONS,             ///< Total number of unique icons. Delimiter only
    NO_ICON = NUM_TRUE_ICONS,   ///< No symbol for this actuator. Display a generic icon

    NUM_TOTAL_ICONS             ///< Delimitor only
    };

// Forward declare
class ActuatorController;

///////////////////////////////////////////////////////////////////////////////
/// Actuator component
///
///////////////////////////////////////////////////////////////////////////////
class Actuator : public ComponentDerived<ActuatorController>
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
   /// Constructor
   ///
   /// @param[in]   controller          Controller controlling this actuator
   /// @param[in]   name                       Friendly name
   /// @param[in]   icon                       Symbol to use in the UI
   /// @param[in]   id                           Unique ID
   /// @param[in]   state                    Initial state of the actuator
   ///
   ///////////////////////////////////////////////////////////////////////////////
    Actuator (ActuatorController*   controller,
              const std::string&    name,
              actuatorIcon          icon,
              size_t                id,
              bool                  state);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the UI icon
    ///
    /// @return     Enumerated icon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    actuatorIcon getIcon () const { return m_icon; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the friendly name of this actuator
    ///
    /// @return     Name of the actuator in the UI
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_name; }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Set the actuator value
    ///
    /// @param[in]  val         Boolean value to set
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void set (bool val);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the state of the actuator
    ///
    /// @return     Actuator state
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool get () const { return m_state; }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of this actuator. Required to call before set
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void request ();
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Release control of this actuator. Call when no longer using this
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void release ();

private:
    std::string         m_name;     ///< Friendly name
    actuatorIcon        m_icon;     ///< UI symbol
    bool                m_state;    ///< Cached state
    };

///////////////////////////////////////////////////////////////////////////////
/// Actuator controller. Interface used by Actuator class to communicate with the actuator controller
///
///////////////////////////////////////////////////////////////////////////////
class ActuatorController : public ControllerBase<Actuator>
    {
    friend class Actuator;
        
    // private, so only the actuator can call this
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Set the state of an actuator
    ///
    /// @param[in]  id          Unique ID of actuator to set
    /// @param[in]  val        Boolean value to set
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setActuator (size_t id, bool val) = 0;
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of an actuator
    ///
    /// @param[in]  id      Unique ID of actuator to request control of
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void requestActuatorControl (size_t id) = 0;
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Release control of an actuator
    ///
    /// @param[in]  id      Unique ID of actuator to release control of
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void releaseActuatorControl (size_t id) = 0;

    };

} // namespace layout
