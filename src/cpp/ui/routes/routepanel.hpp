/**
 * @file        routes/routepanel.hpp
 * @brief       Panel for the routes tab of the main widget
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QPushButton>
#include <QWidget>

#include <control/controllers/base.hpp>

#include <ui/common/autogrid.hpp>

namespace ui::routes
{

class RoutePanel : public QWidget
    {
public:
    RoutePanel (control::controllerList& controllers, QWidget* parent);

    void remove (const control::ControllerBase& controller);

    void add (control::ControllerBase& controller);

private:
    control::controllerList m_controllers;

    };

}