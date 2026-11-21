/*
 * @file        mainmenu.cpp
 * @brief       Main window menu bar
 * @author      Justin Scott
 * @date        2026-11-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/clock.hpp>

#include <ui/config/credits.hpp>
#include <ui/config/dialog.hpp>
#include <ui/config/helpgroup.hpp>

#include <ui/mainmenu.hpp>

#include <utils/string.hpp>

namespace ui
{

///////////////////////////////////////////////////////////////////////////////
/// Set the text for a menu item
///
/// @param[in,out]  menu        Menu to set the text for
/// @param[in]      text        Menu text
///
///////////////////////////////////////////////////////////////////////////////
static void setMenuItemText (QMenu& menu, const QString& text)
    {
    menu.menuAction ()->setText (text);
    }

///////////////////////////////////////////////////////////////////////////////
/// Set the menu item text by index
///
/// @param[in,out]  menu        Menu to set the item for
/// @param[in]      item        Index of menu item
/// @param[in]      text        Menu text
///
///////////////////////////////////////////////////////////////////////////////
static void setMenuItemText (QMenu& menu, size_t item, const QString& text)
    {
    menu.actions ()[item]->setText (text);
    }

///////////////////////////////////////////////////////////////////////////////
/// Get the controller for a menu item within a controller submenu
///
/// @param[in]  action      Menu item action
///
/// @return     Controller for that item's menu
///
///////////////////////////////////////////////////////////////////////////////
static control::ControllerBase& controllerFromMenuItem (const QAction* action)
    {
    QMenu*                      menu;
    control::ControllerBase*    controller;

    Q_CHECK_PTR (action);
    Q_CHECK_PTR (menu = static_cast<QMenu*> (action->parent ()));
    Q_CHECK_PTR (action = menu->menuAction ());
    Q_CHECK_PTR (controller = action->data ().value<control::ControllerBase*> ());

    return *controller;
    }

MainMenu::MainMenu (control::ControllerManager& controllers, QWidget* parent) :
    QMenuBar (parent),
    m_controllers (controllers)
    {
    m_controllersMenu   = addMenu ("");
    m_clockMenu         = addMenu ("");
    m_helpMenu          = addMenu ("");

    m_controllersMenu->addSeparator ();
    m_controllersMenu->addAction ("", this, &MainMenu::addController);

    m_fastClock = m_clockMenu->addAction ("", this, &MainMenu::toggleFastClock);
    m_clockMenu->addSeparator ();
    m_clockMenu->addAction ("", this, &MainMenu::addAutomation);

    m_fastClock->setCheckable (true);
    m_fastClock->setChecked (control::FastClock::instance ().isRunning ());

    m_helpMenu->addAction ("", this, &MainMenu::help);
    m_helpMenu->addAction ("", this, &MainMenu::about)->setMenuRole (QAction::AboutRole);
    m_helpMenu->addAction ("", this, &QApplication::aboutQt)->setMenuRole (QAction::AboutQtRole);
    m_helpMenu->addAction ("", this, &MainMenu::license);
    m_helpMenu->addAction ("", this, &MainMenu::credits);
    m_helpMenu->addAction ("", this, &MainMenu::closeRequested)->setMenuRole (QAction::QuitRole);

    for (control::ControllerBase& controller : controllers)
        {
        addControllerMenu (controller);
        }

    setNativeMenuBar (true);
    setMenuText ();

    // Controller manager signals

    connect (&m_controllers,
             &control::ControllerManager::controllerAdded,
              this,
             &MainMenu::addControllerMenu);

    connect (&m_controllers,
             &control::ControllerManager::controllerDeleted,
              this,
             &MainMenu::removeControllerMenu,
              Qt::DirectConnection);

    connect (&m_controllers,
             &control::ControllerManager::disconnected,
              this,
             &MainMenu::updateConnectionState);

    connect (&m_controllers,
             &control::ControllerManager::reconnected,
              this,
             &MainMenu::updateConnectionState);

    // Fast clock signals

    connect (&control::FastClock::instance (),
             &control::FastClock::stopped,
              this,
              std::bind (&QAction::setChecked, m_fastClock, false));

    connect (&control::FastClock::instance (),
             &control::FastClock::started,
              this,
              std::bind (&QAction::setChecked, m_fastClock, true));
    }

void MainMenu::setMenuText ()
    {
    setMenuItemText (*m_controllersMenu,    tr ("Controllers"));
    setMenuItemText (*m_clockMenu,          tr ("Clock"));
    setMenuItemText (*m_helpMenu,           tr ("Help"));

    // Automation/fast clock menu ite,s
    setMenuItemText (*m_clockMenu, 0, tr ("Fast Clock"));
    setMenuItemText (*m_clockMenu, 2, tr ("Add Automation"));

    // Help menu items
    setMenuItemText (*m_helpMenu, 0, tr ("Help"));
    setMenuItemText (*m_helpMenu, 1, tr ("About"));
    setMenuItemText (*m_helpMenu, 2, tr ("About Qt"));
    setMenuItemText (*m_helpMenu, 3, tr ("License Info"));
    setMenuItemText (*m_helpMenu, 4, utils::str::escape (config::CreditsDialog::TITLE ()));
    setMenuItemText (*m_helpMenu, 5, tr ("Close"));

    // Controller menu items
    size_t  idx = 0;
    QMenu*  submenu;
    auto    actions = m_controllersMenu->actions ();

    while (NULL != (submenu = actions[idx]->menu ()))
        {
        setControllerSubMenuText (*submenu);
        ++idx;
        }

    setMenuItemText (*m_controllersMenu,
                     idx + 1, // Skip separator
                     tr ("New Controller"));
    }

void MainMenu::setControllerSubMenuText (QMenu& submenu)
    {
    const QString MENU_TEXTS[] =
        {
        tr ("New Locomoitive"),
        tr ("New Actuator"),
        tr ("New Route"),
            "",
        tr ("New Sensor")
        };
    ASSERT_ARRAY_LENGTH (MENU_TEXTS, control::NUM_CAPABILITIES);

    QAction*    action;
    auto        actions     = submenu.actions ();
    size_t      idx         = 0;

    while (not (action = actions[idx])->isSeparator ())
        {
        auto capability = action->data ().value<control::controllerCapability> ();

        action->setText (MENU_TEXTS[capability]);

        ++idx;
        }

    setMenuItemText (submenu,
                     idx + 1, // Skip separator
                     tr ("Edit Controller"));

    setMenuItemText (submenu,
                     idx + 2,
                     tr ("Delete Controller"));
    }

void MainMenu::addControllerMenu (control::ControllerBase& controller)
    {
    static const std::set<control::controllerCapability> MENULESS_CAPABILITIES =
        {
        control::CAPABILITY_ESTOP
        };

    QMenu*  controllerSubMenu   = m_controllersMenu->addMenu (controller.getFriendlyName ().c_str ());
    bool    isConnected         = controller.getConnectionHealth ().isConnected ();

    controllerSubMenu->menuAction ()->setData (QVariant::fromValue (&controller));

    for (auto capability : utils::algorithm::EnumRange{ control::NUM_CAPABILITIES })
        {
        if (controller.getMetaClass ().hasCapability (capability) &&
            MENULESS_CAPABILITIES.end () == MENULESS_CAPABILITIES.find (capability))
            {
            QAction* action = controllerSubMenu->addAction ("", this, &MainMenu::addComponent);
            action->setData (QVariant::fromValue (capability));
            action->setEnabled (isConnected);
            }
        }

    controllerSubMenu->addSeparator ();
    controllerSubMenu->addAction ("", this, &MainMenu::editController);
    controllerSubMenu->addAction ("", this, &MainMenu::deleteController);

    setControllerSubMenuText (*controllerSubMenu);

    // insert berfore the separator
    for (QAction* action : m_controllersMenu->actions ())
        {
        if (action->isSeparator ())
            {
            m_controllersMenu->insertMenu (action, controllerSubMenu);
            break;
            }
        }
    }

void MainMenu::removeControllerMenu (const control::ControllerBase& controller)
    {
    QAction* action = controllerItem (controller);

    Q_CHECK_PTR (action);

    m_controllersMenu->removeAction (action);
    }

QAction* MainMenu::menuItem ()
    {
#ifdef Q_DEBUG
    return qobject_cast<QAction*> (sender ());
#else
    return static_cast<QAction*> (sender ());
#endif // Q_DEBUG
    }

QAction* MainMenu::controllerItem (const control::ControllerBase& controller)
    {
    auto actions = m_controllersMenu->actions ();

    auto it = std::find_if (actions.begin (),
                            actions.end (),
                            [&controller] (QAction* action) -> bool
        {
        Q_CHECK_PTR (action);
        return action->data ().value<control::ControllerBase*> () == &controller;
        });

    if (actions.end () != it)
        {
        return *it;
        }
    else{
        return NULL;
        }
    }

void MainMenu::updateConnectionState (const control::ControllerBase& controller)
    {
    QAction*    action = controllerItem (controller);
    QMenu*      menu;

    Q_CHECK_PTR (action);
    Q_CHECK_PTR (menu = action->menu ());

    bool isConnected = controller.getConnectionHealth ().isConnected ();

    QAction*    item;
    int         idx = 0;
    auto        actions = menu->actions ();

    while (not (item = actions[idx])->isSeparator ())
        {
        item->setEnabled (isConnected);
        ++idx;
        }
    }

void MainMenu::addController ()
    {
    config::Dialog::create (this, m_controllers);
    }

void MainMenu::editController ()
    {
    QAction* action = menuItem ();

    config::Dialog::edit (this,
                          controllerFromMenuItem (action),
                          m_controllers);
    }

void MainMenu::deleteController ()
    {
    QAction* action = menuItem ();

    config::Dialog::remove (this,
                            controllerFromMenuItem (action),
                            m_controllers);
    }

void MainMenu::addComponent ()
    {
    QAction* action = menuItem ();

    Q_CHECK_PTR (action);

    auto capability = action->data ().value<control::controllerCapability> ();

    emit componentAddRequested (capability, controllerFromMenuItem (action));
    }

void MainMenu::credits ()
    {
    config::CreditsDialog dlg{ this };

    dlg.exec ();
    }

void MainMenu::help ()
    {
    config::HelpGroup::help (this);
    }

void ui::MainMenu::license ()
    {
    config::HelpGroup::license (this);
    }

void MainMenu::about ()
    {
    config::HelpGroup::about (this);
    }

void MainMenu::toggleFastClock ()
    {
    control::FastClock::instance ().setRunning (m_fastClock->isChecked ());
    }

}
