/**
 * @file        layout/button.hpp
 * @brief       Button widget base class for triggering a layout component
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>

#include <ui/common/gesture.hpp>
#include <ui/common/utils.hpp>
#include <ui/lang.hpp>

#include <QMessageBox>

namespace ui::common
{
//////////////////////////////////////////////////////////////////////////////
/// Button widget base class for triggering a layout component
///
//////////////////////////////////////////////////////////////////////////////
class LayoutButton : public HoldToOpenMenu<QWidget>
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  hasMenu     True if this button has the add/delete context menu
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    LayoutButton (bool hasMenu, QWidget* parent) :
        HoldToOpenMenu<QWidget> (parent),
        m_hasMenu (hasMenu)
        {
        if (hasMenu)
            {
            createMenu ();
            setContextMenuPolicy (Qt::ActionsContextMenu);
            }
        }
signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the user has requested to remove this component
    ///
    //////////////////////////////////////////////////////////////////////////////
    void removeRequested ();

private:
    bool m_hasMenu; ///< True if this button has the add/delete context menu

    UILANG_ON_CHANGE (QWidget,
        {
        if (m_hasMenu)
            {
            common::removeAllActions (*this);
            createMenu ();
            }
        })

    //////////////////////////////////////////////////////////////////////////////
    /// Virutal "edit" function. Implement in callers to edit the component
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void editComponent () = 0;

    //////////////////////////////////////////////////////////////////////////////
    /// Virutal "remove" function. Implement in callers to delete the component
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void removeComponent () = 0;

    //////////////////////////////////////////////////////////////////////////////
    /// Create the context menu
    ///
    //////////////////////////////////////////////////////////////////////////////
    void createMenu ()
        {
        addAction (tr ("Delete"), this, &LayoutButton::remove);
        addAction (tr ("Edit"), this, &LayoutButton::edit);
        }

    //////////////////////////////////////////////////////////////////////////////
    /// Context menu item to delete a component
    ///
    //////////////////////////////////////////////////////////////////////////////
    void remove () { removeComponent (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Context menu item to edit a component
    ///
    //////////////////////////////////////////////////////////////////////////////
    void edit () { editComponent (); }
    };


//////////////////////////////////////////////////////////////////////////////
/// Layout button that is used specifically manage a layout::Component
///
/// @tparam     Component       Component this button manages
///
//////////////////////////////////////////////////////////////////////////////
template<class Component>
class ComponentButton : public LayoutButton
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  automations     (optional) Automation manager
    /// @param[in]  component       Component this button represents
    /// @param[in]  hasMenu         True if this button has the add/delete context menu
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ComponentButton (const control::AutomationManager*  automations,
                     const Component&                   component,
                     bool                               hasMenu,
                     QWidget*                           parent) :
        LayoutButton (hasMenu, parent),
        m_component (component),
        m_automations (automations)
        {}

protected:
    Component m_component; ///< Component this button manages

private:
    const control::AutomationManager* m_automations;  ///< List of automations

    //////////////////////////////////////////////////////////////////////////////
    /// Get the translated text for the delete dialog title
    ///
    /// @return     Delete dialog title
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual QString getDeleteText () const = 0;

    //////////////////////////////////////////////////////////////////////////////
    /// Prompt the user to remove the component
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void removeComponent () override
        {
        QString msg = tr ("Would you like to delete \"%1\"?").
            arg (m_component.getName ().c_str ());

        if (NULL != m_automations)
            {
            auto automations = m_automations->findAutomatedComponents (m_component);

            if (automations.size () > 0)
                {
                msg += "\n\n";
                msg += control::formatAutomationDeletionMessage (automations);
                }
            }

        if (QMessageBox::Yes == QMessageBox::question (this,
                                                       getDeleteText (),
                                                       msg))
            {
            m_component.remove ();
            emit removeRequested ();
            }
        }
    };
} // namespace ui::common
