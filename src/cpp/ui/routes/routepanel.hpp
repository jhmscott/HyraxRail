/**
 * @file        routes/routepanel.hpp
 * @brief       Panel for the routes tab of the main widget
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/autogrid.hpp>

#include <QPushButton>
#include <QWidget>


namespace ui::routes
{

class RoutePanel : public QWidget
    {
public:
    RoutePanel (control::ControllerManager* controllers, QWidget* parent);


private:
    control::ControllerManager* m_controllers;

private slots:

    void remove (const control::ControllerBase& controller);

    void add (control::ControllerBase& controller);

    };

}