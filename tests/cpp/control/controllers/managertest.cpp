/**
 * @file        controlllers/managertest.cpp
 * @brief       Test suite for the Controller manager
 * @author      Justin Scott
 * @date        2026-10-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/controllers/manager.hpp>

#include <testutils/mockcontroller.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the Controller manager
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class ControllerManagerTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test the append function
    ///
    /// @see    control::ControllerManager::append()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void appendTest ()
        {
        control::ControllerManager manager;

        QSignalSpy spy{ &manager, &control::ControllerManager::controllerAdded };

        manager.append (testutils::mockedControllerInfo ("Test 1"), true);
        manager.append (testutils::mockedControllerInfo ("Test 2"), true);
        manager.append (testutils::mockedControllerInfo ("Test 3"), true);

        QCOMPARE (manager.size (), 3);

        QCOMPARE (manager[0].getFriendlyName (), "Test 1");
        QCOMPARE (manager[1].getFriendlyName (), "Test 2");
        QCOMPARE (manager[2].getFriendlyName (), "Test 3");

        QCOMPARE (spy.count (), 3);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the remove function
    ///
    /// @see    control::ControllerManager::remove()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeTest ()
        {
        control::ControllerManager manager;

        QSignalSpy spy{ &manager, &control::ControllerManager::controllerDeleted };

        manager.append (testutils::mockedControllerInfo ("Test 1"), true);
        manager.append (testutils::mockedControllerInfo ("Test 2"), true);
        manager.append (testutils::mockedControllerInfo ("Test 3"), true);

        auto& toRemove = manager[1];

        manager.remove (toRemove);

        QCOMPARE (manager.size (), 2);

        QCOMPARE (manager[0].getFriendlyName (), "Test 1");
        QCOMPARE (manager[1].getFriendlyName (), "Test 3");

        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the component cache refresh operation
    ///
    /// @see    control::ControllerManager::refresh()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshTest ()
        {
        control::ControllerManager manager;

        QSignalSpy startSpy{ &manager, &control::ControllerManager::refreshStarted };
        QSignalSpy endSpy{ &manager, &control::ControllerManager::cacheRebuilt };

        manager.append (testutils::mockedControllerInfo ("Test"), true);

        auto& controller = static_cast<testutils::MockController&> (manager[0]);

        manager.refresh (controller);

        QCOMPARE (startSpy.count (), 1);

        QVERIFY (QTest::qWaitFor ([&]() -> bool { return endSpy.count () > 0; }, 1000));

        QCOMPARE (endSpy.count (), 1);

        // Proxy for checking if refresh() is called
        // Check for twice, intial build in append + refresh
        fakeit::Verify (Method (*controller.actuatorController,   getActuators)).Twice ();
        fakeit::Verify (Method (*controller.locomotiveController, getLocomotives)).Twice ();
        fakeit::Verify (Method (*controller.routeController,      getRoutes)).Twice ();
        }
    };

QTEST_GUILESS_MAIN (ControllerManagerTest)

#include "managertest.moc"
