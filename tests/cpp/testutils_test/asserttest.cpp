/**
 * @file        testutils_test/asserttest.hpp
 * @brief       Test suite for the test assert library
 * @author      Justin Scott
 * @date        2026-09-12
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/assert.hpp>

#include <QIcon>
#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the test assert library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class AssertTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two identical icons
    ///
    /// @see    COMPARE_ICONS()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void iconAssertTrueTest ()
        {
        QIcon icon1{ ":/icons/app/conductor-hyrax.svg" };
        QIcon icon2{ ":/icons/app/conductor-hyrax.svg" };

        COMPARE_ICONS (icon1, icon2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two non-identical icons
    ///
    /// @see    COMPARE_ICONS_NE()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void iconAssertFalseTest ()
        {
        QIcon icon1{ ":/icons/app/conductor-hyrax.svg" };
        QIcon icon2{ ":/icons/app/conductor-hyrax-mini.svg" };

        COMPARE_ICONS_NE (icon1, icon2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two identical ranges
    ///
    /// @see    COMPARE_RANGE()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rangeAssertTrueTest ()
        {
        std::vector<int> expected   = { 1, 2, 3, 4, 5 };
        std::vector<int> actual     = expected;

        COMPARE_RANGE (expected, actual);
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two non-identical ranges
    ///
    /// @see    COMPARE_RANGE_NE()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rangeAssertFalseTest ()
        {
        std::vector<int> expected   = { 1, 2, 3, 4, 5 };
        std::vector<int> actual     = { 5, 6, 7, 8, 9 };

        COMPARE_RANGE_NE (expected, actual);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test formatting an empty range to a string
    ///
    /// @see    testutils::assert_internal::rangeToString()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rangeFormatEmptyTest ()
        {
        std::vector<int> emptyRange;

        QCOMPARE (testutils::assert_internal::rangeToString (emptyRange), "[ Empty ]");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test formatting a vector to a string
    ///
    /// @see    testutils::assert_internal::rangeToString()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rangeFormatVectorTest ()
        {
        std::vector<int> vec = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

        QCOMPARE (testutils::assert_internal::rangeToString (vec),
                  "[ 1, 2, 3, 4, 5, 6, 7, 8, 9 ]");
        }

    void rangeFormatQStringTest ()
        {
        std::vector<QString> vec = { "Test 1", "Test 2" };

        QCOMPARE (testutils::assert_internal::rangeToString (vec),
                  "[ Test 1, Test 2 ]");
        }
    };

QTEST_MAIN (AssertTest)

#include "asserttest.moc"
