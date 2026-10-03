/**
 * @file        virtual/vroutetest.cpp
 * @brief       Test suite for the virtual route controller
 * @author      Justin Scott
 * @date        2026-10-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#define LAYOUT_TEST_CLASS     VirtualRouteTest

#include <testutils/assert.hpp>
#include <testutils/controllerstubs.hpp>
#include <testutils/ext/fakeit.hpp>

#include <layout/virtual/vroute.hpp>

#include <QtTest>

static const size_t ID1 = 777;
static const size_t ID2 = 888;

static const std::string ROUTE_NAME = "Test Route Name";

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the virtual route controller
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class VirtualRouteTest : public QObject
    {
    Q_OBJECT
private:
    testutils::ActuatorControllerStubs                          fakeController;
    std::unique_ptr<fakeit::Mock<layout::ActuatorController>>   mockController;
    std::unique_ptr<layout::VirtualRouteController>             routeController;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a route for testing
    ///
    /// @param[out] outMembers      (Optional) Members of the created route
    ///
    /// @return     Test route
    ///
    ///////////////////////////////////////////////////////////////////////////////
    layout::Route createRoute (layout::routeList* outMembers = NULL)
        {
        layout::Actuator actuator1{ &mockController->get (),
                                    "",
                                    layout::ICON_LIGHTING,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    100,
                                    ID1,
                                    true };
        layout::Actuator actuator2{ &mockController->get (),
                                    "",
                                    layout::ICON_LIGHTING,
                                    layout::actuatorMode::PULSE,
                                    1,
                                    100,
                                    ID2,
                                    true };

        layout::routeList   members{ { actuator1, true }, { actuator2, true } };

        if (NULL != outMembers)
            {
            *outMembers = members;
            }

        return routeController->createRoute (ROUTE_NAME, members);
        }

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Init called before each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void init ()
        {
        mockController.reset (new fakeit::Mock<layout::ActuatorController>{ fakeController });
        routeController.reset (new layout::VirtualRouteController{ mockController->get () });

        fakeit::When (Method (*mockController, setActuator)).AlwaysReturn ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Cleanup called after each test case
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void cleanup ()
        {
        mockController.reset ();
        routeController.reset ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test creating a route
    ///
    /// @see    layout::VirtualRouteController::createRoute()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void createRouteTest ()
        {
        layout::routeList   members;
        layout::Route       route = createRoute (&members);

        QCOMPARE (route.getName (), ROUTE_NAME);
        COMPARE_RANGE (route.getActuators (), members);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting a route
    ///
    /// @see    layout::VirtualRouteController::setRoute()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setRouteTest ()
        {
        layout::Route route = createRoute ();

        route.set ();

        fakeit::When (Method (*mockController, setActuator).Using (ID1, true));
        fakeit::When (Method (*mockController, setActuator).Using (ID2, false));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test removing a route
    ///
    /// @see    layout::VirtualRouteController::removeRoute()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void removeRouteTest ()
        {
        layout::Route route1 = createRoute ();
        layout::Route route2 = createRoute ();
        layout::Route route3 = createRoute ();

        size_t id = route2.getId ();

        route2.remove ();

        // getRoutes() should NOT return the removed route
        COMPARE_RANGE (routeController->getRoutes (), (std::array{ route1, route3 }));

        // Creating a new route should re-use the ID from the removed route
        layout::Route route4 = createRoute ();

        QCOMPARE (route4.getId (), id);
        }
    };

QTEST_GUILESS_MAIN (VirtualRouteTest)

#include "vroutetest.moc"
