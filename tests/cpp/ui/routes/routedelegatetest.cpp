/**
 * @file        common/routedelegatetest.hpp
 * @brief       Test suite for the route delegate
 * @author      Justin Scott
 * @date        2026-10-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/mockcontroller.hpp>
#include <testutils/ui.hpp>

#include <ui/routes/routedelegate.hpp>
#include <ui/routes/editroute.hpp>

#include <ui/common/utils.hpp>

#include <QtTest>

using namespace ui::routes;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the route delegate
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class RouteDelegateTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test cancelling the creation of a route
    ///
    /// @see    ui::actuators::RouteDelegate::openCreate()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cancelCreateTest ()
        {
        control::AutomationManager manager;
        testutils::MockController  controller{ "Test",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        RouteDelegate del{ manager };

        // openCreate() blocks until completion, so we need a timer to access it while it's running
        QTimer::singleShot (20,
            []() -> void
            {
            auto* dlg = ui::common::getTopLevelWidget<EditRouteDialog> ();

            QCOMPARE_NE (dlg, NULL);

            testutils::clickStandardButton (*dlg, QDialogButtonBox::Cancel);
            });

        // Cancel returns NULL
        QCOMPARE (del.openCreate (controller, NULL), NULL);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the creation of route buttons
    ///
    /// @see    ui::actuators::RouteDelegate::createButtons()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void createButtonsTest ()
        {
        control::AutomationManager manager;
        testutils::MockController  controller{ "Test",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        RouteDelegate del{ manager };

        controller.routes.push_back (layout::Route{ &controller.routeController->get (), "", {}, 1 });
        controller.routes.push_back (layout::Route{ &controller.routeController->get (), "", {}, 2 });
        controller.routes.push_back (layout::Route{ &controller.routeController->get (), "", {}, 3 });
        controller.refresh ();

        int count = 0;

        del.createButtons (controller,
            [&count] (ui::common::LayoutButton* btn) -> void
            { ++count; });

        QCOMPARE (count, 3);
        }
    };

QTEST_MAIN (RouteDelegateTest);

#include "routedelegatetest.moc"
