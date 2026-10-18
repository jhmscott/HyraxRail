/**
 * @file        layout/sensortest.hpp
 * @brief       Test suite for the sensor module layout component
 * @author      Justin Scott
 * @date        2026-10-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#define LAYOUT_TEST_CLASS SensorTest

#include <layout/sensor.hpp>

#include <testutils/ext/fakeit.hpp>

#include <QtTest>

static const std::vector<bool>  INIT_SENSOR_STATE{ true, false, true, false, true, false, true, false };
static const std::string        INIT_SENSOR_NAME = "Module 1";

static const std::vector<bool>  NEW_SENSOR_STATE{ false, true, false, true, false, true, false, true };

static const size_t             ID = 8813;

///////////////////////////////////////////////////////////////////////////////
/// Check that a sensor module matches an expected state
///
/// @param[in]  sensor      Sensor to check
/// @param[in]  state       Expected state
///
///////////////////////////////////////////////////////////////////////////////
static void assertSensorState (const layout::SensorModule&  sensor,
                               const std::vector<bool>&     state)
    {
    QCOMPARE (sensor.size (), state.size ());

    for (size_t ii = 0; ii < sensor.size (); ++ii)
        {
        QCOMPARE (sensor[ii], state[ii]);
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Assert that a sensor module is in the initial state
///
/// @param[in]  sensor      Sensor module to check
///
///////////////////////////////////////////////////////////////////////////////
static void assertInitSensorState (const layout::SensorModule& sensor)
    {
    QCOMPARE (sensor.getName (), INIT_SENSOR_NAME);
    assertSensorState (sensor, INIT_SENSOR_STATE);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the actuator layout component
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SensorTest : public QObject
    {
    Q_OBJECT
private:
    using FakeController = layout::ControllerBase<layout::SensorModule>;///< Controller type
    using MockController = fakeit::Mock<layout::SensorController>;      ///< Controller mock type

    FakeController                  fakeController;                     ///< Controller used to test
    std::unique_ptr<MockController> mockController;                     ///< Mocked controller

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Init the test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        mockController.reset (
            new MockController{ static_cast<layout::SensorController&> (fakeController) });
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cleanup after the test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        mockController.reset ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the copy constructor and assignment operators
    ///
    /// @see    layout::Sensor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void copyTest ()
        {
        layout::SensorModule sensor{ &mockController->get (),
                                      INIT_SENSOR_STATE,
                                      INIT_SENSOR_NAME,
                                      ID };
        layout::SensorModule sensorCopy = sensor;


        // Assert the copy was correct
        assertInitSensorState (sensorCopy);

        layout::SensorModule sensorAssign;

        sensorAssign = sensor;

        // Assert the assignment was correct
        assertInitSensorState (sensorAssign);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the shared state between sensor instances
    ///
    /// @see    layout::Sensor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void sharedStateTest ()
        {
        layout::SensorModule sensor{ &mockController->get (),
                                      INIT_SENSOR_STATE,
                                      INIT_SENSOR_NAME,
                                      ID };
        layout::SensorModule sensorCopy = sensor;

        mockController->get ().setSensorState (sensor, NEW_SENSOR_STATE);

        assertSensorState (sensor,      NEW_SENSOR_STATE);
        assertSensorState (sensorCopy,  NEW_SENSOR_STATE);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setSensorTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSensorTest_data ()
        {
        QTest::addColumn<int> ("bit");

        for (int ii = 0; ii < 8; ++ii)
            {
            QTest::addRow ("set bit %d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting a single port in a sensor module
    ///
    /// @see    layout::Sensor::sensorChanged()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setSensorTest ()
        {
        QFETCH (int, bit);

        std::vector<bool> values;

        values.resize (8);

        layout::SensorModule sensor{ &mockController->get (),
                                      values,
                                      INIT_SENSOR_NAME,
                                      ID };

        QSignalSpy spy{ &sensor, &layout::SensorModule::sensorChanged };

        values[bit] = true;
        mockController->get ().setSensorState (sensor, values);

        QCOMPARE (spy.count (), 1);
        QCOMPARE (spy[0].count (), 2);
        QCOMPARE (spy[0][0].toInt (), bit);
        QCOMPARE (spy[0][1].toBool (), true);

        values[bit] = false;
        mockController->get ().setSensorState (sensor, values);

        QCOMPARE (spy.count (), 2);
        QCOMPARE (spy[1].count (), 2);
        QCOMPARE (spy[1][0].toInt (),  bit);
        QCOMPARE (spy[1][1].toBool (), false);
        }
    };

QTEST_GUILESS_MAIN (SensorTest)

#include "sensortest.moc"
