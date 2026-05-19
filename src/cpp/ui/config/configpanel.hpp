/**
 * @file        config/configpanel.hpp
 * @brief       Panel for the settings tab of the main window
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/config/controllergroup.hpp>

#include <QWidget>

namespace ui::config
{

//////////////////////////////////////////////////////////////////////////////
/// App configuration page
///
//////////////////////////////////////////////////////////////////////////////
class ConfigPanel : public QWidget
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ConfigPanel (control::ControllerManager* controllers, QWidget* parent);

    };

} // ui::config