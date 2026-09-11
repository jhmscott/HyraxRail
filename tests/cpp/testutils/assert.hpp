/**
 * @file        testutils/assert.hpp
 * @brief       Custom test case assertions
 * @author      Justin Scott
 * @date        2026-09-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <utils/resources.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Compare that two pixmaps are pixel perfect identical
///
/// @param[in]  actual      Actual pixmap
/// @param[in]  expected    Expected pixmap
///
///////////////////////////////////////////////////////////////////////////////
#define COMPARE_PIXMAPS(actual, expected) \
    QCOMPARE ((actual).toImage (), (expected).toImage ())

///////////////////////////////////////////////////////////////////////////////
/// Compare two icons produce identical bitmaps when rendered
///
/// @param[in]  actual      Actual icon
/// @param[in]  expected    Expected icon
///
/// @remarks    Rendered at the app's default icon size
///
/// @see        utils::resources::ICON_SIZE_NORMAL
///
///////////////////////////////////////////////////////////////////////////////
#define COMPARE_ICONS(actual, expected) \
    COMPARE_PIXMAPS ((actual).pixmap (utils::resources::ICON_SIZE_NORMAL), \
                    (expected).pixmap (utils::resources::ICON_SIZE_NORMAL))
