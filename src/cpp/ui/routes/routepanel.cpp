/**
 * @file        routes/routepanel.cpp
 * @brief       Panel for the routes tab of the main widget
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <utils/algorithm.hpp>

#include <ui/common/utils.hpp>

#include <ui/routes/routegroup.hpp>
#include <ui/routes/routepanel.hpp>



namespace ui::routes
{

RoutePanel::RoutePanel (control::ControllerManager& controllers,
                        control::AutomationManager& automations,
                        QWidget*                    parent) :
    QWidget (parent),
    m_controllers (controllers),
    m_automations (automations)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    for (control::ControllerBase& controller : controllers)
        {
        layout->addWidget (new RouteGroup{ controller, automations, this });
        }

    layout->setAlignment (Qt::AlignTop);

    connect (&m_controllers,
             &control::ControllerManager::controllerAdded,
              this,
             &RoutePanel::addController);

    connect (&m_controllers,
             &control::ControllerManager::controllerDeleted,
              this,
             &RoutePanel::remove);

    setLayout (layout);
    }

void RoutePanel::remove (const control::ControllerBase& controller)
    {
    ptrdiff_t idx = m_controllers.indexOf (controller);

    if (idx >= 0)
        {
        common::removeWidgetFromLayout (*layout (), idx);
        }
    }

void RoutePanel::addController (control::ControllerBase& controller)
    {
    QVBoxLayout* myLayout = static_cast<QVBoxLayout*> (layout ());

    myLayout->addWidget (new RouteGroup{ controller, m_automations, this });
    }
}
