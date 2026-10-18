/**
 * @file        common/sensordelegatetest.hpp
 * @brief       Test suite for the sensor delegate
 * @author      Justin Scott
 * @date        2026-10-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/mockcontroller.hpp>
#include <testutils/ui.hpp>

#include <ui/sensors/sensordelegate.hpp>

#include <ui/common/utils.hpp>

#include <QtTest>


using namespace ui::sensors;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the sensor delegate
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SensorDelegateTest : public QObject
    {
    Q_OBJECT
private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the creation of sensor bars
    ///
    /// @see    ui::sensors::SensorDelegate::createButtons()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void createBarsTest ()
        {
        testutils::MockController  controller{ "Test",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        SensorDelegate del;

        controller.sensors.push_back (layout::SensorModule{ &controller.sensorController->get (),
                                                            { true, false },
                                                             "",
                                                             1 });
        controller.sensors.push_back (layout::SensorModule{ &controller.sensorController->get (),
                                                            { true, false },
                                                             "",
                                                             2 });
        controller.sensors.push_back (layout::SensorModule{ &controller.sensorController->get (),
                                                            { true, false },
                                                             "",
                                                             3 });
        controller.refresh ();

        int count = 0;

        del.createButtons (controller,
            [&count] (ui::common::LayoutButton* btn) -> void
            { ++count; });

        QCOMPARE (count, 3);
        }
    };

QTEST_MAIN (SensorDelegateTest);

#include "sensordelegatetest.moc"
