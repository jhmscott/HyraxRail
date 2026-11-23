/**
 * @file        sensors/sensordelegate.cpp
 * @brief       Delegate for the sensor component
 * @author      Justin Scott
 * @date        2026-10-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/sensors/editsensor.hpp>
#include <ui/sensors/sensorbar.hpp>
#include <ui/sensors/sensordelegate.hpp>


namespace ui::sensors
{
common::LayoutButton* SensorDelegate::openCreate (control::ControllerBase&  controller,
                                                  QWidget*                  parent)
    {
    common::LayoutButton*   btn;
    EditSensorDialog        dlg{ controller, parent };

    if (QDialog::Accepted == dlg.exec ())
        {
        layout::SensorModule sensor = controller.createSensor (dlg.getPortCount (),
                                                               dlg.getModulePosition ());

        btn = new SensorBar{ m_automations, controller, sensor, parent };
        }

    return btn;
    }

void SensorDelegate::createButtons (control::ControllerBase&                    controller,
                                    std::function<void (common::LayoutButton*)> addToLayout)
    {
    for (const layout::SensorModule module : controller.getSensors ())
        {
        addToLayout (new SensorBar{ m_automations, controller, module, NULL });
        }
    }

} // namespace ui::sensors
