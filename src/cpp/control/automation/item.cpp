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
                if (type::ACTUATOR == getType ())
                    {
                    getActuator ()->set (true);
                    }
                else
                    {
                    getRoute ()->set ();
                    }
                break;
                }

            case ACTION_UNSET:
                {
                getActuator ()->set (false);
                break;
                }

            case ACTION_TOGGLE:
                {
                auto actuator = *getActuator ();

                actuator.set (not actuator.get ());
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
        }

    return name;
    }

void AutomationItem::connectSignals ()
    {
    switch (getType ())
        {
        case type::ROUTE:
            {
            connect (&std::get<1> (m_item),
                     &layout::Route::destroyed,
                     this,
                     &AutomationItem::destroyed);
            break;
            }

        case type::ACTUATOR:
            {
            connect (&std::get<0> (m_item),
                     &layout::Actuator::destroyed,
                     this,
                     &AutomationItem::destroyed);
            break;
            }
        }
    }


} // namespace control
