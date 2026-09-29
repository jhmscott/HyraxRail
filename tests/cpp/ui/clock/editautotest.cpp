/**
 * @file        common/editautotest.hpp
 * @brief       Test suite for the automation add/edit dialog
 * @author      Justin Scott
 * @date        2026-09-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>
#include <names.hpp>

#include <testutils/mockcontroller.hpp>

#include <ui/clock/editauto.hpp>

#include <QtTest>

using namespace ui::clock;


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the automation add/edit dialog
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class EditAutoTest : public QObject
    {
    Q_OBJECT
public:
    EditAutoTest () :
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
        static const std::vector<layout::funcInfo>  FUNCS1 =
            {
                { "Function 1", layout::funcInfo::ICON_FUNC_SOUND_GENERIC,  1, true  },
                { "Function 3", layout::funcInfo::ICON_FUNC_LIGHT_CAB,      3, false },
                { "Function 5", layout::funcInfo::ICON_FUNC_MISC_SLOW,      5, true  }
            };

        static const std::vector<layout::funcInfo>  FUNCS2 =
            {
                { "Function 2", layout::funcInfo::ICON_FUNC_LIGHT_HEADLIGHT,2, false  },
                { "Function 4", layout::funcInfo::ICON_FUNC_SOUND_OPERATING,4, true   },
                { "Function 6", layout::funcInfo::ICON_FUNC_SOUND_HORN,     6, false  }
            };

        controllers.append (
            testutils::mockedControllerInfo ("Edit Auto Controller"));

        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        controller.actuators =
            {
                { &controller, "Actuator 1", layout::ICON_LIGHTING,     layout::actuatorMode::PULSE, 1, 100, 1, true },
                { &controller, "Actuator 2", layout::ICON_STREET_LIGHT, layout::actuatorMode::PULSE, 2, 100, 2, true },
            };

        controller.routes =
            {
                { &controller, "Route 1", {}, 1 },
                { &controller, "Route 2", {}, 2 },
            };

        controller.locomotives =
            {
                { &controller, "Loco 1", layout::TRACK_PROTO_DCC28, 1, FUNCS1, 1 },
                { &controller, "Loco 2", layout::TRACK_PROTO_DCC28, 2, FUNCS2, 2 },
            };
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
    /// Test populating the items combobox from a controller manager
    ///
    /// @see    ui::clock::EditAutoDialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void editAutoPopulateItemsTest ()
        {
        auto& controller = static_cast<testutils::MockController&> (controllers[0]);

        EditAutoDialog dlg{ controllers, NULL };

        auto* items = dlg.findChild<ui::common::TieredDropdown*> (OBJNAME_EDITAUTO_ITEMS);

        QCOMPARE_NE (items, NULL);

        size_t expectedCount  = 2 +                                 // Parent items for routes and actuators
                                controller.actuators.size ()    +
                                controller.routes.size ()       +
                                controller.locomotives.size ()  +   // Parent items for locomotive functions
                                std::accumulate (controller.locomotives.begin (),
                                                 controller.locomotives.end (),
                                                 0LLU,
                                                 [] (size_t count, const layout::Locomotive& loco) -> size_t
                                                 { return count + loco.getFunctions ().size (); });

        QCOMPARE (expectedCount, items->count ());

        int idx = 0;

        QCOMPARE (items->itemText (idx).trimmed (), "Actuators");
        ++idx;

        for (int ii = 0; ii < controller.actuators.size (); ++idx, ++ii)
            {
            QCOMPARE (items->itemText (idx).trimmed (),
                      controller.actuators[ii].getName ());
            }

        QCOMPARE (items->itemText (idx).trimmed (), "Routes");
        ++idx;

        for (int ii = 0; ii < controller.routes.size (); ++idx, ++ii)
            {
            QCOMPARE (items->itemText (idx).trimmed (),
                      controller.routes[ii].getName ());
            }

        for (const auto& loco : controller.locomotives)
            {
            QCOMPARE (items->itemText (idx).trimmed (), loco.getName ());
            ++idx;

            auto functions = loco.getFunctions ();

            for (int ii = 0; ii < functions.size (); ++idx, ++ii)
                {
                QCOMPARE (items->itemText (idx).trimmed (),
                          functions[ii].uiName ());
                }
            }
        }
    };

QTEST_MAIN (EditAutoTest)

#include "editautotest.moc"
