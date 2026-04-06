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

#include <ui/config/controllergroup.hpp>

#include <QWidget>

namespace ui::config
{

class ConfigPanel : public QWidget
    {
    Q_OBJECT
public:
    ConfigPanel (const std::vector<control::ControllerBase*>& controllers, QWidget* parent);

    void addController (control::ControllerBase& controller)
        { m_controllers->addControllerInfo (controller); }

signals:
    void controllerDeleted (control::ControllerBase& controller);

    void controllerAdded (const control::createControllerInfo& controller);
private:
    ControllerGroup* m_controllers;
    };

}