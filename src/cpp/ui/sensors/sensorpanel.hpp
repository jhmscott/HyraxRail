/**
 * @file        sensors/sensorpanel.hpp
 * @brief       Panel to display the state of sensors
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QWidget>

namespace ui::sensors
{
class SensorPanel : public QWidget
    {
public:
    SensorPanel (QWidget* parent);
    };
}