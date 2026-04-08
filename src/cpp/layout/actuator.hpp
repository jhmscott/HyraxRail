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

enum actuatorIcon
    {
    ICON_TURNOUT_LEFT,
    ICON_TURNOUT_RIGHT,


    NUM_TRUE_ICONS,
    NO_ICON = NUM_TRUE_ICONS,

    NUM_TOTAL_ICONS
    };

class ActuatorController;

class Actuator : public ComponentDerived<ActuatorController>
    {
public:
    Actuator (ActuatorController*   controller,
              const std::string&    name,
              actuatorIcon          icon,
              size_t                id,
              bool                  state);

    actuatorIcon getIcon () const { return m_icon; }

    std::string getName () const { return m_name; }

    void set (bool val);

    bool get () const { return m_state; }

    void request ();

    void release ();

private:
    std::string         m_name;
    actuatorIcon        m_icon;
    bool                m_state;
    };


class ActuatorController : public ControllerBase<Actuator>
    {
    friend class Actuator;
private:
    virtual void setActuator (size_t id, bool val) = 0;

    virtual void requestActuatorControl (size_t id) = 0;

    virtual void releaseActuatorControl (size_t id) = 0;

    };

}