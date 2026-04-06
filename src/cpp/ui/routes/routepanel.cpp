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

RoutePanel::RoutePanel (control::controllerList& controllers, QWidget* parent) :
    QWidget (parent),
    m_controllers (controllers)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    for (control::ControllerBase* controller : controllers)
        {
        layout->addWidget (new RouteGroup{ *controller, this });
        }

    layout->setAlignment (Qt::AlignTop);
    setLayout (layout);
    }

void RoutePanel::remove (const control::ControllerBase& controller)
    {
    auto it = std::find (m_controllers.begin (),
                         m_controllers.end (),
                         &controller);

    if (m_controllers.end () != it)
        {
        common::removeWidgetFromLayout (*layout (),
                                        std::distance (m_controllers.begin (),
                                                       it));
        }

    utils::algorithm::erase (m_controllers, &controller);
    }

void RoutePanel::add (control::ControllerBase& controller)
    {
    QVBoxLayout* myLayout = static_cast<QVBoxLayout*> (layout ());

    m_controllers.push_back (&controller);

    myLayout->addWidget (new RouteGroup{ controller, this });
    }
}
