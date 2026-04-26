/**
 * @file        common/hovericon.hpp
 * @brief       Frameless pushbutton with an icon, that animates with a
 *              shadow effect on mouse hovers
 * @author      Justin Scott
 * @date        2026-04-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/hovericon.hpp>

#include <ui/common/utils.hpp>

#include <QPainter>
#include <QPaintEvent>

namespace ui::common
{

namespace constants
{
constexpr uint MAX_OPACITY          = 128;  ///< Final opacity value
constexpr uint ANIMATION_DURATION   = 250;  ///< In milseconds
}

HoverIcon::HoverIcon (const QIcon& icon, const QColor& hoverColor, style style, QWidget* parent) :
    PointedButton (icon, "", parent),
    m_anim (new QPropertyAnimation{ this, "opacity", this }),
    m_hoverColor (hoverColor),
    m_style (style)
    {
    common::makeFrameless (*this);
    }

void HoverIcon::paintEvent (QPaintEvent* event)
    {
    QStyleOptionButton styles;

    initStyleOption (&styles);

    if (m_opacity > 0)
        {
        int         diameter = qMin (height (), width ());
        QPainter    painter{ this };
        QColor      color = m_hoverColor;

        color.setAlpha (m_opacity);

        painter.setRenderHint (QPainter::Antialiasing, true);

        painter.translate (width ()  / 2,
                           height () / 2);

        painter.setPen (Qt::NoPen);
        painter.setBrush (QBrush{ color });
        QRect rect
            {
            -diameter / 2,
            -diameter / 2,
             diameter,
             diameter
            };
        switch (m_style)
            {
            case style::CIRCLE:
                {
                painter.drawEllipse (rect);
                break;
                }
            case style::ROUNDED_RECT:
                {
                painter.drawRoundedRect (rect, 2, 2);
                break;
                }
            }

        }

    PointedButton::paintEvent (event);
    }

void HoverIcon::enterEvent (QEnterEvent* event)
    {
    PointedButton::enterEvent (event);

    m_anim->setStartValue (m_opacity);
    m_anim->setEndValue (constants::MAX_OPACITY);

    m_anim->setDuration (constants::ANIMATION_DURATION *
                        (constants::MAX_OPACITY - m_opacity) /
                         constants::MAX_OPACITY);
    m_anim->start ();
    }


void HoverIcon::leaveEvent (QEvent* event)
    {
    PointedButton::leaveEvent (event);

    m_anim->setStartValue (m_opacity);
    m_anim->setEndValue (0);

    m_anim->setDuration (constants::ANIMATION_DURATION * m_opacity /
                         constants::MAX_OPACITY);
    m_anim->start ();
    }
}
