/**
 * @file        common/hovericon.hpp
 * @brief       Frameless pushbutton with an icon, that animates with a
 *              shadow effect on mouse hovers
 * @author      Justin Scott
 * @date        2026-04-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/common/pointedwidget.hpp>


#include <QPushButton>

namespace ui::common
{

class HoverIcon : public PointedButton
    {
public:
    enum class style
        {
        CIRCLE,
        ROUNDED_RECT
        };

    HoverIcon (const QIcon& icon, const QColor& hoverColor, style style, QWidget* parent);

protected:
    virtual void paintEvent (QPaintEvent* event) override;

private:
    QColor  m_hoverColor;
    style   m_style;
    };

}