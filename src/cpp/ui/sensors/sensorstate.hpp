/**
 * @file        sensors/sensorstate.hpp
 * @brief       Widget to display the state of sensors
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QWidget>

namespace ui::sensors
{
class SensorState : public QWidget
    {
    Q_OBJECT
public:
    SensorState (uint radius, bool state, int idx, QWidget* parent);

    void setNotified (bool notified);

    bool getNotified () { return m_notified; }

    void setName (const QString& name) { m_name = name; }

    QString getName () const
        { return hasCustomName () ? m_name : getPortName (); }

    bool hasCustomName () const { return not m_name.isEmpty (); }
protected:
    virtual void paintEvent (QPaintEvent* event) override;
private:
    uint m_radius;
    bool m_state;
    bool m_notified;
    int  m_idx;
    QString m_name;

    QString getPortName () const { return QString::asprintf ("Port %d", m_idx);  }

    void openMenu (QPoint pos);

    void rename (bool checked);

    static constexpr uint padding = 4;
    };

}