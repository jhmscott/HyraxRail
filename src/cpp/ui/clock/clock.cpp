/**
 * @file        clock/clock.cpp
 * @brief       Base class for clock widgets
 * @author      Justin Scott
 * @date        2026-06-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/clock/clock.hpp>

namespace ui::clock
{

ClockWidget::ClockWidget (QWidget* parent) :
    QWidget (parent)
    {

    connect (&control::FastClock::instance (),
             &control::FastClock::tick,
              this,
             &ClockWidget::setTime);

    connect (&control::FastClock::instance (),
             &control::FastClock::timeChanged,
              this,
             &ClockWidget::setTime);
    }

}
