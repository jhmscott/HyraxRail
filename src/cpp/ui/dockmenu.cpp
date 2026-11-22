/**
 * @file        dockmenu.cpp
 * @brief       Mac OS Dock Menu
 * @author      Justin Scott
 * @date        2026-11-22
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/clock.hpp>

#include <ui/common/utils.hpp>

#include <ui/dockmenu.hpp>

namespace ui
{
DockMenu::DockMenu (control::ControllerManager& controllers, QWidget* parent) :
    QMenu (parent),
    m_controllers (controllers)
    {
    auto& clock = control::FastClock::instance ();

    addAction ("", this, &DockMenu::refreshAll);
    QAction* action = addAction ("", this, &DockMenu::runFastClock);

    action->setCheckable (true);
    action->setChecked (clock.isRunning ());

    connect (&control::FastClock::instance (),
             &control::FastClock::stopped,
              this,
              std::bind (&QAction::setChecked, action, false));

    connect (&control::FastClock::instance (),
             &control::FastClock::started,
              this,
              std::bind (&QAction::setChecked, action, true));

    setMenuText ();
    }

void DockMenu::setMenuText ()
    {
    common::setMenuItemText (*this, 0, tr ("Refresh Controllers"));
    common::setMenuItemText (*this, 1, tr ("Fast Clock"));
    }

void DockMenu::refreshAll ()
    {
    m_controllers.refreshAll ();
    }

void DockMenu::runFastClock ()
    {
    QAction* menuItem = static_cast<QAction*> (sender ());

    Q_CHECK_PTR (menuItem);

    control::FastClock::instance ().setRunning (menuItem->isChecked ());
    }

} // namespace ui
