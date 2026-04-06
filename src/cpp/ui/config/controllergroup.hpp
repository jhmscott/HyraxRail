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

#include <QGroupBox>
#include <QWidget>

namespace ui::config
{
class ControllerGroup : public QGroupBox
    {
    Q_OBJECT
public:
    ControllerGroup (const std::vector<control::ControllerBase*>&   controllers,
                     QWidget*                                       parent);


    void addControllerInfo (control::ControllerBase& controller);

signals:
    void controllerDeleted (control::ControllerBase& controller);

    void controllerAdded (const control::createControllerInfo& controller);

private:
    std::vector<control::ControllerBase*>   m_controllers;
    bool                                    m_hasAddController = false;
    void addController ();

    void onControllerDeleted (control::ControllerBase& controller);

    };

}