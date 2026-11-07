/**
 * @file        automation/itemtest.cpp
 * @brief       Test suite for the automation item class
 * @author      Justin Scott
 * @date        2026-09-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#define LAYOUT_TEST_CLASS AutomationItemTest
#include <control/automation/item.hpp>

#include <testutils/mockcontroller.hpp>

#include <QtTest>


static const std::string    ROUTE_NAME      = "Automated Route";
static const std::string    ACTUATOR_NAME   = "Automated Actuator";
static const std::string    LOCO_NAME       = "Automated Locomotive";
static const size_t         ID              = 1189998819991197253;

static const std::vector<layout::funcInfo>  LOCO_FUNCS =
    {
        { "Function 1", layout::funcInfo::ICON_FUNC_SOUND_GENERIC,  1, true  },
        { "Function 3", layout::funcInfo::ICON_FUNC_LIGHT_CAB,      3, false },
        { "Function 5", layout::funcInfo::ICON_FUNC_MISC_SLOW,      5, true  },
        { "Function 7", layout::funcInfo::ICON_FUNC_MISC_ABV,       7, false }
    };

Q_DECLARE_METATYPE (control::AutomationItem::action);

///////////////////////////////////////////////////////////////////////////////
/// Given an action and an initial state, get the new state, and whether the
/// state has changed
///
/// @param[in]  action      Action taken
/// @param[in]  initial     Initial item state
///
/// @return     { expected, stateChanged }
///
///////////////////////////////////////////////////////////////////////////////
static std::pair<bool, bool>
getExpectedCallAndState (control::AutomationItem::action action, bool initial)
    {
    bool expected;
    bool stateChanged;

    switch (action)
        {
        case control::AutomationItem::ACTION_SET:
            {
            expected        = true;
            stateChanged  = not initial;
            break;
            }
        case control::AutomationItem::ACTION_UNSET:
            {
            expected        = false;
            stateChanged  = initial;
            break;
            }
        case control::AutomationItem::ACTION_TOGGLE:
            {
            expected        = not initial;
            stateChanged  = true;
            break;
            }
        default:
            {
            QTest::qFail ("This should never happen", __FILE__, __LINE__);
            throw std::runtime_error{ "This should never happen" };
            }
        }

    return std::make_pair (expected, stateChanged);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the automation item class
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class AutomationItemTest : public QObject
    {
    Q_OBJECT

private:
    std::unique_ptr<testutils::MockController> controller;  ///< Controller for automations

private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case init, called before each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        controller.reset (
            new testutils::MockController{
                "Automation Test Mocked Controller",
                std::make_unique<testutils::MockControllerProtocol> (utils::device::deviceInfo{}) });
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case cleanup, called after each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        controller.reset ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the getter functions when the item contains a route
    ///
    /// @see    control::AutomationItem
    /// @see    control::AutomationItem::type::ROUTE
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void routeItemTest ()
        {
        layout::Route           route{ controller.get (), ROUTE_NAME, {}, ID };
        control::AutomationItem item{ route };

        QCOMPARE (item.getType (),  control::AutomationItem::type::ROUTE);
        QCOMPARE (item.name (),     ROUTE_NAME);
        QCOMPARE (item.getRoute (), route);
        QCOMPARE (item.getActions (),
                  utils::algorithm::makeBitset<
                        control::AutomationItem::NUM_ACTIONS> (
                                control::AutomationItem::ACTION_SET));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting an action for a route automation item
    ///
    /// @see    control::AutomationItem::doAction()
    /// @see    control::AutomationItem::type::ROUTE
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void routeActionTest ()
        {
        fakeit::When (Method (*controller->routeController, requestRouteControl)).AlwaysReturn ();
        fakeit::When (Method (*controller->routeController, releaseRouteControl)).AlwaysReturn ();
        fakeit::When (Method (*controller->routeController, setRoute)).AlwaysReturn ();

        layout::Route           route{ controller.get (), ROUTE_NAME, {}, ID };
        control::AutomationItem item{ route };

        item.doAction (control::AutomationItem::ACTION_SET);

        fakeit::Verify (Method (*controller->routeController, setRoute).Using (ID));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test deleting a route that has been automated
    ///
    /// @see    control::AutomationItem::destroyed()
    /// @see    control::AutomationItem::type::ROUTE
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void routeDeleteTest ()
        {
        fakeit::When (Method (*controller->routeController, removeRoute)).AlwaysReturn ();

        layout::Route           route{ controller.get (), ROUTE_NAME, {}, ID };
        control::AutomationItem item{ route };
        QSignalSpy              spy{ &item, &control::AutomationItem::destroyed };

        route.remove ();

        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the getter functions when the item contains an actuator
    ///
    /// @see    control::AutomationItem
    /// @see    control::AutomationItem::type::ACTUATOR
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void actuatorItemTest ()
        {
        layout::Actuator actuator{ controller.get (),
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   true };
        control::AutomationItem item{ actuator };

        QCOMPARE (item.getType (),      control::AutomationItem::type::ACTUATOR);
        QCOMPARE (item.name (),         ACTUATOR_NAME);
        QCOMPARE (item.getActuator (),  actuator);
        QCOMPARE (item.getActions (),
                  utils::algorithm::makeBitset<
                        control::AutomationItem::NUM_ACTIONS> (
                                control::AutomationItem::ACTION_SET,
                                control::AutomationItem::ACTION_UNSET,
                                control::AutomationItem::ACTION_TOGGLE));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for actuatorActionTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void actuatorActionTest_data ()
        {
        QTest::addColumn<bool> ("initial");
        QTest::addColumn<control::AutomationItem::action> ("action");

        for (auto action : utils::algorithm::EnumRange{ control::AutomationItem::action::ACTION_SET,
                                                        control::AutomationItem::action::NUM_ACTIONS })
            {
            auto text = control::actionText (action).toStdString ();

            QTest::addRow ("start ON, do %s",  text.c_str ()) << true << action;
            QTest::addRow ("start OFF, do %s", text.c_str ()) << false << action;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting an action for an actuator automation item
    ///
    /// @see    control::AutomationItem::doAction()
    /// @see    control::AutomationItem::type::ACTUATOR
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void actuatorActionTest ()
        {
        QFETCH (bool, initial);
        QFETCH (control::AutomationItem::action, action);

        fakeit::When (Method (*controller->actuatorController, requestActuatorControl)).AlwaysReturn ();
        fakeit::When (Method (*controller->actuatorController, releaseActuatorControl)).AlwaysReturn ();
        fakeit::When (Method (*controller->actuatorController, setActuator)).AlwaysReturn ();

        layout::Actuator actuator{ controller.get (),
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   initial };
        control::AutomationItem item{ actuator };

        item.doAction (action);

        auto [expected, shouldBeCalled] = getExpectedCallAndState (action, initial);

        if (shouldBeCalled)
            {
            fakeit::Verify (Method (*controller->actuatorController, setActuator).Using (ID, expected));
            }
        else
            {
            fakeit::Verify (Method (*controller->actuatorController, setActuator)).Never ();
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test deleting an actuator that has been automated
    ///
    /// @see    control::AutomationItem::destroyed()
    /// @see    control::AutomationItem::type::ACTUATOR
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void actuatorDeleteTest ()
        {
        fakeit::When (Method (*controller->actuatorController, removeActuator)).AlwaysReturn ();

        layout::Actuator actuator{ controller.get (),
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   true };
        control::AutomationItem item{ actuator };
        QSignalSpy              spy{ &item, &control::AutomationItem::destroyed };

        actuator.remove ();

        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the getter functions when the item contains an loco function
    ///
    /// @see    control::AutomationItem
    /// @see    control::AutomationItem::type::LOCO_FUNC
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoFunctionItemTest ()
        {
        layout::Locomotive loco{ controller.get (),
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };
        control::AutomationItem item{ loco, 3 };

        QCOMPARE (item.getType (),  control::AutomationItem::type::LOCO_FUNC);
        QCOMPARE (item.name (),     "Function 3 : Function 3");
        QCOMPARE (item.getActions (),
                  utils::algorithm::makeBitset<
                  control::AutomationItem::NUM_ACTIONS> (
                      control::AutomationItem::ACTION_SET,
                      control::AutomationItem::ACTION_UNSET,
                      control::AutomationItem::ACTION_TOGGLE));

        auto func = item.getFunction ();

        QCOMPARE_NE (func, std::nullopt);
        QCOMPARE (func->loco, loco);
        QCOMPARE (func->func, 3);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test dat for locoFunctionItemTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoFunctionActionTest_data ()
        {
        QTest::addColumn<int>  ("idx");
        QTest::addColumn<control::AutomationItem::action> ("action");

        for (auto action : utils::algorithm::EnumRange{ control::AutomationItem::action::ACTION_SET,
                                                        control::AutomationItem::action::NUM_ACTIONS })
            {
            for (int ii = 0; ii < std::size (LOCO_FUNCS); ++ii)
                {
                auto text = control::actionText (action).toStdString ();

                QTest::addRow ("Do %s to function %d",
                               text.c_str (),
                               LOCO_FUNCS[ii].id) << ii << action;
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting an action for an loco function automation item
    ///
    /// @see    control::AutomationItem::doAction()
    /// @see    control::AutomationItem::type::LOCO_FUNC
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoFunctionActionTest ()
        {
        QFETCH (int, idx);
        QFETCH (control::AutomationItem::action, action);

        fakeit::When (Method (*controller->locomotiveController, requestControl)).AlwaysReturn ();
        fakeit::When (Method (*controller->locomotiveController, releaseControl)).AlwaysReturn ();
        fakeit::When (Method (*controller->locomotiveController, setFunc)).AlwaysReturn ();

        layout::Locomotive loco{ controller.get (),
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };
        control::AutomationItem item{ loco, LOCO_FUNCS[idx].id };

        item.doAction (action);

        auto [expected, shouldBeCalled] = getExpectedCallAndState (action, LOCO_FUNCS[idx].state);

        if (shouldBeCalled)
            {
            fakeit::Verify (Method (*controller->locomotiveController, setFunc).
                            Using (ID, LOCO_FUNCS[idx].id, expected));
            }
        else
            {
            fakeit::Verify (Method (*controller->locomotiveController, setFunc)).Never ();
            }
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Test deleting a locomotive that has an automated function
    ///
    /// @see    control::AutomationItem::destroyed()
    /// @see    control::AutomationItem::type::LOCO_FUNC
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoDeleteTest ()
        {
        fakeit::When (Method (*controller->locomotiveController, removeLocomotive)).AlwaysReturn ();

        layout::Locomotive loco{ controller.get (),
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };
        control::AutomationItem item{ loco, 3 };
        QSignalSpy              spy{ &item, &control::AutomationItem::destroyed };

        loco.remove ();

        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for locoFunctionDeleteTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoFunctionDeleteTest_data ()
        {
        QTest::addColumn<int> ("idx");

        for (int ii = 0; ii < std::size (LOCO_FUNCS); ++ii)
            {
            QTest::addRow ("Delete function %d",
                           LOCO_FUNCS[ii].id) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test deleting an automated function from a locomotive
    ///
    /// @see    control::AutomationItem::destroyed()
    /// @see    control::AutomationItem::type::LOCO_FUNC
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoFunctionDeleteTest ()
        {
        QFETCH (int, idx);

        layout::Locomotive loco{ controller.get (),
                                 LOCO_NAME,
                                 layout::TRACK_PROTO_MFX,
                                 1,
                                 LOCO_FUNCS,
                                 0,
                                 ID };
        control::AutomationItem item{ loco, LOCO_FUNCS[idx].id };
        QSignalSpy              spy{ &item, &control::AutomationItem::destroyed };

        auto newFuncs = LOCO_FUNCS;

        newFuncs.erase (newFuncs.begin () + idx);

        loco.setFunctions (newFuncs);

        QCOMPARE (spy.count (), 1);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test isAutomating() with an actuator
    ///
    /// @see    control::AutomationItem::isAutomating()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void isAutomatingActuatorTest ()
        {
        layout::Actuator actuator1{ controller.get (),
                                    ACTUATOR_NAME,
                                    layout::ICON_STREET_LIGHT,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    100,
                                    ID,
                                    true };

        layout::Actuator actuator2{ controller.get (),
                                    ACTUATOR_NAME,
                                    layout::ICON_STREET_LIGHT,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    100,
                                    ID + 1,
                                    true };
        layout::Route route{ controller.get (), ROUTE_NAME, {}, ID };

        control::AutomationItem item{ actuator1 };

        // Positive test case
        QVERIFY (item.isAutomating (actuator1));

        // Negative test 1, same type, different ID
        QVERIFY (not item.isAutomating (actuator2));

        // Negative test 2, same type, different ID
        QVERIFY (not item.isAutomating (route));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test isAutomating() with a route
    ///
    /// @see    control::AutomationItem::isAutomating()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void isAutomatingRouteTest ()
        {
        layout::Route route1{ controller.get (), ROUTE_NAME, {}, ID };
        layout::Route route2{ controller.get (), ROUTE_NAME, {}, ID + 1};

        layout::Actuator actuator{ controller.get (),
                                   ACTUATOR_NAME,
                                   layout::ICON_STREET_LIGHT,
                                   layout::actuatorMode::PULSE,
                                   1,
                                   100,
                                   ID,
                                   true };

        control::AutomationItem item{ route1 };

        // Positive test case
        QVERIFY (item.isAutomating (route1));

        // Negative test 1, same type, different ID
        QVERIFY (not item.isAutomating (route2));

        // Negative test 2, same type, different ID
        QVERIFY (not item.isAutomating (actuator));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test isAutomating() with a locomotive
    ///
    /// @see    control::AutomationItem::isAutomating()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void isAutomatingLocoTest ()
        {
        static const int FUNC_ID = 10;

        layout::Locomotive loco1{ controller.get (),
                                  LOCO_NAME,
                                  layout::TRACK_PROTO_MFX,
                                  1,
                                  LOCO_FUNCS,
                                  0,
                                  ID };
        layout::Locomotive loco2{ controller.get (),
                                  LOCO_NAME,
                                  layout::TRACK_PROTO_MFX,
                                  1,
                                  LOCO_FUNCS,
                                  0,
                                  ID + 1 };

        layout::Route route{ controller.get (), ROUTE_NAME, {}, ID };

        control::AutomationItem item{ loco1, FUNC_ID };

        // Loco component Tests

        // Positive test case for loco
        QVERIFY (item.isAutomating (loco1));

        // Negative test 1, same type, different ID
        QVERIFY (not item.isAutomating (loco2));

        // Negative test 2, same type, different ID
        QVERIFY (not item.isAutomating (route));

        // Function

        // Positive test case for function
        QVERIFY (item.isAutomating (control::AutomationItem::function{ loco1, FUNC_ID }));

        // Negative test 1, same loco, different number
        QVERIFY (not item.isAutomating (control::AutomationItem::function{ loco1, FUNC_ID - 1 }));

        // Negative test 2, different loco, same number
        QVERIFY (not item.isAutomating (control::AutomationItem::function{ loco2, FUNC_ID }));
        }
    };

QTEST_GUILESS_MAIN (AutomationItemTest)

#include "itemtest.moc"
