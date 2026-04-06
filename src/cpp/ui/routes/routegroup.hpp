/**
 * @file        routes/routegroup.hpp
 * @brief       Group box triggering routes. One per controller
 * @author      Justin Scott
 * @date        2026-03-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>

#include <QGroupBox>

namespace ui::routes
{

class RouteGroup : public QGroupBox
    {

public:
    RouteGroup (control::ControllerBase& controller, QWidget* parent);

    control::ControllerBase* getController () { return m_controller; }

private:
    control::ControllerBase* m_controller;

    };

}