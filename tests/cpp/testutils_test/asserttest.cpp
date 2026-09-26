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

struct comparisonTestStruct
    {
    int num;

    bool operator== (const comparisonTestStruct& other) const { return num == other.num; }

    bool operator!= (const comparisonTestStruct& other) const { return not (other == *this); }
    };

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

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests with a type that isn't ostream formattable, but is QTextStream
    /// formattable. Uses QString as it's a simple example
    ///
    /// @see    testutils::assert_internal::rangeToString()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void rangeFormatQStringTest ()
        {
        std::vector<QString> vec = { "Test 1", "Test 2" };

        QCOMPARE (testutils::assert_internal::rangeToString (vec),
                  "[ Test 1, Test 2 ]");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareOpEqualTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareOpEqualTest_data ()
        {
        QTest::addColumn<int> ("num");

        for (int ii = 0; ii < 1000; ++ii)
            {
            QTest::addRow ("%d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the operator==/operator!= assert COMPARE_OP_EQ()
    ///
    /// @see    COMPARE_OP_EQ()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareOpEqualTest ()
        {
        QFETCH (int, num);

        comparisonTestStruct s1{ num };
        comparisonTestStruct s2{ num };

        COMPARE_OP_EQ (s1, s2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareOpNotEqualTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareOpNotEqualTest_data ()
        {
        QTest::addColumn<int> ("num1");
        QTest::addColumn<int> ("num2");

        for (int ii = 0; ii < 100; ++ii)
            {
            for (int jj = 0; jj < 100; ++jj)
                {
                if (ii != jj)
                    {
                    QTest::addRow ("%d != %d", ii, jj) << ii << jj;
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the operator==/operator!= assert COMPARE_OP_NE()
    ///
    /// @see    COMPARE_OP_NE()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareOpNotEqualTest ()
        {
        QFETCH (int, num1);
        QFETCH (int, num2);

        comparisonTestStruct s1{ num1 };
        comparisonTestStruct s2{ num2 };

        COMPARE_OP_NE (s1, s2);
        }
    };

QTEST_MAIN (AssertTest)

#include "asserttest.moc"
