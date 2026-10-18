/**
 * @file        sensors/sensordelegate.cpp
 * @brief       Delegate for the sensor component
 * @author      Justin Scott
 * @date        2026-10-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/sensors/sensorbar.hpp>
#include <ui/sensors/sensordelegate.hpp>


namespace ui::sensors
{
common::LayoutButton* SensorDelegate::openCreate (control::ControllerBase&  controller,
                                                  QWidget*                  parent)
    {
    return NULL;
    }

void SensorDelegate::createButtons (control::ControllerBase&                    controller,
                                    std::function<void (common::LayoutButton*)> addToLayout)
    {
    for (const layout::SensorModule module : controller.getSensors ())
        {
        addToLayout (new SensorBar{ module, NULL });
        }
    }

} // namespace ui::sensors
