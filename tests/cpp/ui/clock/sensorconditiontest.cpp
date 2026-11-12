/**
 * @file        common/sensorconditiontest.cpp
 * @brief       Test suite for the sensor condition form test
 * @author      Justin Scott
 * @date        2026-11-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>
#include <names.hpp>

#include <testutils/mockcontroller.hpp>

#include <ui/clock/sensor.hpp>

#include <QtTest>

using namespace ui::clock;

Q_DECLARE_METATYPE (control::AutomationSensor::condition);

///////////////////////////////////////////////////////////////////////////////
/// Assert that the port dropdown is populated correctly
///
/// @param[in]  ports       Combobox to test
/// @param[in]  count       Expected number for ports
///
///////////////////////////////////////////////////////////////////////////////
static void assertPorts (const QComboBox& ports, int count)
    {
    QCOMPARE (ports.count (), count);

    for (int ii = 0; ii < count; ++ii)
        {
        QCOMPARE (ports.itemText (ii).trimmed (), QString{ "Port %1" }.arg (ii));
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test data generator. Generates each permutation of automation Sensor
///
///////////////////////////////////////////////////////////////////////////////
static void generateSensorData ()
    {
    QTest::addColumn<int> ("module");
    QTest::addColumn<int> ("port");
    QTest::addColumn<control::AutomationSensor::condition> ("cond");

    // Currently hard coded, because this runs before the init()
    for (int module = 0; module < 2; ++module)
        {
        const int maxPorts = 0 == module ? 8 : 16;

        for (int port = 0; port < maxPorts; ++port)
            {
            for (control::AutomationSensor::condition cond :
            utils::algorithm::EnumRange{ control::AutomationSensor::NUM_CONDITIONS })
                {
                QTest::addRow ("%d-bit module - Port %d - Condition %s",
                               maxPorts,
                               port,
                               control::sensorConditionText (cond).toStdString ().c_str ())
                    << module << port << cond;
                }
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the automation add/edit dialog
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SensorConditionFormTest : public QObject
    {
    Q_OBJECT
public:
    SensorConditionFormTest () :
        controllers (NULL)
        {}

private:
    control::ControllerManager controllers;  ///< List of controllers for testing

private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case init; run before each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        controllers.append (
            testutils::mockedControllerInfo ("Edit Auto Controller"), true);

        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        std::vector<bool> bits8;
        std::vector<bool> bits16;

        bits8.resize (8);
        bits16.resize (16);

        controller.sensors =
            {
                { &controller, std::move (bits8),  "8-bit module",  1 },
                { &controller, std::move (bits16), "16-bit module", 2 }
            };

        controller.refresh ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case cleanup; run after each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        controllers.clear ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests that the modules dropdown is populated correctly
    ///
    /// @see    ui::clock::SensorConditionForm
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void modulesDropDownTest ()
        {
        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        SensorConditionForm form{ controllers, NULL };

        QComboBox* modules = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_MODULES);

        QCOMPARE_NE (modules, NULL);
        // 1 extra item for the controller parent item
        QCOMPARE (modules->count (), controller.sensors.size () + 1);

        QCOMPARE (modules->itemText (0), controller.getFriendlyName ());

        for (int ii = 1; ii < modules->count (); ++ii)
            {
            QCOMPARE (modules->itemText (ii).trimmed (),
                      controller.sensors[ii - 1].getName ());
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests that the ports dropdown is populated correctly
    ///
    /// @see    ui::clock::SensorConditionForm
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void portsDropdownTest ()
        {
        SensorConditionForm form{ controllers, NULL };
        QVERIFY (not form.hasAcceptableInput ()); // no module selected, reject input

        QComboBox* modules  = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_MODULES);
        QComboBox* ports    = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_PORTS);

        QCOMPARE_NE (ports,  NULL);
        QCOMPARE_NE (modules, NULL);

        QCOMPARE (ports->count (), 0);

        modules->setCurrentIndex (1);
        assertPorts (*ports, 8);
        QVERIFY (form.hasAcceptableInput ()); // a module must be selected

        modules->setCurrentIndex (2);
        assertPorts (*ports, 16);
        QVERIFY (form.hasAcceptableInput ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setConditionTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setConditionTest_data () { generateSensorData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the condition
    ///
    /// @see    ui::clock::SensorConditionForm::setCondition()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setConditionTest ()
        {
        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        QFETCH (int, module);
        QFETCH (int, port);
        QFETCH (control::AutomationSensor::condition, cond);

        control::AutomationSensor sensor{ controller.sensors[module], cond, (uint8_t) port };

        SensorConditionForm form{ controllers, NULL };

        form.setCondition (sensor);

        QComboBox* modules  = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_MODULES);
        QComboBox* ports    = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_PORTS);
        QComboBox* condition= form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_CONDITIONS);

        QCOMPARE_NE (modules,   NULL);
        QCOMPARE_NE (ports,     NULL);
        QCOMPARE_NE (condition, NULL);

        QCOMPARE (modules->currentText ().trimmed (),
                  controller.sensors[module].getName ());
        QCOMPARE (ports->currentText ().trimmed (),
                  QString{ "Port %1" }.arg (static_cast<int> (port)));
        QCOMPARE (condition->currentText ().trimmed (),
                  control::sensorConditionText (cond));
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for getConditionTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void getConditionTest_data () { generateSensorData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test populating the fields and retrieving the created condition
    ///
    /// @see    ui::clock::SensorConditionForm::getCondition()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void getConditionTest ()
        {
        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        QFETCH (int, module);
        QFETCH (int, port);
        QFETCH (control::AutomationSensor::condition, cond);

        SensorConditionForm form{ controllers, NULL };

        QComboBox* modules  = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_MODULES);
        QComboBox* ports    = form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_PORTS);
        QComboBox* condition= form.findChild<QComboBox*> (OBJNAME_SENSOR_CONDFORM_CONDITIONS);

        QCOMPARE_NE (modules,   NULL);
        QCOMPARE_NE (ports,     NULL);
        QCOMPARE_NE (condition, NULL);

        modules->setCurrentIndex (module + 1);
        ports->setCurrentIndex (port);
        condition->setCurrentIndex (static_cast<int> (cond)); // shhh

        std::unique_ptr<control::AutomationCondition> aut{ form.getCondition () };

        QCOMPARE_NE (aut, NULL);
        QCOMPARE (aut->getType (), control::AutomationCondition::TYPE_SENSOR);

        control::AutomationSensor* sensor = static_cast<control::AutomationSensor*> (aut.get ());

        QCOMPARE (sensor->getModule (),     controller.sensors[module]);
        QCOMPARE (sensor->getPort (),       port);
        QCOMPARE (sensor->getCondition (),  cond);
        }
    };

QTEST_MAIN (SensorConditionFormTest)

#include "sensorconditiontest.moc"
