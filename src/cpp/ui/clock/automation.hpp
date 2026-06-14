/**
 * @file        clock/automation.hpp
 * @brief       List of configured automations
 * @author      Justin Scott
 * @date        2026-06-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/addbtn.hpp>
#include <ui/lang.hpp>

#include <QBoxLayout>
#include <QGroupBox>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Group box containing list of configured automations with options to create
/// update and delete
///
///////////////////////////////////////////////////////////////////////////////
class AutomationGroup : public QGroupBox
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  automations     List of automations
    /// @param[in]  parent          Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    AutomationGroup (control::ControllerManager&    controllers,
                     control::AutomationManager&    automations,
                     QWidget*                       parent);

private:
    control::AutomationManager& m_manager;      ///< List of automations
    control::ControllerManager& m_controllers;  ///< List of controllers
    common::AddButton*          m_addBtn;       ///< Button to add a new automation
    QVBoxLayout*                m_layout;       ///< Layout containing list of automations

    ///////////////////////////////////////////////////////////////////////////////
    /// Add a widget for a task
    ///
    /// @param[in]  task        Task to create widget for
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addTask (control::AutomationTask& task);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set all the labels and titles
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTitles ();

    UILANG_ON_CHANGE (QGroupBox, setTitles ());

private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Open a form dialog to create an automation
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addAutomation ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Remove a task from the UI
    ///
    /// @param[in]  task        Task to remove
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeTask (const control::AutomationTask& task);

    };
} // namespace ui::clock