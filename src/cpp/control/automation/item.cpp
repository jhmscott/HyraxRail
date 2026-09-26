/**
 * @file        automation/item.cpp
 * @brief       Represents an item that can be triggered automatically
 * @author      Justin Scott
 * @date        2026-06-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/item.hpp>

#include <utils/algorithm.hpp>

namespace control
{

AutomationItem::AutomationItem (const layout::Actuator& actuator) :
    m_item (actuator)
    {
    connectSignals ();
    }

AutomationItem::AutomationItem (const layout::Route& route) :
    m_item (route)
    {
    connectSignals ();
    }

AutomationItem::AutomationItem (const layout::Locomotive& loco, uint8_t num) :
    m_item (function{ loco, num })
    {
    connectSignals ();
    }

AutomationItem::AutomationItem (const AutomationItem& other) :
    m_item (other.m_item)
    {
    connectSignals ();
    }

AutomationItem::actions AutomationItem::getActions () const
    {
    actions actions;

    switch (getType ())
        {
        case type::ROUTE:
            {
            utils::algorithm::makeBitset (actions, ACTION_SET);
            break;
            }

        case type::LOCO_FUNC:
        case type::ACTUATOR:
            {
            utils::algorithm::makeBitset (actions,
                                          ACTION_SET,
                                          ACTION_TOGGLE,
                                          ACTION_UNSET);
            break;
            }
        }

    return actions;
    }

void AutomationItem::doAction (action action)
    {
    if (getActions ()[action])
        {
        switch (action)
            {
            case ACTION_SET:
                {
                setState (true);
                break;
                }

            case ACTION_UNSET:
                {
                setState (false);
                break;
                }

            case ACTION_TOGGLE:
                {
                setState (not getState ());
                break;
                }

            case NUM_ACTIONS:
                {
                // ignore delimiter
                break;
                }
            }
        }
    else
        {
        qDebug () << "Programming Error: Unsupported action";
        }
    }

std::optional<layout::Actuator> AutomationItem::getActuator () const
    {
    std::optional<layout::Actuator> actuator;

    if (type::ACTUATOR == getType ())
        {
        actuator = std::get<0> (m_item);
        }

    return actuator;
    }

std::optional<layout::Route> AutomationItem::getRoute () const
    {
    std::optional<layout::Route> route;

    if (type::ROUTE == getType ())
        {
        route = std::get<1> (m_item);
        }

    return route;
    }

std::optional<AutomationItem::function> AutomationItem::getFunction () const
    {
    std::optional<function> func;

    if (type::LOCO_FUNC == getType ())
        {
        func = std::get<2> (m_item);
        }

    return func;
    }

std::optional<layout::funcInfo> AutomationItem::getFunctionInfo () const
    {
    std::optional<function>         func = getFunction ();
    std::optional<layout::funcInfo> info;

    if (func.has_value ())
        {
        info = func->loco.getFunctionByNumber (func->func);
        }

    return info;
    }

std::string AutomationItem::name () const
    {
    std::string name;

    switch (getType ())
        {
        case type::ACTUATOR:
            {
            name = getActuator ()->getName ();
            break;
            }

        case type::ROUTE:
            {
            name = getRoute ()->getName ();
            break;
            }
        case type::LOCO_FUNC:
            {
            auto info = getFunctionInfo ();

            if (info.has_value ())
                {
                name = info->uiName ().toStdString ();
                }
            break;
            }
        }

    return name;
    }

void AutomationItem::connectSignals ()
    {
    switch (getType ())
        {
        case type::ACTUATOR:
            {
            connect (&std::get<0> (m_item),
                     &layout::Actuator::destroyed,
                      this,
                     &AutomationItem::destroyed);
            break;
            }
        case type::ROUTE:
            {
            connect (&std::get<1> (m_item),
                     &layout::Route::destroyed,
                      this,
                     &AutomationItem::destroyed);
            break;
            }

        case type::LOCO_FUNC:
            {
            auto& [loco, num] = std::get<2> (m_item);

            connect (&loco,
                     &layout::Locomotive::destroyed,
                      this,
                     &AutomationItem::destroyed);

            connect (&loco,
                     &layout::Locomotive::functionDeleted,
                      this,
                     [filterNum = num, this] (uint8_t num)
                     {
                     if (filterNum == num)
                         {
                         emit AutomationItem::destroyed ();
                         }
                     });
            break;
            }
        }
    }

void AutomationItem::setState (bool state)
    {
    switch (getType ())
        {
        case type::ACTUATOR:
            {
            getActuator ()->set (state);
            break;
            }
        case type::ROUTE:
            {
            getRoute ()->set ();
            break;
            }
        case type::LOCO_FUNC:
            {
            auto& [loco, func] = std::get<2> (m_item);

            loco.setFunc (func, state);
            break;
            }
        }
    }

bool AutomationItem::getState () const
    {
    bool state = false;

    switch (getType ())
        {
        case type::ACTUATOR:
            {
            state = getActuator ()->get ();
            break;
            }
        case type::ROUTE:
            {
            break;
            }
        case type::LOCO_FUNC:
            {
            auto info = getFunctionInfo ();

            if (info.has_value ())
                {
                state = info->state;
                }
            break;
            }
        }
    return state;
    }


} // namespace control
