/**
 * @file        common/touchslider.cpp
 * @brief       Slider class that fixes the issues with QSlider on touchscreens
 * @author      Justin Scott
 * @date        2026-08-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/touchslider.hpp>
#include <ui/common/utils.hpp>

#include <QMouseEvent>

namespace ui::common
{

TouchSlider::TouchSlider (Qt::Orientation ornt, QWidget* parent) :
    QSlider (ornt, parent)
    {
    setAttribute (Qt::WA_AcceptTouchEvents);
    }

bool TouchSlider::event (QEvent* event)
    {
    bool rc = false;

    switch (event->type ())
        {
        case QTouchEvent::TouchBegin:
        case QTouchEvent::TouchUpdate:
        case QTouchEvent::TouchEnd:
            {
            if (isEnabled ()) // block touch events for disabled widgets
                {
                QTouchEvent* touch = static_cast<QTouchEvent*> (event);

                if (Qt::Vertical == orientation ())
                    {
                    setValue (minimum () +
                              ((maximum () - minimum ()) *
                               (height ()  - touch->point (0).position ().y ())) / height ());
                    }
                else // (Qt::Horizontal == orientation ())
                    {
                    setValue (minimum () +
                              ((maximum () - minimum ()) *
                               touch->point (0).position ().x ()) / width ());
                    }

                event->accept ();
                }

            rc = true;
            break;
            }
        default:
            {
            rc = QSlider::event (event);
            break;
            }
        }

    return rc;
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
