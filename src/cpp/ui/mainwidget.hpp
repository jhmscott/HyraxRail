/**
 * @file        mainwidget.hpp
 * @brief       Top level widget
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/actuators/actuatorpanel.hpp>
#include <ui/common/schemeicon.hpp>
#include <ui/config/configpanel.hpp>
#include <ui/lang.hpp>
#include <ui/routes/routepanel.hpp>
#include <ui/trains/dualcontrol.hpp>

#include <QSettings>
#include <QTabWidget>

namespace ui
{

///////////////////////////////////////////////////////////////////////////////
/// Main widget, which comprises the client area of the main widget
///
///////////////////////////////////////////////////////////////////////////////
class MainWidget : public common::SchemeTabWidget
    {
    Q_OBJECT;
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent          Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    MainWidget (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~MainWidget ();
private:
    control::ControllerManager*     m_controllers;  ///< List of controllers
    control::AutomationManager*     m_automations;  ///< List of automations

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the tab tooltips
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTooltips ();

    UILANG_ON_CHANGE (common::SchemeTabWidget, setTooltips ());
    };

} // namespace ui
