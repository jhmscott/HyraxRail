/**
 * @file        config/configpanel.cpp
 * @brief       Panel for the settings tab of the main window
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/config/configpanel.hpp>
#include <ui/config/controllergroup.hpp>
#include <ui/config/helpgroup.hpp>
#include <ui/config/notifygroup.hpp>

#include <QBoxLayout>

namespace ui::config
{
ConfigPanel::ConfigPanel (control::ControllerManager* controllers, QWidget* parent)
    {
    QVBoxLayout*        layout          = new QVBoxLayout{ this };

    m_controllers = new ControllerGroup{ controllers, this };

    layout->addWidget (m_controllers);
    layout->addWidget (new NotifyGroup{ this });
    layout->addWidget (new HelpGroup{ this });

    layout->setAlignment (Qt::AlignTop);

    setLayout (layout);
    }
}
