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
struct funcInfo
    {
    enum icon_t
        {
        // Lights
        ICON_FUNC_LIGHT_HEADLIGHT,
        ICON_FUNC_LIGHT_CAB,

        // Sound
        ICON_FUNC_SOUND_HORN,
        ICON_FUNC_SOUND_BRAKES,
        ICON_FUNC_SOUND_COUPLING,
        ICON_FUNC_SOUND_GENERIC,
        ICON_FUNC_SOUND_OPERATING,

        // Miscillenaous
        ICON_FUNC_MISC_PANTOGRAPH,
        ICON_FUNC_MISC_ABV,
        ICON_FUNC_MISC_SLOW,

        NUM_TRUE_ICONS,

        ICON_FUNC_NUMBER = NUM_TRUE_ICONS,

        NUM_TOTAL_ICONS
        };

    std::string name;
    icon_t      icon;
    uint8_t     id;
    bool        state;
    };

class LocomotiveController;

class Locomotive : public ComponentDerived<LocomotiveController>
    {
    Q_OBJECT
public:
    Locomotive () = default;

    Locomotive (LocomotiveController*   controller,
                const std::string&      name,
                size_t                  id);

    std::string getName () const { return m_name; }

    void setSpeed (int8_t speed);

    void requestControl ();

    void releaseControl ();

    void setFunc (uint8_t func, bool enable);

    std::vector<funcInfo> getFunctions () const;

    // Move to base class?
    LocomotiveController* getController () { return m_controller; }

signals:
    void funcSet (uint8_t func, bool enable);

private:
    std::string m_name;
    };

class LocomotiveController : public ControllerBase<Locomotive>
    {
    friend class Locomotive;
private:

    virtual void setSpeed (size_t id, int8_t speed) = 0;

    virtual void requestControl (size_t id) = 0;

    virtual void releaseControl (size_t id) = 0;

    virtual void setFunc (size_t id, uint8_t func, bool enable) = 0;

    virtual std::vector<funcInfo> getFunctions (size_t id) const = 0;
    };



}