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

//////////////////////////////////////////////////////////////////////////////
/// Widget that displays a pointing hand cursor when you hover over it
///
/// @tparam     Widget      QT widget class to add this capability to
///
//////////////////////////////////////////////////////////////////////////////
template<class Widget>
class PointedWidget : public Widget
    {
    static_assert (std::is_base_of_v<QWidget, Widget>,"Must inherit form a QWidget type");
public:
    // Use constructors from widget base class
    using Widget::Widget;

protected:
    //////////////////////////////////////////////////////////////////////////////
    /// Handle the mouse enter event
    ///
    /// @param[in]  event       Mouse enter event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void enterEvent (QEnterEvent* event) override
        {
        if (Widget::isEnabled ())
            {
            Widget::setCursor (Qt::PointingHandCursor);
            }

        Widget::enterEvent (event);
        }
    };

// Push button that displays a pointing hand cursor when you hover over it
using PointedButton = PointedWidget<QPushButton>;


} // namespace ui::common