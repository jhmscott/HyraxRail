/**
 * @file        sensors/managestate.cpp
 * @brief       Widget to display and manage the state of a sensor port
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/stringdialog.hpp>

#include <ui/sensors/managestate.hpp>

#include <QLabel>
#include <QMenu>
#include <QWidgetAction>

namespace ui::sensors
{
SensorManageState::SensorManageState (const layout::SensorModule&   module,
                                      size_t                        idx,
                                      uint                          radius,
                                      QWidget*                      parent) :
    common::HoldToOpenMenu<SensorDisplayState> (module, idx, radius, parent)
    {
    setToolTip (getName ());
    setContextMenuPolicy (Qt::CustomContextMenu);

    connect (this,
            &QWidget::customContextMenuRequested,
             this,
            &SensorManageState::openMenu);
    }

void SensorManageState::openMenu (const QPoint& pos)
    {
    QMenu*          menu    = new QMenu{ this };
    QAction*        notify;
    QAction*        rename;
    QWidgetAction*  header  = new QWidgetAction{ menu };
    QLabel*         label   = new QLabel{ getName (), menu };

    label->setAlignment (Qt::AlignHCenter);
    label->setMargin (5);

    header->setDefaultWidget (label);

    menu->addAction (header);
    menu->addSeparator ();

    notify = menu->addAction (tr ("Notify"));
    rename = menu->addAction (tr ("Rename"));

    notify->setCheckable (true);
    notify->setChecked (getNotified ());

    connect (notify,
            &QAction::toggled,
             this,
            &SensorManageState::setNotified);

    connect (rename,
            &QAction::triggered,
             this,
            &SensorManageState::rename);

    // @TODO: I hate the checkmark style but adding this inexplicably adds a frame to the unchecked
    // version (but not the unchecked version)
    // I have tried adding border: none; background: transparent;
    // It doesn't seem to care
    //
#if 0
    menu->setStyleSheet (menu->styleSheet () +
                         "QMenu::indicator:checked{  image: url(:/icons/misc/dot.svg); }"
                         "QMenu::indicator:unchecked{ image: url(:/icons/misc/empty-dot.svg); }");
#endif

    menu->popup (mapToGlobal (pos));
    }

void SensorManageState::rename ()
    {
    common::StringDialog dlg{ this };

    dlg.setWindowTitle (tr ("Rename Port"));
    dlg.setPlaceholder (getPortName ());

    if (hasCustomName ())
        {
        dlg.setText (m_name);
        }

    if (QDialog::Accepted == dlg.exec ())
        {
        setName (dlg.getText ());
        }
    }

} // namespace ui::sensors
