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

#include <ui/common/icontoggle.hpp>

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
    /// @param[in]  controller          Controller to display
    /// @param[in]  parent                   Parent widget
    /// @param[in]  includeDelete   If true, include the delete button
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ControllerInfo (control::ControllerBase* controller, QWidget* parent, bool includeDelete);
        
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

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signal to indicate the "delete" button had been pressed
    ///
    /// @param[in]  controller          Controller the delte was requested for
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void deleted (control::ControllerBase& controller);
        
private:
    control::ControllerBase*    m_controller;       ///< Controller being displayed
    QPushButton*                m_connectionIcon;   ///< Connection status icon
    common::IconToggle*         m_stop;             ///< Emergency/global stop button
    QPushButton*                m_settings;         ///< Settinsg button. Opens the config dialog
    QLabel*                     m_nameLabel;        ///< Controller friendly name label

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the health icon to the current connection health
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshHealthIcon ()
        {
        setHealth (NULL == m_controller ?
                   control::ConnectionWorkerThread::health
                   {
                        control::ConnectionWorkerThread::HEALTH_UNAVAILABLE,
                        std::chrono::milliseconds { 0 }
                    } :
                    m_controller->getConnctionHealth ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the toggle of the emergency stop button. If there is no emergency stop present, this triggers an
    /// emergency stop. Otherwise this clears the emergency stop state (also called the "go" state on some
    /// controllers
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void eStop () { m_controller->eStop (m_stop->isChecked ()); }
        
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
    };

}
