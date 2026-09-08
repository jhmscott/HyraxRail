/**
 * @file        utils/pptest.hpp
 * @brief       Test suite for the pre-processor utility library
 * @author      Justin Scott
 * @date        2026-09-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/pp.hpp>

#include <QtTest>

using namespace std::string_literals;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the pre-processor utility library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class PreProcessorTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test the UTILPP_VA_NUM_ARGS macro
    ///
    /// @see    UTILPP_VA_NUM_ARGS()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void numArgsTest ()
        {
        QCOMPARE (UTILPP_VA_NUM_ARGS (test1), 1);
        QCOMPARE (UTILPP_VA_NUM_ARGS (test1, test2), 2);
        QCOMPARE (UTILPP_VA_NUM_ARGS (test1, test2, test3), 3);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the UTILPP_STRINGIFY macro
    ///
    /// @see    UTILPP_STRINGIFY()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void stringifyTest ()
        {
        QCOMPARE (UTILPP_STRINGIFY (Test1), "Test1"s);
        QCOMPARE (UTILPP_STRINGIFY (Test2), "Test2"s);
        QCOMPARE (UTILPP_STRINGIFY (Test with spaces),
                  "Test with spaces"s);
        }
    };

QTEST_GUILESS_MAIN (PreProcessorTest)

#include "pptest.moc"
