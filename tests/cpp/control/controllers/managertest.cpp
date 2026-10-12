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

#include <res/version.h>

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
        // Check for twice, initial build in append + refresh
        fakeit::Verify (Method (*controller.actuatorController,   getActuators)).Twice ();
        fakeit::Verify (Method (*controller.locomotiveController, getLocomotives)).Twice ();
        fakeit::Verify (Method (*controller.routeController,      getRoutes)).Twice ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test saving and restoring a controller manager to the user's settings
    ///
    /// @see    control::saveControllersToUserSettings()
    /// @see    control::loadControllersFromUserSettings()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void saveControllersTest ()
        {
        using namespace utils::device;

        static const std::pair<std::string, deviceInfo> CONTROLLERS[] =
            {
                { "Test 1", { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM1" }, 9600 } } },
                { "Test 2", { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM2" }, 9600 } } },
                { "Test 3", { TYPE_TCP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 443} } },
                { "Test 4", { TYPE_TCP, socketInfo{ HostInfo::fromString ("8.8.8.8",   HostInfo::type::IP), 80 } } },
                { "Test 5", { TYPE_TCP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 443} } },
                { "Test 6", { TYPE_TCP, socketInfo{ HostInfo::fromString ("8.8.8.8",   HostInfo::type::IP), 80 } } },
            };
        static constexpr size_t NUM_CONTROLLERS = std::size (CONTROLLERS);

        // Set a unique name from app (and other tests) so this doesn't impact actual
        // settings when run locally
        QApplication::setApplicationName    (PRODUCT_NAME_SHORT "_ManagerTests");
        QApplication::setOrganizationName   (COMPANY_NAME);
        QApplication::setOrganizationDomain (COMPANY_DOMAIN);
        QApplication::setApplicationVersion (VERSION_STRING);

        control::ControllerManager original;

        for (const auto& [name, device] : CONTROLLERS)
            {
            original.append (testutils::mockedControllerInfo (name, device), true);
            }

        std::vector<std::string> names;

        control::saveControllersToUserSettings (original);

        auto fromSettings = control::loadControllersFromUserSettings (
            [&names] (const std::string& name) -> void
            {
            names.push_back (name);
            });

        QCOMPARE (fromSettings->size (), NUM_CONTROLLERS);
        QCOMPARE (names.size (), NUM_CONTROLLERS);

        for (size_t ii = 0; ii < NUM_CONTROLLERS; ++ii)
            {
            QCOMPARE (names[ii], CONTROLLERS[ii].first);

            QCOMPARE (original[ii].getFriendlyName (),  (*fromSettings)[ii].getFriendlyName ());
            QCOMPARE (original[ii].getDeviceInfo (),    (*fromSettings)[ii].getDeviceInfo ());
            }
        }
    };

QTEST_GUILESS_MAIN (ControllerManagerTest)

#include "managertest.moc"
