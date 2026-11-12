/**
 * @file        automation/conditiontest.cpp
 * @brief       Test suite for the automation condition class
 * @author      Justin Scott
 * @date        2026-11-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/condition.hpp>

#include <testutils/controllerstubs.hpp>

#include <QtTest>

using namespace std::chrono_literals;

static constexpr uint8_t NUM_BITS = 8;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the automation condition class
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class AutomationConditionTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for evaluateTimerTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void evaluateTimerTest_data ()
        {
        QTest::addColumn<std::chrono::seconds> ("duration");

        QTest::newRow ("1s")  << 1s;
        QTest::newRow ("2s")  << 2s;
        QTest::newRow ("5s")  << 5s;
        QTest::newRow ("10s") << 10s;

        QTest::newRow ("1 min") << 60s;
        QTest::newRow ("10 min") << 600s;
        QTest::newRow ("1 hour") << 36000s;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test evaluate for the timer condition
    ///
    /// @see    control::AutomationTimer::evaluate()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void evaluateTimerTest ()
        {
        QFETCH (std::chrono::seconds, duration);

        control::AutomationTimer timer{ duration };

        // Sanity check
        QCOMPARE (timer.getDuration (), duration);

        control::FastClock::time_point start;

        // Negative checks

        // 0.5 second before timer
        QVERIFY (not timer.evaluate (start, start, start + duration - 500ms));

        // 0.5 seconds after time, but already checked at the time
        QVERIFY (not timer.evaluate (start + duration, start + duration, start + duration + 500ms));

        // Positive checks

        // Exactly duration has past
        QVERIFY (timer.evaluate (start, start, start + duration));

        // Past duration, and not checked since the last period
        QVERIFY (timer.evaluate (start, start, start + duration + 500ms));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for evaluateDailyAlarm()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void evaluateDailyAlarm_data ()
        {
        /// Don't need a lot, just sanity checking it works on different days
        static const QDate DATES[] =
            {
                { 2001, 9, 11 },
                { 2007, 6, 29 },
                { 2020, 1, 30 }
            };

        QTest::addColumn<QTime> ("time");
        QTest::addColumn<QDate> ("date");

        for (int hour = 0; hour < 24; ++hour)
            {
            for (int min = (hour % 6); min < 60; min += 6)
                {
                for (int sec = (min % 10); sec < 60; sec += 10)
                    {
                    for (const QDate& date : DATES)
                        {
                        QTest::addRow ("%s %02d:%02d:%02d",
                                       date.toString ().toStdString ().c_str (),
                                       hour,
                                       min,
                                       sec)
                            << QTime{ hour, min, sec }
                            << date;
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    ///  Test evaluate for the alarm condition
    ///
    /// @see    control::AutomationAlarm::evaluate()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void evaluateDailyAlarm ()
        {
        QFETCH (QTime, time);
        QFETCH (QDate, date);

        QDateTime datetime{ date, time };

        control::AutomationAlarm alarm{ time };

        // Sanity check
        QCOMPARE (alarm.getTime (), time);

        auto start = utils::time::toTP (datetime);

        // 0.5 second before alarm
        QVERIFY (not alarm.evaluate (start - 500ms, start - 500ms, start - 500ms));

        // 0.5 seconds after alarm, but checked 0.25s after
        QVERIFY (not alarm.evaluate (start + 250ms, start + 250ms, start + 500ms));

        QVERIFY (alarm.evaluate (start - 500ms, start - 500ms, start + 500ms));
        }

    ///////////////////////////////////////////////////////////////////////////////
    ///  Test evaluate for the sensor condition
    ///
    /// @see    control::AutomationSesnor::evaluate()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void evaluateSensorTest ()
        {
        std::vector<bool> bits;

        bits.resize (NUM_BITS);

        testutils::SensorControllerStubs    controller;
        layout::SensorModule                sensor{ &controller, bits, "", 1 };

        control::FastClock::time_point tp; // Doesn't matter

        for (int state = 0; state < 2; ++state) // initial port state
            {
            for (uint8_t ii = 0; ii < NUM_BITS; ++ii) // Port to watch
                {
                std::fill (bits.begin (),
                           bits.end (),
                           1 == state);
                controller.setSensorState (sensor, bits);

                control::AutomationSensor rising{  sensor, control::AutomationSensor::CONDITION_RISING_EDGE, ii };
                control::AutomationSensor falling{ sensor, control::AutomationSensor::CONDITION_FALLING_EDGE, ii };
                control::AutomationSensor change{  sensor, control::AutomationSensor::CONDITION_CHANGE, ii };

                for (uint8_t jj = 0; jj < NUM_BITS; ++jj) // Port to set
                    {
                    bits[jj] = 0 == state; // flip the state
                    controller.setSensorState (sensor, bits);

                    if (ii == jj)
                        {
                        if (0 == state)
                            {
                            QVERIFY (rising.evaluate (tp, tp, tp));
                            QVERIFY (not falling.evaluate (tp, tp, tp));
                            }
                        else // (1 == state)
                            {
                            QVERIFY (not rising.evaluate (tp, tp, tp));
                            QVERIFY (falling.evaluate (tp, tp, tp));
                            }

                        QVERIFY (change.evaluate (tp, tp, tp));
                        }
                    else
                        {
                        // Changed a different port, this should not trigger any of the conditions
                        QVERIFY (not rising.evaluate (tp, tp, tp));
                        QVERIFY (not falling.evaluate (tp, tp, tp));
                        QVERIFY (not change.evaluate (tp, tp, tp));
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the isAutomating() function for a sensor condition
    ///
    /// @see    control::AutomationSensor::isAutomating()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void sensorIsAutomatingTest ()
        {
        std::vector<bool> bits;

        bits.resize (NUM_BITS);

        testutils::SensorControllerStubs    controller;
        testutils::RouteControllerStubs     routeController;

        layout::SensorModule sensor1{ &controller, bits, "", 1 };
        layout::SensorModule sensor2{ &controller, bits, "", 2 };

        layout::SensorModule sensorCopy = sensor1;

        layout::Route        route{ &routeController, "", {}, 1 }; // same ID as our sensor

        control::AutomationSensor rising{ sensor1, control::AutomationSensor::CONDITION_RISING_EDGE, 4 };

        QVERIFY (rising.isAutomating (sensor1));    // Original sensor
        QVERIFY (rising.isAutomating (sensorCopy)); // copy of sensor
        QVERIFY (not rising.isAutomating (sensor2));// Different sensor
        QVERIFY (not rising.isAutomating (route));  // Same ID, different component type
        }
    };

QTEST_GUILESS_MAIN (AutomationConditionTest)

#include "conditiontest.moc"
