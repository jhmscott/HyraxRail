/**
 * @file        common/touchslider.cpp
 * @brief       Slider class that fixes the issues with QSlider on touchscreens
 * @author      Justin Scott
 * @date        2026-08-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/touchslider.hpp>

#include <QMouseEvent>

namespace ui::common
{
void TouchSlider::mousePressEvent (QMouseEvent* event)
    {
    // User on touch screens expect sliders to jump to the position selected
    if (Qt::LeftButton                      == event->button ()         &&
        QPointingDevice::PointerType::Finger & event->pointerType ()    &&
        not getHandleRect ().contains (event->position ().x (),
                                       event->position ().y ()))
        {
        if (Qt::Vertical == orientation ())
            {
            setValue (minimum () +
                    ((maximum () - minimum ()) *
                     (height ()  - event->position ().y ())) / height ());
            }
        else // (Qt::Horizontal == orientation ())
            {
            setValue (minimum () +
                    ((maximum () - minimum ()) *
                      event->position ().x ()) / width ());
            }

        event->accept ();
        }
    else
        {
        QSlider::mousePressEvent (event);
        }
    }

QRect TouchSlider::getHandleRect () const
    {
    QStyleOptionSlider option;

#ifdef Q_OS_WIN
    option.initFrom (this);
#else
    initStyleOption (&option);
#endif // Q_OS_WIN

    option.subControls = QStyle::SC_SliderHandle;

    return style ()->subControlRect (QStyle::CC_Slider,
                                    &option,
                                     QStyle::SC_SliderHandle,
                                     this);
    }
}