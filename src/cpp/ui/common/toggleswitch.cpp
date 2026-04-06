/**
 * @file        common/toggleswitch.cpp
 * @brief       Pill style toggle switch
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/toggleswitch.hpp>

#include <qevent.h>
#include <QPainter>

namespace ui::common
{
ToggleSwitch::ToggleSwitch (QWidget* parent) :
    QAbstractButton (parent),
    m_height (16),
    m_opacity (0.000),
    m_switch (false),
    m_margin (3),
    m_thumb ("#d5d5d5"),
    m_anim (new QPropertyAnimation (this, "offset", this))
    {
    m_x     = m_height / 2;
    m_y     = m_height / 2;
    m_brush = QColor ("#009688");
    }


QSize ToggleSwitch::sizeHint () const
    {
    return QSize (2 * (m_height + m_margin),
                  m_height + 2 * m_margin);
    }
void ToggleSwitch::paintEvent (QPaintEvent*)
    {
    QPainter p (this);
    p.setPen (Qt::NoPen);

    if (isEnabled ())
        {
        p.setBrush (m_switch ? m_brush : Qt::black);
        p.setOpacity (m_switch ? 0.5 : 0.38);
        p.setRenderHint (QPainter::Antialiasing, true);
        p.drawRoundedRect (QRect (m_margin, m_margin, width () - 2 * m_margin, height () - 2 * m_margin), 8.0, 8.0);
        p.setBrush (m_thumb);
        p.setOpacity (1.0);
        p.drawEllipse (QRectF (m_x - (m_height / 2), m_y - (m_height / 2), height (), height ()));
        }
    else
        {
        p.setBrush (Qt::black);
        p.setOpacity (0.12);
        p.drawRoundedRect (QRect (m_margin, m_margin, width () - 2 * m_margin, height () - 2 * m_margin), 8.0, 8.0);
        p.setOpacity (1.0);
        p.setBrush (QColor ("#BDBDBD"));
        p.drawEllipse (QRectF (m_x - (m_height / 2), m_y - (m_height / 2), height (), height ()));
        }
    }
void ToggleSwitch::mouseReleaseEvent (QMouseEvent* event)
    {
    if (event->button () & Qt::LeftButton)
        {
        m_switch = m_switch ? false : true;
        m_thumb = m_switch ? m_brush : QBrush ("#d5d5d5");
        if (m_switch)
            {
            m_anim->setStartValue (m_height / 2);
            m_anim->setEndValue (width () - m_height);
            m_anim->setDuration (120);
            m_anim->start ();
            }
        else
            {
            m_anim->setStartValue (m_x);
            m_anim->setEndValue (m_height / 2);
            m_anim->setDuration (120);
            m_anim->start ();
            }
        }
    QAbstractButton::mouseReleaseEvent (event);
    }
void ToggleSwitch::enterEvent (QEnterEvent* event)
    {
    setCursor (Qt::PointingHandCursor);
    QAbstractButton::enterEvent (event);
    }
}