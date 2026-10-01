/**
 * @file        config/dialogtest.hpp
 * @brief       Test suite for the controller settings dialog
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <names.hpp>

#include <testutils/controllerstubs.hpp>
#include <testutils/mockcontroller.hpp>
#include <testutils/ext/fakeit.hpp>

#include <ui/config/dialog.hpp>

#include <utils/string.hpp>

#include <QtTest>

using namespace ui::config;

Q_DECLARE_METATYPE (control::controllerCapabilitySet);

///////////////////////////////////////////////////////////////////////////////
/// Test a capability string
///////////////////////////////////////////////////////////////////////////////
static void testCapabilitiesString (std::string_view string, control::controllerCapabilitySet capabilities)
    {
    const std::string CAPABILITY_NAMES[] =
        {
        "Locomotive Controller\n",
        "Actuator Controller\n",
        "Route Controller\n",
        "Emergency Stop\n"
        };
    ASSERT_ARRAY_LENGTH (CAPABILITY_NAMES, control::NUM_CAPABILITIES);

    for (int ii = 0; ii < control::NUM_CAPABILITIES; ++ii)
        {
        bool expected = capabilities[ii];

        QCOMPARE (std::string_view::npos != string.find (CAPABILITY_NAMES[ii]), expected);
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the controller settings dialog
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ConfigDialogTest : public QObject
    {
    Q_OBJECT
private:
    static inline testutils::ControllerMetaClassBaseStubs META =
        {
        "MockedCapabilityController",
        "Capability Test Controller",
        { &testutils::MockControllerProtocol::getMetaClassStatic () },
        control::controllerCapabilitySet{ 0 }
        };

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for capabilityTooltipTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void capabilityTooltipTest_data ()
        {
        QTest::addColumn<control::controllerCapabilitySet> ("hardware");
        QTest::addColumn<control::controllerCapabilitySet> ("software");

        // Hardware only tests
        for (int ii = 0; ii < control::NUM_CAPABILITIES; ++ii)
            {
            control::controllerCapabilitySet hardware{ 0 };

            hardware[ii] = true;

            QTest::addRow ("Hardware: %d", ii)
                << hardware << control::controllerCapabilitySet{ 0 };
            }

        // 2 hardware
        for (int ii = 0; ii < control::NUM_CAPABILITIES; ++ii)
            {
            for (int jj = 0; jj < control::NUM_CAPABILITIES; ++jj)
                {
                if (ii != jj)
                    {
                    control::controllerCapabilitySet hardware{ 0 };

                    hardware[ii] = true;
                    hardware[jj] = true;

                    QTest::addRow ("Hardware 1: %d, Hardware 2: %d", ii, jj)
                        << hardware << control::controllerCapabilitySet{ 0 };
                    }
                }
            }

        // 1 hardware + 1 software
        for (int ii = 0; ii < control::NUM_CAPABILITIES; ++ii)
            {
            for (int jj = 0; jj < control::NUM_CAPABILITIES; ++jj)
                {
                control::controllerCapabilitySet hardware{ 0 };
                control::controllerCapabilitySet software{ 0 };

                hardware[ii] = true;
                software[jj] = true;

                QTest::addRow ("Hardware: %d, Software: %d", ii, jj)
                    << hardware << software;
                }
            }


        // 1 hardware + 2 software
        for (int ii = 0; ii < control::NUM_CAPABILITIES; ++ii)
            {
            for (int jj = 0; jj < control::NUM_CAPABILITIES; ++jj)
                {
                for (int kk = 0; kk < control::NUM_CAPABILITIES; ++kk)\
                    {
                    if (jj != kk)
                        {
                        control::controllerCapabilitySet hardware{ 0 };
                        control::controllerCapabilitySet software{ 0 };

                        hardware[ii] = true;

                        software[jj] = true;
                        software[kk] = true;

                        QTest::addRow ("Hardware: %d, Software1: %d, Software: %d", ii, jj, kk)
                            << hardware << software;
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the capability tooltip
    ///
    /// @see    ui::config::Dialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void capabilityTooltipTest ()
        {
        QFETCH (control::controllerCapabilitySet, hardware);
        QFETCH (control::controllerCapabilitySet, software);

        const_cast<control::controllerCapabilitySet&> (META.hardwareCapabilities) = hardware;
        const_cast<control::controllerCapabilitySet&> (META.softwareCapabilities) = software;

        testutils::ControllerBaseStubs fakeController{
            "Fake Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo {}) };

        fakeit::Mock<control::ControllerBase> controller{ fakeController };

        fakeit::When (Method (controller, getMetaClass)).AlwaysDo (
            [this] () -> const control::ControllerMetaClassBase&
            { return META; });

        ui::config::Dialog dlg{ NULL, &controller.get () };

        auto* controllerDropdown = dlg.findChild<ui::common::OptionalDropdown*> (
                                                        OBJNAME_CONFIG_DIALOG_CONTROLLER);

        QCOMPARE_NE (controllerDropdown, NULL);

        std::string tooltip = controllerDropdown->toolTip ().toStdString ();

        auto [hardwareActual, softwareActual] = utils::str::split (tooltip, "\n\n");

        QVERIFY (0 == strncmp ("Hardware Capabilities:\n", hardwareActual.data (), 23));

        testCapabilitiesString (hardwareActual, hardware);

        if (software.any ())
            {
            QVERIFY (0 == strncmp ("Software Capabilities:\n", softwareActual.data (), 23));
            testCapabilitiesString (softwareActual, software);
            }
        else
            {
            QCOMPARE (softwareActual, "");
            }
        }
    };

QTEST_MAIN (ConfigDialogTest);

#include "dialogtest.moc"
