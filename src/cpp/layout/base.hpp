/**
 * @file        layout/base.hpp
 * @brief       Base class for layout components and their
 *              associated controller
 * @author      Justin Scott
 * @date        2026-03-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QObject>

#include <set>

namespace layout
{
template<class Component>
class ControllerBase;

class ComponentBase : public QObject
    {
    Q_OBJECT
signals:
    void destroyed ();
    };


template<class Controller>
class ComponentDerived : public ComponentBase
    {
public:
    using controller_t  = typename Controller;

#if 0
    static_assert (std::is_base_of_v<ControllerBase<typename Controller::component_t>, Controller>,
                   "Controller must derive from ControllerBase");
#endif

    ComponentDerived (Controller* controller, size_t id) :
        m_controller (controller),
        m_id (id)
        { register_ (); }

    ComponentDerived () :
        ComponentDerived (NULL, 0)
        {}

    ComponentDerived (const ComponentDerived& other) :
        ComponentDerived (other.m_controller, other.m_id)
        {}

    ComponentDerived& operator= (const ComponentDerived& other)
        {
        if (this != &other)
            {
            deregister ();

            m_controller    = other.m_controller;
            m_id            = other.m_id;

            register_ ();
            }

        return *this;
        }

    ComponentDerived (ComponentDerived&& other) :
        m_controller (other.m_controller),
        m_id (other.m_id)
        {
        other.deregister ();
        register_ ();
        }

    ComponentDerived& operator= (ComponentDerived&& other)
        {
        if (this != &other)
            {
            deregister ();

            m_controller    = other.m_controller;
            m_id            = other.m_id;

            other.deregister ();
            register_ ();
            }

        return *this;
        }

    ~ComponentDerived ()
        { deregister (); }

    void destroy ()
        {
        emit destroyed ();
        m_controller = NULL;
        }

    bool operator== (const ComponentDerived& other) const
        {
        return other.m_controller   == m_controller &&
               other.m_id           == m_id;
        }

protected:
    Controller* m_controller;
    size_t      m_id = 0;

    void deregister ()
        {
        if (NULL != m_controller)
            {
            m_controller->deregisterComponent (this);
            m_controller = NULL;
            }
        }

    void register_ ()
        {
        if (NULL != m_controller)
            {
            m_controller->registerComponent (this);
            }
        }
    };


template<class Component>
class ControllerBase
    {
public:
    using component_t       = typename Component;
    using componentBase_t   = typename ComponentDerived<typename component_t::controller_t>;

    friend class componentBase_t;

    static_assert (std::is_base_of_v<componentBase_t, Component>,
                   "Component must derive from ComponentBase");

    ~ControllerBase ()
        {
        for (Component* component : m_components)
            {
            component->destroy ();
            }
        }
protected:
    std::set<Component*> m_components;

private:
    template<class T>
    void registerComponent (T* component)
        {
        m_components.emplace (static_cast<component_t*> (component));
        }

    template<class T>
    void deregisterComponent (T* component)
        {
        m_components.erase (static_cast<component_t*> (component));
        }
    };


}