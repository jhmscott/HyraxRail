/**
 * @file        config/controllergroup.hpp
 * @brief       Group box for the controller settings
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <QGroupBox>
#include <QWidget>

namespace ui::config
{
class ControllerGroup : public QGroupBox
    {
    Q_OBJECT
public:
    ControllerGroup (control::ControllerManager*    controllers,
                     QWidget*                       parent);

private:
    control::ControllerManager* m_controllers;
    bool                        m_hasAddController = false;

    void addController ();

private slots:
    void onControllerDeleted (const control::ControllerBase& controller);

    void addControllerInfo (control::ControllerBase& controller);
    };

}