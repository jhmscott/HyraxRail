/**
 * @file        layout/basetest.hpp
 * @brief       Test suite for the component and controller base classes
 * @author      Justin Scott
 * @date        2026-09-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <layout/base.hpp>

#include <testutils/assert.hpp>

#include <QtTest>

// forward declare
class TrivialController;

/// Empty state struct
struct TrivialState
    {};

/// Component with no state associated with it
using TrivialComponent = layout::ComponentDerived<TrivialController, TrivialState>;

/// Controller for TrivialComponents
class TrivialController : public layout::ControllerBase<TrivialComponent> {};



///////////////////////////////////////////////////////////////////////////////
/// Test suite for the component and controller base classes
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class LayoutBaseTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the == and != operators for the components
    ///
    /// @see    layout::ComponentDerived::operator==
    /// @see    layout::ComponentDerived::operator!=
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void equalityTest ()
        {
        TrivialController controller1;
        TrivialController controller2;

        // Controller 1 components
        TrivialComponent components1[] =
            {
                { &controller1, 1, std::make_shared<TrivialState> () },
                { &controller1, 2, std::make_shared<TrivialState> () },
                { &controller1, 3, std::make_shared<TrivialState> () }
            };

        // component 2 components
        TrivialComponent components2[] =
            {
                { &controller2, 1, std::make_shared<TrivialState> () },
                { &controller2, 2, std::make_shared<TrivialState> () },
                { &controller2, 3, std::make_shared<TrivialState> () }
            };

        TrivialComponent components1Copy[std::size (components1)];
        TrivialComponent components2Copy[std::size (components2)];

        std::copy (components1,
                   components1 + std::size (components1),
                   components1Copy);

        std::copy (components2,
                   components2 + std::size (components2),
                   components2Copy);

        for (size_t ii = 0; ii < std::size (components1); ++ii)
            {
            // Components with the same ID, but different controllers should not be equal
            COMPARE_OP_NE (components1[ii],     components2[ii]);
            COMPARE_OP_NE (components1Copy[ii], components2Copy[ii]);

            // Comparison against self (equal)
            COMPARE_OP_EQ (components1[ii],     components1[ii]);
            COMPARE_OP_EQ (components2[ii],     components2[ii]);
            COMPARE_OP_EQ (components1Copy[ii], components1Copy[ii]);
            COMPARE_OP_EQ (components2Copy[ii], components2Copy[ii]);

            // Comparison against a copy (equal)
            COMPARE_OP_EQ (components1[ii], components1Copy[ii]);
            COMPARE_OP_EQ (components2[ii], components2Copy[ii]);
            }

        // Same controller but different IDs (not equal)
        COMPARE_OP_NE (components1[0], components1[1]);
        COMPARE_OP_NE (components1[0], components1[2]);
        COMPARE_OP_NE (components1[1], components1[2]);

        COMPARE_OP_NE (components2[0], components2[1]);
        COMPARE_OP_NE (components2[0], components2[2]);
        COMPARE_OP_NE (components2[1], components2[2]);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test of the less than operator
    ///
    /// @see    layout::ComponentDerived::operator<
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void lessThanTest ()
        {
        TrivialController controller1;
        TrivialController controller2;

        // Controller 1 components
        TrivialComponent components1[] =
            {
                { &controller1, 1, std::make_shared<TrivialState> () },
                { &controller1, 2, std::make_shared<TrivialState> () },
                { &controller1, 3, std::make_shared<TrivialState> () },
                { &controller1, 4, std::make_shared<TrivialState> () }
            };

        // component 2 components
        TrivialComponent components2[] =
            {
                { &controller2, 1, std::make_shared<TrivialState> () },
                { &controller2, 2, std::make_shared<TrivialState> () },
                { &controller2, 3, std::make_shared<TrivialState> () },
                { &controller2, 4, std::make_shared<TrivialState> () }
            };

        bool stackIncreasing = &controller2 > &controller1;

        // Controller has precedence over ID,
        // everything in controller1 should be less than controller 2
        for (size_t ii = 0; ii < std::size (components1); ++ii)
            {
            for (size_t jj = 0; jj < std::size (components1); ++jj)
                {
                if (stackIncreasing)
                    {
                    QCOMPARE_LT (components1[ii], components2[jj]);
                    }
                else
                    {
                    QCOMPARE_LT (components2[ii], components1[jj]);
                    }
                }
            }

        // Same controller, only ID should matter for sorting

        for (size_t ii = 0; ii < std::size (components1); ++ii)
            {
            for (size_t jj = ii + 1; jj < std::size (components1); ++jj)
                {
                QCOMPARE_LT (components1[ii], components1[jj]);
                }
            }
        }
    };

QTEST_GUILESS_MAIN (LayoutBaseTest)

#include "basetest.moc"
