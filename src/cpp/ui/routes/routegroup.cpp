/**
 * @file        routes/routegroup.hpp
 * @brief       Group box triggering routes. One per controller
 * @author      Justin Scott
 * @date        2026-03-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <QBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QTimer>

#include <ui/common/autogrid.hpp>
#include <ui/common/seperator.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>

#include <ui/routes/editroute.hpp>
#include <ui/routes/routebutton.hpp>
#include <ui/routes/routegroup.hpp>


namespace ui::routes
{


RouteGroup::RouteGroup (control::ControllerBase& controller, QWidget* parent) :
    QGroupBox (controller.getFriendlyName ().c_str (), parent),
    m_controller (&controller)
    {
    QVBoxLayout*    layout      = new QVBoxLayout{};
    QHBoxLayout*    addLayout   = new QHBoxLayout{};

    m_gridLayout = new common::AutoGridLayout
        {
        common::AutoGridLayout::expand::ROW_FIRST,
        common::AutoGridLayout::EXPAND,
        NULL
        };

    for (const layout::Route& route : m_controller->getRoutes ())
        {
        addRouteToGrid (route);
        }

    m_gridLayout->setAlignment (Qt::AlignTop | Qt::AlignLeft);

    common::PointedButton* addBtn = new common::PointedButton{ QIcon{ ":/icons/misc/plus.svg" }, "", this };

    common::makeFrameless (*addBtn);

    addLayout->addWidget (addBtn, 0, Qt::AlignLeft);
    addLayout->addWidget (new QLabel{ "Add Route", this }, 0, Qt::AlignLeft);
    addLayout->setAlignment (Qt::AlignLeft);

    layout->addItem (m_gridLayout);
    layout->addWidget (new common::Separator{ this });
    layout->addLayout (addLayout);

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);

    connect (addBtn,
            &QPushButton::released,
             this,
            &RouteGroup::addRoute);

    setLayout (layout);
    }

void RouteGroup::addRouteToGrid (const layout::Route& route)
    {
    RouteButton* btn = new RouteButton{ route, this };

    m_gridLayout->addWidget (btn);

    connect (btn,
            &RouteButton::routeDeleted,
             this,
            &RouteGroup::removeRoute);
    }

void RouteGroup::removeRoute ()
    {
    RouteButton*    btn         = static_cast<RouteButton*> (QObject::sender ());
    QRect           geometry    = m_gridLayout->geometry ();

    m_gridLayout->removeWidget (btn);

    delete btn;

    // Force a refresh
    m_gridLayout->setGeometry (geometry);
    }

void RouteGroup::addRoute ()
    {
    layout::Route   route;

    // Dialog box scope, it existing when we try to add the new
    // route interferes with the layout
    {
    EditRouteDialog dlg{ *m_controller, this };

    if (QDialog::Accepted == dlg.exec ())
        {
        route = m_controller->createRoute (dlg.getName (),
                                           dlg.getActuators ());
        }
    }

    if (route)
        {
        addRouteToGrid (route);

        m_gridLayout->itemAt (m_gridLayout->count () - 1)->widget ()->show ();

        // Force a refresh
        m_gridLayout->setGeometry (m_gridLayout->geometry ());
        }
    }


}
