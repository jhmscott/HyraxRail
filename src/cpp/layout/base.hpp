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


#include <common.hpp>

#include <QObject>

#include <set>

namespace layout
{
// Forward declare
template<class Component>
class ControllerBase;


///////////////////////////////////////////////////////////////////////////////
/// Component polymorphic base class. Provides signal when it's controller is destroyed
///
///////////////////////////////////////////////////////////////////////////////
class ComponentBase : public QObject
    {
    Q_OBJECT
signals:

    ///////////////////////////////////////////////////////////////////////////////
    /// Signals that this component's controller is being destroyed, and this component should be removed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyed ();
    };


///////////////////////////////////////////////////////////////////////////////
/// Component base class template implementation. Each component is identified by an ID that is unqiue for
/// that component type and controller. This ID is passed from the componebtto the controller throuh the
/// controller interface
///
/// @tparam     Controller type. Must inherit from ControllerBase
///
///////////////////////////////////////////////////////////////////////////////
template<class Controller>
class ComponentDerived : public ComponentBase
    {
public:
    using controller_t  = typename Controller;

    // TODO: get this compiler check working
#if 0
    static_assert (std::is_base_of_v<ControllerBase<typename Controller::component_t>, Controller>,
                   "Controller must derive from ControllerBase");
#endif

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller          Controller controlling this component
    /// @param[in]  id                            Component unique ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ComponentDerived (Controller* controller, size_t id) :
        m_controller (controller),
        m_id (id)
        { register_ (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Default constructor. Creates a component with no control
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ComponentDerived () = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy constructor
    ///
    /// @param[in]  other           Component to copy
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ComponentDerived (const ComponentDerived& other) :
        ComponentDerived (other.m_controller, other.m_id)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Copy assignment operator
    ///
    /// @param[in]  other           Component to copy
    ///
    /// @return     Reference to this
    ///
    ///////////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////////
    /// Move constructor
    ///
    /// @param[in,out]  other           Component to move
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ComponentDerived (ComponentDerived&& other) :
        m_controller (other.m_controller),
        m_id (other.m_id)
        {
        other.deregister ();
        register_ ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Moveassignment operator
    ///
    /// @param[in]  other           Component to move
    ///
    /// @return     Reference to this
    ///
    ///////////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor. Deregisters this with the controller so it no longer recieves destory signals
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~ComponentDerived ()
        { deregister (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Called by the controller when it is about to be destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroy ()
        {
        emit destroyed ();
        m_controller = NULL;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Comparison operator. Evaluates to true if these reference the same object on the same controller
    ///
    /// @param[in]  other           Component to compare to
    ///
    /// @return     True if this and other are the same component
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool operator== (const ComponentDerived& other) const
        {
        return other.m_controller   == m_controller &&
               other.m_id           == m_id;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the ID used to identify this component to the controller
    ///
    /// @return     Component unique ID
    ///
    ///////////////////////////////////////////////////////////////////////////////
    size_t getId () const { return m_id; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the controller controlling this locomotive
    ///
    /// @return     Locomotive controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Controller* getController () { return m_controller; }


    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this a valid component
    ///
    /// @return     false if not a valid component
    ///
    ///////////////////////////////////////////////////////////////////////////////
    operator bool () const { return 0 != m_id; }

protected:
    Controller* m_controller    = NULL;     ///< Non owning controller reference
    size_t      m_id            = 0;        ///< Unique ID

    ///////////////////////////////////////////////////////////////////////////////
    /// Set a member variable for all instances of this object with the same ID
    ///
    /// @tparam     T           Member variable type
    /// @tparam     ComponentT  Component type
    ///
    /// @param[in]  member      Member variable pointer
    /// @param[in]  value       New value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class T, class ComponentT>
    void setAll (T ComponentT::* member, const identityType<T>& value)
        {  m_controller->setAll (m_id, member, value); }

    ///////////////////////////////////////////////////////////////////////////////
    /// De-register this with the controller, so it no longer receives the destroyed signal
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void deregister ()
        {
        if (NULL != m_controller)
            {
            m_controller->deregisterComponent (this);
            m_controller = NULL;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Register this with the controller, so it recieves the signal when the controller is destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void register_ ()
        {
        if (NULL != m_controller)
            {
            m_controller->registerComponent (this);
            }
        }
    };


///////////////////////////////////////////////////////////////////////////////
///  Base class for layout component controllers
///
/// @tparam     Component           Component this controls
///
///////////////////////////////////////////////////////////////////////////////
template<class Component>
class ControllerBase
    {
public:
    using component_t       = typename Component;
    using componentBase_t   = typename ComponentDerived<typename component_t::controller_t>;

    friend class componentBase_t;

    static_assert (std::is_base_of_v<componentBase_t, Component>,
                   "Component must derive from ComponentBase");

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor. Signals to all its components it is being destroyed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~ControllerBase ()
        {
        for (Component* component : m_components)
            {
            component->destroy ();
            }
        }

protected:
    std::set<Component*> m_components;  ///< Components under control of this controller

private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Set a member variable for all instances of a given object
    ///
    /// @tparam     Member variable type
    ///
    /// @param[in]  id      ID of object to set
    /// @param[in]  member  Member variable pointer
    /// @param[in]  value   Value to set member to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class T>
    void setAll (size_t id, T Component::* member, const identityType<T>& value)
        {
        for (Component* component : m_components)
            {
            if (component->getId () == id)
                {
                component->*member = value;
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Register a component
    ///
    /// @tparam     T       Component base type
    ///
    /// @param[in]  component   Component being registered
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class T>
    void registerComponent (T* component)
        {
        m_components.emplace (static_cast<component_t*> (component));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// De-register a component
    ///
    /// @tparam     T       Component base type
    ///
    /// @param[in]  component   Component being de-registered
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class T>
    void deregisterComponent (T* component)
        {
        m_components.erase (static_cast<component_t*> (component));
        }
    };


} // namespace layput
