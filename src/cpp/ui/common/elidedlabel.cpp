/**
 * @file        common/elidedlabel.cpp
 * @brief       Label that automatically elides text when there isn't enough space
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/elidedlabel.hpp>

namespace ui::common
{
ElidedLabel::ElidedLabel (const QString& text, QWidget* parent) :
    QLabel (text, parent)
    {
    setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Preferred);
    }

void ElidedLabel::paintEvent (QPaintEvent* event)
    {
    QPainter     painter{ this };
    QFontMetrics metrics    = painter.fontMetrics ();

    QString     elided      = metrics.elidedText (text (), Qt::ElideRight, width ());

    painter.drawText (QPoint{ 0, metrics.ascent() }, elided);
    }

} // namespace ui::common
