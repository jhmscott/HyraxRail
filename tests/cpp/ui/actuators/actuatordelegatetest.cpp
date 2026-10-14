/**
 * @file        common/actuatordelegatetest.hpp
 * @brief       Test suite for the actuator delegate
 * @author      Justin Scott
 * @date        2026-10-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/mockcontroller.hpp>
#include <testutils/ui.hpp>

#include <ui/actuators/actuatordelegate.hpp>
#include <ui/actuators/editactuator.hpp>

#include <ui/common/utils.hpp>

#include <QtTest>


using namespace ui::actuators;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the actuator delegate
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ActuatorDelegateTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test cancelling the creation of an actuator
    ///
    /// @see    ui::actuators::ActuatorDelegate::openCreate()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cancelCreateTest ()
        {
        control::AutomationManager manager;
        testutils::MockController  controller{ "Test",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        ActuatorDelegate del{ manager };

        // openCreate() blocks until completion, so we need a timer to access it while it's running
        QTimer::singleShot (20,
            []() -> void
            {
            auto* dlg = ui::common::getTopLevelWidget<EditActuatorDialog> ();

            QCOMPARE_NE (dlg, NULL);

            testutils::clickStandardButton (*dlg, QDialogButtonBox::Cancel);
            });

        // Cancel returns NULL
        QCOMPARE (del.openCreate (controller, NULL), NULL);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the creation of actuator buttons
    ///
    /// @see    ui::actuators::ActuatorDelegate::createButtons()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void createButtonsTest ()
        {
        control::AutomationManager manager;
        testutils::MockController  controller{ "Test",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        ActuatorDelegate del{ manager };

        controller.actuators.push_back (layout::Actuator{  &controller.actuatorController->get (),
                                                            "",
                                                            layout::ICON_LIGHTING,
                                                            layout::actuatorMode::PULSE,
                                                            1,
                                                            100,
                                                            1,
                                                            true });
        controller.actuators.push_back (layout::Actuator{  &controller.actuatorController->get (),
                                                            "",
                                                            layout::ICON_LIGHTING,
                                                            layout::actuatorMode::PULSE,
                                                            2,
                                                            100,
                                                            2,
                                                            true });
        controller.actuators.push_back (layout::Actuator{  &controller.actuatorController->get (),
                                                            "",
                                                            layout::ICON_LIGHTING,
                                                            layout::actuatorMode::PULSE,
                                                            3,
                                                            100,
                                                            3,
                                                            true });
        controller.refresh ();

        int count = 0;

        del.createButtons (controller,
            [&count] (ui::common::LayoutButton* btn) -> void
            { ++count; });

        QCOMPARE (count, 3);
        }
    };

QTEST_MAIN (ActuatorDelegateTest);

#include "actuatordelegatetest.moc"
