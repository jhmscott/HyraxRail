#pragma once

#include <QPushButton>
#include <QWidget>

#include <type_traits>

namespace ui::common
{
template<class Widget>
class PointedWidget: public Widget
    {
    static_assert (std::is_base_of_v<QWidget, Widget>,"Must inherit form a QWidget type");
public:
    using Widget::Widget;

protected:
    virtual void enterEvent (QEnterEvent* event) override
        {
        Widget::setCursor (Qt::PointingHandCursor);
        Widget::enterEvent (event);
        }
    };

using PointedButton = PointedWidget<QPushButton>;
}