/**
 * @file        sensors/sensorbar.hpp
 * @brief       Bar representing the state of sensors for a given sensor module
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/sensors/sensorstate.hpp>

#include <QHBoxLayout>
#include <QWidget>

namespace ui::sensors
{

class SensorBar : public QWidget
    {
public:
    SensorBar (const QString& name, QWidget* parent);

private:
    std::vector<SensorState*> m_states;
    };

}