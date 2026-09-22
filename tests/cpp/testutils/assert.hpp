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

#include <QTextStream>
#include <QtTest>
#include <QtTypeTraits>

#include <algorithm>
#include <sstream>

/// Assertion internal details
namespace testutils::assert_internal
{

///////////////////////////////////////////////////////////////////////////////
/// Format a range to string
///
/// @tparam     Rng     Range type
///
/// @param[in]  range   Range to format
///
///////////////////////////////////////////////////////////////////////////////
template<class Rng>
std::string rangeToString (const Rng& range)
    {
    std::string str;

    if (range.begin () == range.end ())
        {
        str = "[ Empty ]";
        }
    else
        {
        using Value = typename Rng::value_type;

        constexpr bool hasTextStream= QTypeTraits::has_ostream_operator_v<QTextStream, Value>;
        constexpr bool hasOstream   = QTypeTraits::has_ostream_operator_v<std::ostream, Value>;

        constexpr bool useTextStream= hasTextStream && not hasOstream;

        using Stream = std::conditional_t<useTextStream, QTextStream, std::stringstream>;

        QString qstr;

        Stream ss;

        if constexpr (useTextStream)
            {
            ss.setString (&qstr);
            }

        ss << "[ ";

        for (const auto& val : range)
            {
            ss << val << ", ";
            }

        if constexpr (useTextStream)
            {
            qstr.chop (2);
            }
        else
            {
            ss.seekp (-2, ss.cur);
            }

        ss << " ]";

        if constexpr (useTextStream)
            {
            str = qstr.toStdString ();
            }
        else
            {
            str = ss.str ();
            }
        }

    return str;
    }

///////////////////////////////////////////////////////////////////////////////
/// Format a range assertion message
///
/// @param[in]  actual      Actual range
/// @param[in]  expected    Expected range
/// @param[in]  actualName  Name of the actual range
/// @param[in]  actualName  Name of the expected range
/// @param[in]  equals      True if the error case is actual == expected
///                         False if the error case is actual != expected
///
/// @return     Formatted assertion message
///
///////////////////////////////////////////////////////////////////////////////
template<class R1, class R2>
std::string createRangeAssertMessage (const R1&     actual,
                                      const R2&     expected,
                                      const char*   actualName,
                                      const char*   expectedName,
                                      bool          equals)
    {
    const std::string   op = equals ? " == " : " != ";
    std::string         msg;

    msg = "\n\n";

    msg += actualName + op + expectedName + "\n\n";

    msg += "Actual   : " + rangeToString (actual) + "\n";
    msg += "Expected : " + rangeToString (expected) + "\n";

    return msg;
    }
} // namespace testutils::assert_internal

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
/// Compare that two pixmaps are NOT pixel perfect identical
///
/// @param[in]  actual      Actual pixmap
/// @param[in]  expected    Expected pixmap
///
///////////////////////////////////////////////////////////////////////////////
#define COMPARE_PIXMAPS_NE(actual, expected) \
    QCOMPARE_NE ((actual).toImage (), (expected).toImage ())

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

///////////////////////////////////////////////////////////////////////////////
/// Compare two icons produce NON identical bitmaps when rendered
///
/// @param[in]  actual      Actual icon
/// @param[in]  expected    Expected icon
///
/// @remarks    Rendered at the app's default icon size
///
/// @see        utils::resources::ICON_SIZE_NORMAL
///
///////////////////////////////////////////////////////////////////////////////
#define COMPARE_ICONS_NE(actual, expected) \
    COMPARE_PIXMAPS_NE ((actual).pixmap (utils::resources::ICON_SIZE_NORMAL), \
                        (expected).pixmap (utils::resources::ICON_SIZE_NORMAL))

///////////////////////////////////////////////////////////////////////////////
/// Compare two containers/ranges
///
/// @param[in]  actual      Actual range
/// @param[in]  expected    Expected ranged
///
///////////////////////////////////////////////////////////////////////////////
#define COMPARE_RANGE(actual, expected)                                         \
    QVERIFY2 ((std::equal ((actual).begin (),                                   \
                           (actual).end (),                                     \
                           (expected).begin (),                                 \
                           (expected).end ())),                                 \
              testutils::assert_internal::createRangeAssertMessage ((actual),   \
                                                         (expected),            \
                                                         #actual,               \
                                                         #expected,             \
                                                         false).c_str ())

///////////////////////////////////////////////////////////////////////////////
/// Compare two containers/ranges
///
/// @param[in]  actual      Actual range
/// @param[in]  expected    Expected ranged
///
///////////////////////////////////////////////////////////////////////////////
#define COMPARE_RANGE_NE(actual, expected)                                      \
    QVERIFY2 (not (std::equal ((actual).begin (),                               \
                               (actual).end (),                                 \
                               (expected).begin (),                             \
                               (expected).end ())),                             \
              testutils::assert_internal::createRangeAssertMessage ((actual),   \
                                                                    (expected), \
                                                                    #actual,    \
                                                                    #expected,  \
                                                                    true).c_str ())