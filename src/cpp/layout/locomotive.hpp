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
#include <layout/protocol.hpp>

#include <utils/algorithm.hpp>


#ifdef LAYOUT_TEST_CLASS
class LAYOUT_TEST_CLASS;
#endif // LAYOUT_TEST_CLASS

///////////////////////////////////////////////////////////////////////////////
/// Define a locomotive controller
///
/// @param[in]  ...         Variadic list of track protocols this supports
///
/// @ingroup    META_CLASS_MACRO
///
///////////////////////////////////////////////////////////////////////////////
#define LOCOMOTIVE_CONTROLLER_DEFINE(...) \
    public:\
        static const layout::LocomotiveControllerMetaClass& getLocoMetaClassStatic () { return locoMeta; } \
        virtual const layout::LocomotiveControllerMetaClass& getLocoMetaClass () const override { return locoMeta; } \
    private:\
        static inline const layout::LocomotiveControllerMetaClass locoMeta \
            { utils::algorithm::makeBitset<layout::TRACK_PROTO_UNKNOWN> (__VA_ARGS__) };


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

        // Miscellaneous
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

struct locomotiveState
    {
    std::string     m_name;     ///< Friendly name
    trackProtocol   m_proto;    ///< Protocol to communicate between the controller and loco
    uint            m_address;  ///< Track protocol address
    };


///////////////////////////////////////////////////////////////////////////////
/// Interface for controlling a locomotive
///
///////////////////////////////////////////////////////////////////////////////
class Locomotive : public ComponentDerived<LocomotiveController, locomotiveState>
    {
    Q_OBJECT
public:
    using Base = ComponentDerived<LocomotiveController, locomotiveState>;

    using Base::ComponentDerived;

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller  Controller controlling this locomotive
    /// @param[in]  name        Friendly name
    /// @param[in]  proto       Track protocol
    /// @param[in]  id          Unique ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Locomotive (LocomotiveController*   controller,
                const std::string&      name,
                trackProtocol           proto,
                uint                    address,
                size_t                  id);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the friendly name of this locomotive
    ///
    /// @return     Name for use in UI
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_state->m_name; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the locomotive name
    ///
    /// @param[in]  name        New name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setName (const std::string& name);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the locomotive speed
    ///
    /// @param[in]  speed       Speed value. Positive is forward. Negative is reverse.
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSpeed (int8_t speed);

    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of  this locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void requestControl ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Release control of  this locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void releaseControl ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Set a function value
    ///
    /// @param[in]  func    Function number
    /// @param[in]  enable  True to enable this function, false to disable
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
    /// Get the protocol the controller uses to communicate with the train over
    /// the track
    ///
    /// @return     Track protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    trackProtocol getProtocol () const { return m_state->m_proto; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the track protocol
    ///
    /// @param[in]  protocol    Track protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setProtocol (trackProtocol protocol);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the track protocol address
    ///
    /// @return     Track protocol address
    ///
    ///////////////////////////////////////////////////////////////////////////////
    uint getAddress () const { return m_state->m_address;  }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the track protocol address
    ///
    /// @param[in]  address     Track protocol address
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setAddress (uint address);

    ///////////////////////////////////////////////////////////////////////////////
    /// Remove this locomotive from the controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void remove ();
signals:
    // TODO: not used
    void funcSet (uint8_t func, bool enable);

    };


///////////////////////////////////////////////////////////////////////////////
/// Provides metadata about the locomotive controller
///
/// @ingroup    META_CLASS
///
///////////////////////////////////////////////////////////////////////////////
class LocomotiveControllerMetaClass
    {
public:
    const protocolMask protocols;   ///< List of supported track protocols
    };

///////////////////////////////////////////////////////////////////////////////
/// Interface used by the locomotive class to communicate with the controller
///
///////////////////////////////////////////////////////////////////////////////
class LocomotiveController : public ControllerBase<Locomotive>
    {
    friend class Locomotive;

#ifdef LAYOUT_TEST_CLASS
    friend class ::LAYOUT_TEST_CLASS;
#endif // LAYOUT_TEST_CLASS

public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the locomotive meta class instance
    ///
    /// @return     Meta class instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual const LocomotiveControllerMetaClass& getLocoMetaClass () const = 0;

    // private so only the locomotive class may call these functions
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Set the speed of a locomotive
    ///
    /// @param[in]  id      Unique ID of the locomotive
    /// @param[in]  speed   Speed value. Positve is forward. Negative is reverse
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setSpeed (size_t id, int8_t speed) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set a locomotive function
    ///
    /// @param[in]  id      Unique ID of locomotive
    /// @param[in]  func    Function number
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
    /// Set the locomotive name
    ///
    /// @param[in]  id      Unique ID of locomotive
    /// @param[in]  name    New name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setLocomotiveName (size_t id, const std::string& name) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the track protocol address
    ///
    /// @param[in]  id          Unique ID of locomotive
    /// @param[in]  address     Track protocol address
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setLocomotiveAddress (size_t id, uint address) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the track protocol
    ///
    /// @param[in]  id          Unique ID of locomotive
    /// @param[in]  protocol    Track protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setLocomotiveProtocol (size_t id, trackProtocol proto) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Remove this locomotive from this controller
    ///
    /// @param[in]  id      Locomotive ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void removeLocomotive (size_t id) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of  a locomotive
    ///
    /// @param[in]  id      Unique ID of the locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void requestControl (size_t id) = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Request control of  a locomotive
    ///
    /// @param[in]  id      Unique ID of the locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void releaseControl (size_t id) = 0;

    };



} // namespcae layout
