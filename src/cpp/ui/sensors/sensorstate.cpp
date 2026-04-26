/**
 * @file        sensors/sensorstate.cpp
 * @brief       Widget to display the state of sensors
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/stringdialog.hpp>

#include <ui/sensors/sensorstate.hpp>

#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QWidgetAction>

namespace ui::sensors
{
SensorState::SensorState (uint radius, bool state, int idx, QWidget* parent) :
    QWidget (parent),
    m_radius (radius),
    m_state (state),
    m_notified (false),
    m_idx (idx)
    {
    setFixedSize (radius * 2 + padding,
                  radius * 2 + padding);

    setToolTip (getName ());
    setContextMenuPolicy (Qt::CustomContextMenu);

    connect (this,
            &QWidget::customContextMenuRequested,
             this,
            &SensorState::openMenu);
    }

void SensorState::paintEvent (QPaintEvent* event)
    {
    QPainter        painter{ this };
    Qt::GlobalColor color   = m_state       ? Qt::green : Qt::red;
    Qt::GlobalColor outline = m_notified    ? Qt::blue  : Qt::black;
    QPoint          center  = rect ().center ();

    // ... Why do I need to do this?
    center.rx () += 1;
    center.ry () += 1;

    painter.setRenderHint (QPainter::Antialiasing, true);
    painter.setPen (QPen{ QBrush { outline }, m_notified ? 2.0 : 1.0 });
    painter.setBrush (color);

    painter.drawEllipse (center, m_radius + 1, m_radius + 1);
    }

void SensorState::openMenu (QPoint pos)
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

    notify = menu->addAction ("Notify");
    rename = menu->addAction ("Rename");

    notify->setCheckable (true);
    notify->setChecked (m_notified);

    connect (notify,
            &QAction::toggled,
             this,
            &SensorState::setNotified);

    connect (rename,
            &QAction::triggered,
             this,
            &SensorState::rename);

    // TODO: I hate the checkmark style but adding this inexplicably adds a frame to the unchecked
    // version (but not the unchecked version)
    // I have tried adde border: none; background: transparent;
    // It doesn't seem to care
    //
#if 0
    menu->setStyleSheet (menu->styleSheet () +
                         "QMenu::indicator:checked{  image: url(:/icons/misc/dot.svg); }"
                         "QMenu::indicator:unchecked{ image: url(:/icons/misc/empty-dot.svg); }");
#endif
    menu->popup (mapToGlobal (pos));
    }

void SensorState::rename (bool checked)
    {
    common::StringDialog dlg{ this };

    dlg.setWindowTitle ("Rename Port");
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

void SensorState::setNotified (bool notified)
    {
    m_notified = notified;

    // Redraw
    update ();
    }

}
