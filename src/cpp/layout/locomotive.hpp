/**
 * @file        layout/locomotive.hpp
 * @brief       Provides an interface for controlling a locomotive
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include <layout/base.hpp>

namespace layout
{
// Information about a locomitve function
struct funcInfo
    {
    // Symbol to use in the UI
    enum icon_t
        {
        // Lights
        ICON_FUNC_LIGHT_HEADLIGHT,  ///< Headlight
        ICON_FUNC_LIGHT_CAB,        ///< Cabin lighting

        // Sound
        ICON_FUNC_SOUND_HORN,       ///< Horn Sound effect
        ICON_FUNC_SOUND_BRAKES,     ///< Brake sound effect
        ICON_FUNC_SOUND_COUPLING,   ///< Train coupling sound effect
        ICON_FUNC_SOUND_GENERIC,    ///< Generic sound effect symbol
        ICON_FUNC_SOUND_OPERATING,  ///< Operating sound effects

        // Miscillenaous
        ICON_FUNC_MISC_PANTOGRAPH,  ///< Raise/lower the locomotive pantograph
        ICON_FUNC_MISC_ABV,         ///< ABV? Can't remember this one
        ICON_FUNC_MISC_SLOW,        ///< Slow the lcoomootive

        NUM_TRUE_ICONS,             ///< Number of true icons. Delimiter only

        ICON_FUNC_NUMBER =          ///< Use the function number as the icon
            NUM_TRUE_ICONS,

        NUM_TOTAL_ICONS             ///< Delimiter only
        };

    std::string name;               ///< Friendly name
    icon_t      icon;               ///< UI Symbol
    uint8_t     id;                 ///< Function number
    bool        state;              ///< Current state
    };

// forward declare
class LocomotiveController;


///////////////////////////////////////////////////////////////////////////////
/// Interface for controlling a locomotive
///
///////////////////////////////////////////////////////////////////////////////
class Locomotive : public ComponentDerived<LocomotiveController>
    {
    Q_OBJECT
public:
    // Default constructor. TODO: is still needed?
    Locomotive () = default;
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller          Controller controlling this locomotive
    /// @param[in]  name                        Friendly name
    /// @param[in]  id                            Unique ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Locomotive (LocomotiveController*   controller,
                const std::string&      name,
                size_t                  id);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the friendly name of this locomotive
    ///
    /// @return     Name for use in UI
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_name; }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Set the locomotive speed
    ///
    /// @param[in]  speed           Speed value. Positive is forward. Negative is reverse.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSpeed (int8_t speed);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of  this locomitve
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void requestControl ();
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Release control of  this locomitve
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void releaseControl ();
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Set a function value
    ///
    /// @param[in]  func        Function number
    /// @param[in]  enable   True to enable this function, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFunc (uint8_t func, bool enable);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the functions supported by this locomotive
    ///
    /// @return     List of functions
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<funcInfo> getFunctions () const;

        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the controller controlling this locomotive
    ///
    /// @return     Locomotive controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    LocomotiveController* getController () { return m_controller; }

signals:
    // TODO: not used
    void funcSet (uint8_t func, bool enable);

private:
    std::string m_name;     ///< Friendly name
    };


///////////////////////////////////////////////////////////////////////////////
/// Interface used by the locomoitve class to communicate with the controller
///
///////////////////////////////////////////////////////////////////////////////
class LocomotiveController : public ControllerBase<Locomotive>
    {
    friend class Locomotive;
        
    // private so only the locomotive class may call these functions
private:
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Set the speed of a locomotive
    ///
    /// @param[in]  id          Unique ID of the locomotive
    /// @param[in]  speed    Speed value. Positve is forward. Negative is reverse
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setSpeed (size_t id, int8_t speed) = 0;
        
    
    ///////////////////////////////////////////////////////////////////////////////
    /// Set a locomotive function
    ///
    /// @param[in]  id          Unique ID of locomotive
    /// @param[in]  func      Function number
    /// @param[in]  enable  True to enable the function. False to disable
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setFunc (size_t id, uint8_t func, bool enable) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get a list of the function supported by a locomotive
    ///
    /// @param[in]  id      Unique ID of locomotive
    ///
    /// @return     List of functions
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::vector<funcInfo> getFunctions (size_t id) const = 0;
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of  a locomitve
    ///
    /// @param[in]  id          Unique ID of the locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void requestControl (size_t id) = 0;
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of  a locomitve
    ///
    /// @param[in]  id          Unique ID of the locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void releaseControl (size_t id) = 0;

    };



} // namespcae layout
