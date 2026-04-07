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
HoverIcon::HoverIcon (const QIcon& icon, const QColor& hoverColor, style style, QWidget* parent) :
    PointedButton (icon, "", parent),
    m_hoverColor (hoverColor),
    m_style (style)
    {
    m_hoverColor.setAlphaF (0.25);

    common::makeFrameless (*this);
    }

void HoverIcon::paintEvent (QPaintEvent* event)
    {
    QStyleOptionButton styles;

    initStyleOption (&styles);

    if ((QStyle::State_MouseOver & styles.state) > 0)
        {
        int         diameter = qMin (height (), width ());
        QPainter    painter{ this };

        painter.setRenderHint (QPainter::Antialiasing, true);

        painter.translate (width ()  / 2,
                           height () / 2);

        painter.setPen (Qt::NoPen);
        painter.setBrush (QBrush{ m_hoverColor });
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

    QPushButton::paintEvent (event);
    }
}
