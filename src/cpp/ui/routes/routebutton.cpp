/**
 * @file        routes/rouetebutton.cpp
 * @brief       Button to control the state and configuration of a route
 * @author      Justin Scott
 * @date        2026-04-24
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/pointedwidget.hpp>

#include <ui/routes/editroute.hpp>
#include <ui/routes/routebutton.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>


namespace ui::routes
{

RouteButton::RouteButton (const layout::Route& route, QWidget* parent) :
    QWidget (parent),
    m_route (new layout::Route{ route })
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };
    QPushButton* button = new common::PointedButton{ QIcon{ ":/icons/misc/path.svg" }, "", this };

    m_route->request ();

    button->setFixedSize (50, 50);
    button->setIconSize (QSize{ 30, 30 });
    button->setStyleSheet ("QPushButton:pressed { background-color: royalblue; }");

    layout->addWidget (button);
    layout->addWidget (m_name = new QLabel{ m_route->getName ().c_str (), this },
                       0,
                       Qt::AlignHCenter);

    setContentsMargins (0, 20, 0, 0);
    setContextMenuPolicy (Qt::CustomContextMenu);

    connect (button,
             &QPushButton::released,
             m_route,
             &layout::Route::set);

    connect (this,
             &QWidget::customContextMenuRequested,
             this,
             &RouteButton::openMenu);

    setLayout (layout);
    }

void RouteButton::removeRoute ()
    {
    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   "Delete Route",
                                                   QString::asprintf ("Would you like to delete %s?",
                                                                      m_route->getName ().c_str ())))
        {
        m_route->remove ();

        emit routeDeleted ();
        }
    }

void RouteButton::editRoute ()
    {
    EditRouteDialog dlg
        {
        *static_cast<control::ControllerBase*>
                            (m_route->getController ()),
        this,
        m_route
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        layout::routeList   newMembers  = dlg.getActuators ();
        std::string         newName     = dlg.getName ();

        if (m_route->getActuators () != newMembers)
            {
            m_route->setActuators (newMembers);
            }

        if (m_route->getName () != newName)
            {
            m_route->setName (newName);
            m_name->setText (newName.c_str ());
            }
        }
    }

void RouteButton::openMenu (const QPoint& point)
    {
    QMenu*      menu = new QMenu{ this };
    QAction*    edit;
    QAction*    deleteAct;

    deleteAct   = menu->addAction ("delete");
    edit        = menu->addAction ("edit");

    connect (deleteAct,
            &QAction::triggered,
             this,
            &RouteButton::removeRoute);

    connect (edit,
            &QAction::triggered,
             this,
            &RouteButton::editRoute);

    menu->popup (mapToGlobal (point));
    }


} // namespace ui::routes