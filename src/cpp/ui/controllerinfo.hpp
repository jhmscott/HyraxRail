/**
 * @file        controllerinfo.hpp
 * @brief       Widget to display the state of a controller
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/icontoggle.hpp>
#include <ui/lang.hpp>

namespace ui
{

///////////////////////////////////////////////////////////////////////////////
/// Controller info widget. DIsplays information about a given controller and provides an interface for
/// changing it's settings
///
///////////////////////////////////////////////////////////////////////////////
class ControllerInfo : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller      Controller to display
    /// @param[in]  parent          Parent widget
    /// @param[in]  includeDelete   If true, include the delete button
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerInfo (control::ControllerManager& manager,
                    control::ControllerBase*    controller,
                    QWidget*                    parent,
                    bool                        includeDelete);

    ///////////////////////////////////////////////////////////////////////////////
    /// Clear the controller info from the widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clear ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the controller to display
    ///
    /// @param[in]  controller          Controller to display
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setController (control::ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the controller this is managing
    ///
    /// @return     Controller instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    control::ControllerBase* getController () { return m_controller; }

private:
    control::ControllerManager&     m_manager;
    control::ControllerBase*        m_controller;       ///< Controller being displayed
    layout::EmergencyStopController*m_stopper;          ///< Stop controller
    common::SchemeIconButton*       m_connectionIcon;   ///< Connection status icon
    QPushButton*                    m_stop;             ///< Emergency/global stop button
    common::SchemeIconButton*       m_settings;         ///< Settings button. Opens the config dialog
    QLabel*                         m_nameLabel;        ///< Controller friendly name label
    QPushButton*                    m_deleteBtn = NULL;
    common::SchemeIconButton*       m_refresh;

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the toggle of the emergency stop button. If there is no emergency stop present, this triggers an
    /// emergency stop. Otherwise this clears the emergency stop state (also called the "go" state on some
    /// controllers
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void eStop () { m_stopper->eStop (m_stop->isChecked ()); updateStopTooltips (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the controller health icon
    ///
    /// @param[in]  health          New health value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setHealth (control::ConnectionWorkerThread::health health);

    ///////////////////////////////////////////////////////////////////////////////
    /// Called when the config button is pressed. Opens the controller config dialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onConfig ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Called when the delete button is pressed. Prompts the user to confirm the deletion, before signalling the
    /// parent
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onDelete ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the health icon to the current connection health
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshHealthIcon (const control::ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set this to display no controller configured
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setNoControllerConfigured ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the start of a refresh operation
    ///
    /// @param[in]  controller      Controller being refreshed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshStarted (const control::ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the completion of a refresh, with a newly rebuilt cache
    ///
    /// @param[in]  controller      Controller refreshed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshComplete (const control::ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Trigger a manual refresh of the controller cache
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refresh ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the tooltips
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTooltips ();

    void updateStopTooltips ();

    UILANG_ON_CHANGE (QWidget,
        {
        if (NULL != m_controller)
            {
            refreshHealthIcon (*m_controller);
            setController (*m_controller);
            }
        else
            {
            setNoControllerConfigured ();
            }

        setTooltips ();
        })
    };

}
