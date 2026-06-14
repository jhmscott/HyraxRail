/**
 * @file        routes/rouetebutton.cpp
 * @brief       Button to control the state and configuration of a route
 * @author      Justin Scott
 * @date        2026-04-24
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/pointedwidget.hpp>
#include <ui/common/utils.hpp>

#include <ui/routes/editroute.hpp>
#include <ui/routes/routebutton.hpp>

#include <utils/string.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>


namespace ui::routes
{

RouteButton::RouteButton (const layout::Route&          route,
                          control::AutomationManager&   automations,
                          QWidget*                      parent) :
    QWidget (parent),
    m_route (layout::Route{ route }),
    m_automations (automations)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };
    QPushButton* button = new common::PointedIconButton{ "misc/path", this };

    m_route.request ();

    button->setFixedSize (50, 50);
    button->setIconSize (QSize{ 30, 30 });
    button->setStyleSheet ("QPushButton:pressed { background-color: royalblue; }");

    common::refreshStyleSheetOnColorSchemeChange (*button);

    layout->addWidget (button);
    layout->addWidget (m_name = new QLabel{ m_route.getName ().c_str (), this });

    setContentsMargins (0, 20, 0, 0);
    setContextMenuPolicy (Qt::ActionsContextMenu);

    connect (button,
            &QPushButton::released,
            &m_route,
            &layout::Route::set);

    createMenu ();
    updateTooltip ();
    setLayout (layout);
    }

void RouteButton::updateTooltip ()
    {
    QString text = tr ("Actuators") + ":\n";

    for (const auto& [actuator, state] : m_route.getActuators ())
        {
        text += QString{ "  %1 : %2\n" }.arg (actuator.getName (),
                                              utils::str::formatOnOff (state));
        }

    text.removeLast ();

    setToolTip (text);
    }

void RouteButton::createMenu ()
    {
    addAction (tr ("Delete"),this, &RouteButton::removeRoute);
    addAction (tr ("Edit"),  this, &RouteButton::editRoute);
    }

void RouteButton::removeRoute ()
    {
    QString msg = tr ("Would you like to delete route \"%1\"?").arg (
                                            m_route.getName ().c_str ());
    std::vector<std::string> automations;

    for (control::AutomationTask& task : m_automations)
        {
        control::AutomationItem& item = *task.getItem ();

        if (control::AutomationItem::type::ROUTE == item.getType () &&
            m_route == item.getRoute ())
            {
            automations.push_back (item.name ());
            }
        }

    if (automations.size () > 0)
        {
        msg += "\n\n";
        msg += tr ("The following automations will also be deleted:");
        msg += "\n";

        for (const std::string& name : automations)
            {
            msg += QString{ " ● %1" }.arg (name);
            }
        }

    if (QMessageBox::Yes == QMessageBox::question (this,
                                                   tr ("Delete Route"),
                                                   msg))
        {
        m_route.remove ();

        emit routeDeleted ();
        }
    }

void RouteButton::editRoute ()
    {
    EditRouteDialog dlg
        {
        *static_cast<control::ControllerBase*>
                            (m_route.getController ()),
        this,
       &m_route
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        layout::routeList   newMembers  = dlg.getActuators ();
        std::string         newName     = dlg.getName ();

        if (m_route.getActuators () != newMembers)
            {
            m_route.setActuators (newMembers);
            updateTooltip ();
            }

        if (m_route.getName () != newName)
            {
            m_route.setName (newName);
            m_name->setText (newName.c_str ());
            }
        }
    }


} // namespace ui::routes