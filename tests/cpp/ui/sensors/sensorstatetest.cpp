/**
 * @file        common/sensorstatetest.hpp
 * @brief       Test suite for the sensor state widget
 * @author      Justin Scott
 * @date        2026-10-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#define LAYOUT_TEST_CLASS   SensorStateTest

#include <testutils/mockcontroller.hpp>
#include <testutils/ui.hpp>

#include <ui/sensors/sensorstate.hpp>

#include <ui/common/utils.hpp>

#include <QtTest>


using namespace ui::sensors;

///////////////////////////////////////////////////////////////////////////////
/// Asserts the sensor is drawn correctly for a given state
///
/// @param[in]  wdgt        Widget state
/// @param[in]  state       Expected state
///
///////////////////////////////////////////////////////////////////////////////
static void assertSensorState (SensorDisplayState& wdgt, bool state)
    {
    QSize  size     = wdgt.size ();
    QColor expected = state ? Qt::green : Qt::red;
    QImage image{ size, QImage::Format_ARGB32 };

    image.fill (Qt::transparent);
    wdgt.render (&image);

    // Check the center pixel for the actual colour of the dot
    QColor actual = image.pixelColor (size.width () / 2, size.height () / 2);

    QCOMPARE (actual, expected);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the sensor state widget
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SensorStateTest : public QObject
    {
    Q_OBJECT
private:
    std::unique_ptr<testutils::MockController> controller;

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test case setup
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        controller.reset (new testutils::MockController{ "Sensor Test",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) });
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case cleanup
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        controller.reset ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for initStateTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void initStateTest_data ()
        {
        QTest::addColumn<bool> ("state");

        QTest::newRow ("true")  << true;
        QTest::newRow ("false") << false;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that the widget displays the initial state of the sensor
    ///
    /// @see    ui::sensors::SensorDisplayState
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void initStateTest ()
        {
        QFETCH (bool, state);

        layout::SensorModule sensor{ controller.get (), { state }, "Test Module", 1 };

        SensorDisplayState wdgt{ sensor, 0, 10, NULL };

        assertSensorState (wdgt, state);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for changeStateTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeStateTest_data ()
        {
        QTest::addColumn<bool> ("state");
        QTest::addColumn<int> ("idx");
        QTest::addColumn<int> ("toChange");

        for (int ii = 0; ii < 2; ++ii)
            {
            for (int jj = 0; jj < 2; ++jj)
                {
                QTest::addRow ("Sensor %d, set %d to true",  ii, jj) << true  << ii << jj;
                QTest::addRow ("Sensor %d, set %d to false", ii, jj) << false << ii << jj;
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test changing the state for an already created sensor state widget
    ///
    /// @see    ui::sensors::SensorDisplayState
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void changeStateTest ()
        {
        QFETCH (bool, state);
        QFETCH (int,  idx);
        QFETCH (int,  toChange);

        std::vector<bool> states;

        states.resize (2, not state);

        layout::SensorModule sensor{ controller.get (), states, "Test Module", 1 };

        SensorDisplayState wdgt{ sensor, static_cast<size_t> (idx), 10, NULL };

        states[toChange] = state;

        controller->sensorController->get ().setSensorState (sensor, states);

        if (toChange == idx)
            {
            // Assert we've changed to the new state
            assertSensorState (wdgt, state);
            }
        else
            {
            // We signaled the other sensor changed state
            // so it should still be in the initial state
            assertSensorState (wdgt, not state);
            }
        }
    };

QTEST_MAIN (SensorStateTest);

#include "sensorstatetest.moc"
