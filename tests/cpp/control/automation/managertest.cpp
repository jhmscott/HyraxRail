/**
 * @file        automation/managertest.cpp
 * @brief       Test suite for the automation manager class
 * @author      Justin Scott
 * @date        2026-11-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/automation/manager.hpp>

#include <testutils/controllerstubs.hpp>
#include <testutils/mockcontroller.hpp>

#include <QtTest>

static const std::string    ROUTE_NAME      = "Automated Route";
static const std::string    ACTUATOR_NAME   = "Automated Actuator";
static const std::string    LOCO_NAME       = "Automated Locomotive";
static const size_t         ID              = 1189998819991197253;

static const std::vector<layout::funcInfo>  LOCO_FUNCS =
    {
        { "Function 1", layout::funcInfo::ICON_FUNC_SOUND_GENERIC, 1, true },
        { "Function 3", layout::funcInfo::ICON_FUNC_LIGHT_CAB, 3, false },
        { "Function 5", layout::funcInfo::ICON_FUNC_MISC_SLOW, 5, true },
        { "Function 7", layout::funcInfo::ICON_FUNC_MISC_ABV, 7, false }
    };


///////////////////////////////////////////////////////////////////////////////
/// Populate an automation manager with items to automate
///
/// @param[out] manager     Automation manager
/// @param[in]  controller  Controller for components
///
///////////////////////////////////////////////////////////////////////////////
static void populateItems (control::AutomationManager&  manager,
                           testutils::MockController&   controller)
    {
    layout::Route       route{ &controller, ROUTE_NAME, {}, ID };
    layout::Actuator    actuator{ &controller,
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   true };
    layout::Locomotive loco{ &controller,
                              LOCO_NAME,
                              layout::TRACK_PROTO_MFX,
                              1,
                              LOCO_FUNCS,
                              0,
                              ID };

    manager.emplace ().setItem (new control::AutomationItem{ route });
    manager.emplace ().setItem (new control::AutomationItem{ route });

    manager.emplace ().setItem (new control::AutomationItem{ actuator });
    manager.emplace ().setItem (new control::AutomationItem{ actuator });

    for (const auto& function : LOCO_FUNCS)
        {
        manager.emplace ().setItem (new control::AutomationItem{ loco, function.id });
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Generate a set of count values
///
///////////////////////////////////////////////////////////////////////////////
static void generateCountData ()
    {
    QTest::addColumn<size_t> ("count");

    for (size_t ii = 1; ii <= 10; ++ii)
        {
        QTest::addRow ("%zu", ii) << ii;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Generate a set of function numbers
///
///////////////////////////////////////////////////////////////////////////////
static void generateFunctionData ()
    {
    QTest::addColumn<uint8_t> ("num");

    for (const auto& function : LOCO_FUNCS)
        {
        QTest::newRow (function.name.c_str ()) << function.id;
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the automation manager class
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class AutomationManagerTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for emplaceTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void emplaceTest_data () { generateCountData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test emplacing tasks
    ///
    /// @see    control::AutomationManager::emplace()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void emplaceTest ()
        {
        QFETCH (size_t, count);

        control::AutomationManager manager;

        for (size_t ii = 0; ii < count; ++ii)
            {
            manager.emplace ();
            }

        QCOMPARE (manager.size (), count);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for clearTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clearTest_data () { generateCountData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test clearing the manager
    ///
    /// @see    control::AutomationManager::clear()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clearTest ()
        {
        QFETCH (size_t, count);

        control::AutomationManager manager;
        QSignalSpy spy{ &manager, &control::AutomationManager::taskDestroyed };

        for (size_t ii = 0; ii < count; ++ii)
            {
            manager.emplace ();
            }

        manager.clear ();

        QCOMPARE (manager.size (), 0);
        QCOMPARE (spy.count (), count);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test erasing a task from the manager
    ///
    /// @see    control::AutomationManager::erase()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void eraseTest ()
        {
        control::AutomationManager      manager;
        const control::AutomationTask*  destroyedTask = NULL;

        // QSignalSpy doesn't capture non-copyable types :(
        connect (&manager,
                 &control::AutomationManager::taskDestroyed,
                  this,
                  [&destroyedTask] (const control::AutomationTask& task) -> void
                  { destroyedTask = &task; });

        manager.emplace ();
        auto& task = manager.emplace ();
        manager.emplace ();

        // Remove the second task
        manager.erase (std::next (manager.begin ()));

        QCOMPARE (manager.size (), 2);
        QCOMPARE (destroyedTask, &task);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test destroying an automated route
    ///
    /// @see    control::AutomationManager::taskDestroyed()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyRouteTest ()
        {
        control::AutomationManager      manager;
        testutils::RouteControllerStubs controller;
        QSignalSpy                      spy{ &manager,
                                             &control::AutomationManager::taskDestroyed };
        layout::Route                   route{ &controller, ROUTE_NAME, {}, ID };

        manager.emplace ().setItem (new control::AutomationItem{ route });

        route.remove ();

        QCOMPARE (manager.size (), 0);
        QCOMPARE (spy.count (), 1);
        }
    ///////////////////////////////////////////////////////////////////////////////
    /// Test destroying an automated actuator
    ///
    /// @see    control::AutomationManager::taskDestroyed()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyActuatorTest ()
        {
        control::AutomationManager          manager;
        testutils::ActuatorControllerStubs  controller;
        QSignalSpy                          spy{ &manager,
                                                 &control::AutomationManager::taskDestroyed };

        layout::Actuator actuator{ &controller,
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   true };

        manager.emplace ().setItem (new control::AutomationItem{ actuator });

        actuator.remove ();

        QCOMPARE (manager.size (), 0);
        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test destroying an automated locomotive
    ///
    /// @see    control::AutomationManager::taskDestroyed()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyLocoTest ()
        {
        control::AutomationManager              manager;
        testutils::LocomotiveControllerStubs    controller;
        QSignalSpy                              spy{ &manager,
                                                     &control::AutomationManager::taskDestroyed };

        layout::Locomotive loco{ &controller,
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };

        manager.emplace ().setItem (new control::AutomationItem{ loco, 1 });

        loco.remove ();

        QCOMPARE (manager.size (), 0);
        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for destroyFunctionTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyFunctionTest_data () { generateFunctionData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test destroying an automated locomotive function
    ///
    /// @see    control::AutomationManager::taskDestroyed()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void destroyFunctionTest ()
        {
        QFETCH (uint8_t, num);

        control::AutomationManager              manager;
        testutils::LocomotiveControllerStubs    controller;
        QSignalSpy                              spy{ &manager,
                                                     &control::AutomationManager::taskDestroyed };

        layout::Locomotive loco{ &controller,
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };

        manager.emplace ().setItem (new control::AutomationItem{ loco, num });

        auto newFuncs = LOCO_FUNCS;

        // Test 1, remove all function EXCEPT the one we're automating

        newFuncs.erase (std::remove_if (newFuncs.begin (),
                                        newFuncs.end (),
                                        [num] (const layout::funcInfo& info)
                                        { return num != info.id; }),
                        newFuncs.end ());

        loco.setFunctions (newFuncs);

        // This should not destroy the task
        QCOMPARE (manager.size (), 1);
        QCOMPARE (spy.count (), 0);

        // Test 2, only remove the automated task

        newFuncs = LOCO_FUNCS;

        newFuncs.erase (std::remove_if (newFuncs.begin (),
                                        newFuncs.end (),
                                        [num] (const layout::funcInfo& info)
                                        { return num == info.id; }),
                        newFuncs.end ());

        loco.setFunctions (newFuncs);

        // This should destroy the task
        QCOMPARE (manager.size (), 0);
        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test finding an automated route
    ///
    /// @see    control::AutomationManager::findAutomatedComponents()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findRouteTest ()
        {
        control::AutomationManager  manager;
        testutils::MockController   controller{
            "Automation Test Mocked Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        populateItems (manager, controller);

        layout::Route route{ &controller, ROUTE_NAME, {}, ID };

        auto tasks = manager.findAutomatedComponents (route);

        QCOMPARE (tasks.size (), 2);

        // Check type
        QCOMPARE (tasks[0].get ().getItem ()->getType (),
                  control::AutomationItem::type::ROUTE);
        QCOMPARE (tasks[1].get ().getItem ()->getType (),
                  control::AutomationItem::type::ROUTE);

        // Check component
        QCOMPARE (tasks[0].get ().getItem ()->getRoute (), route);
        QCOMPARE (tasks[1].get ().getItem ()->getRoute (), route);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test finding an automated actuator
    ///
    /// @see    control::AutomationManager::findAutomatedComponents()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findActuatorTest ()
        {
        control::AutomationManager  manager;
        testutils::MockController   controller{
            "Automation Test Mocked Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        populateItems (manager, controller);

        layout::Actuator actuator{ &controller,
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   true };

        auto tasks = manager.findAutomatedComponents (actuator);

        QCOMPARE (tasks.size (), 2);

        // Check type
        QCOMPARE (tasks[0].get ().getItem ()->getType (),
                  control::AutomationItem::type::ACTUATOR);
        QCOMPARE (tasks[1].get ().getItem ()->getType (),
                  control::AutomationItem::type::ACTUATOR);

        // Check component
        QCOMPARE (tasks[0].get ().getItem ()->getActuator (), actuator);
        QCOMPARE (tasks[1].get ().getItem ()->getActuator (), actuator);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test finding an automated locomotive
    ///
    /// @see    control::AutomationManager::findAutomatedComponents()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findLocoTest ()
        {
        control::AutomationManager  manager;
        testutils::MockController   controller{
            "Automation Test Mocked Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        populateItems (manager, controller);

        layout::Locomotive loco{ &controller,
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };

        auto tasks = manager.findAutomatedComponents (loco);

        QCOMPARE (tasks.size (), LOCO_FUNCS.size ());

        // Check type
        for (const auto& task : tasks)
            {
            QCOMPARE (task.get ().getItem ()->getType (),
                      control::AutomationItem::type::LOCO_FUNC);
            QCOMPARE (task.get ().getItem ()->getFunction ()->loco, loco);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for findLocoFuncTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findLocoFuncTest_data () { generateFunctionData (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test finding an automated locomotive function
    ///
    /// @see    control::AutomationManager::findAutomatedComponents()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findLocoFuncTest ()
        {
        QFETCH (uint8_t, num);

        control::AutomationManager  manager;
        testutils::MockController   controller{
            "Automation Test Mocked Controller",
            std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) };

        populateItems (manager, controller);

        layout::Locomotive loco{ &controller,
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };

        control::AutomationItem::function function{ loco, num };

        auto tasks = manager.findAutomatedComponents (function);

        QCOMPARE (tasks.size (), 1);
        const auto* item = tasks[0].get ().getItem ();

        QCOMPARE (item->getType (), control::AutomationItem::type::LOCO_FUNC);
        QCOMPARE (item->getFunction ()->loco, loco);
        QCOMPARE (item->getFunction ()->func, num);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the formatting of an automation deletion message
    ///
    /// @see    control::formatAutomationDeletionMessage()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void formatDeletionTest ()
        {
        static constexpr size_t NUM_TASKS = 10;

        control::AutomationManager      manager;
        testutils::RouteControllerStubs controller;

        layout::Route route{ &controller, ROUTE_NAME, {}, ID };

        for (size_t ii = 0; ii < NUM_TASKS; ++ii)
            {
            auto& task = manager.emplace ();

            task.setItem (new control::AutomationItem{ route });
            task.name = QString{ "Automation %1" }.arg (ii);
            }

        auto tasks = manager.findAutomatedComponents (route);

        QCOMPARE (tasks.size (), NUM_TASKS);

        QString msg = control::formatAutomationDeletionMessage (tasks);

        for (size_t ii = 0; ii < NUM_TASKS; ++ii)
            {
            QVERIFY (msg.contains (QString{ "Automation %1" }.arg (ii)));
            }
        }
    };

QTEST_GUILESS_MAIN (AutomationManagerTest)

#include "managertest.moc"
