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
                          control::ControllerBase&      controller,
                          control::AutomationManager&   automations,
                          QWidget*                      parent) :
    ComponentButton (&automations, route, true, parent),
    m_controller (controller)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };
    QPushButton* button = new common::PointedIconButton{ "misc/path", this };

    m_component.request ();

    button->setFixedSize (utils::resources::PUSHBUTTON_SIZE);
    button->setIconSize (utils::resources::ICON_SIZE_LARGE);
    button->setStyleSheet ("QPushButton:pressed { background-color: royalblue; }");

    common::refreshStyleSheetOnColorSchemeChange (*button);

    layout->addWidget (button);
    layout->addWidget (m_name = new QLabel{ m_component.getName ().c_str (), this });

    setContentsMargins (0, 20, 0, 0);

    connect (button,
            &QPushButton::released,
            &m_component,
            &layout::Route::set);

    updateTooltip ();
    setLayout (layout);
    }

void RouteButton::updateTooltip ()
    {
    QString text = tr ("Actuators") + ":\n";

    for (const auto& [actuator, state] : m_component.getActuators ())
        {
        text += QString{ "  %1 : %2\n" }.arg (actuator.getName (),
                                              utils::str::formatOnOff (state));
        }

    text.removeLast ();

    setToolTip (text);
    }


void RouteButton::editComponent ()
    {
    EditRouteDialog dlg
        {
        m_controller,
        this,
       &m_component
        };

    if (QDialog::Accepted == dlg.exec ())
        {
        layout::routeList   newMembers  = dlg.getActuators ();
        std::string         newName     = dlg.getName ();

        if (m_component.getActuators () != newMembers)
            {
            m_component.setActuators (newMembers);
            updateTooltip ();
            }

        if (m_component.getName () != newName)
            {
            m_component.setName (newName);
            m_name->setText (newName.c_str ());
            }
        }
    }


} // namespace ui::routes
