/**
 * @file        common/pointedwidget.hpp
 * @brief       Adds a pointed cursor when you mouse over the widget
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QPushButton>
#include <QWidget>

#include <type_traits>

namespace ui::common
{
template<class Widget>
class PointedWidget : public Widget
    {
    static_assert (std::is_base_of_v<QWidget, Widget>,"Must inherit form a QWidget type");
public:
    using Widget::Widget;

protected:
    virtual void enterEvent (QEnterEvent* event) override
        {
        if (Widget::isEnabled ())
            {
            Widget::setCursor (Qt::PointingHandCursor);
            }

        Widget::enterEvent (event);
        }
    };

using PointedButton = PointedWidget<QPushButton>;
}