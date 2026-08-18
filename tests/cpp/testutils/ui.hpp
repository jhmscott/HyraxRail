
/**
 * @file        testutils/ui.hpp
 * @brief       UI testing utility functions
 * @author      Justin Scott
 * @date        2026-08-18
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/utils.hpp>

#include <QtTest>
#include <QMenu>
#include <QWidget>
#include <QWindow>

namespace testutils
{

///////////////////////////////////////////////////////////////////////////////
/// Test a context menu item
///
/// @param[in]  widget              Widget to test menu for
/// @param[in]  menuId              Menu item index to test
/// @param[in]  validateMenu        Callback to validate the opened menu
/// @param[in]  onMenuItemSelected  Called after selecting the menu item
///
///////////////////////////////////////////////////////////////////////////////
inline void testMenuItem (const QWidget&                        widget,
                          int                                   menuId,
                          std::function<void (const QMenu&)>    validateMenu,
                          std::function<void ()>                onMenuItemSelected)
    {
    QWindow* window = widget.windowHandle ();

    QCOMPARE_NE (window, NULL);

    QTimer::singleShot (10,
        [&] () -> void
        {
        QMenu* menu = ui::common::getTopLevelWidget<QMenu> ();

        QCOMPARE_NE (menu, NULL);
        validateMenu (*menu);

        // timers all the way down...
        QTimer::singleShot (10, onMenuItemSelected);

        // Open the delete dialog
        QTest::mouseClick (menu,
                           Qt::LeftButton,
                           Qt::KeyboardModifiers{},
                           menu->actionGeometry (
                                menu->actions ().at (menuId)).center ());
        });

    // This will block until the menu is dismissed,
    // so we need to manipulate the menu by queuing a timer
    QTest::mouseClick (window, Qt::RightButton);
    }
}
