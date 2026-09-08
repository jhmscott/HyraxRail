/**
 * @file        utils/algorithmtest.hpp
 * @brief       Test suite for the algorithm library
 * @author      Justin Scott
 * @date        2026-08-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/algorithm.hpp>

#include <QtTest>

using namespace utils::algorithm;

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the algorithm library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class AlgorithmTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the utils::algorithm::makeBitset() function
    ///
    /// @see    utils::algorithm::makeBitset()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void makeBitsetTest ()
        {
        static constexpr int NUM_BITS = 10;

        std::bitset<NUM_BITS> actual;

        for (int ii = 0; ii < NUM_BITS; ++ii)
            {
            for (int jj = 0; jj < NUM_BITS; ++jj)
                {
                for (int kk = 0; kk < NUM_BITS; ++kk)
                    {
                    makeBitset (actual, ii, jj, kk);

                    for (int bit = 0; bit < NUM_BITS; ++bit)
                        {
                        QCOMPARE (actual[bit],
                                  bit == ii ||
                                  bit == jj ||
                                  bit == kk);
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the utils::algorithm::bitsetToSet() function
    ///
    /// @see    utils::algorithm::bitsetToSet()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void bitsetToSetTest ()
        {
        static constexpr int NUM_BITS = 10;

        std::bitset<NUM_BITS> bits;

        for (int ii = 0; ii < NUM_BITS; ++ii)
            {
            for (int jj = 0; jj < NUM_BITS; ++jj)
                {
                for (int kk = 0; kk < NUM_BITS; ++kk)
                    {
                    makeBitset (bits, ii, jj, kk);

                    std::set<size_t> set = bitsetToSet (bits);

                    for (int bit = 0; bit < NUM_BITS; ++bit)
                        {
                        QCOMPARE (bits[bit], set.end () != set.find (bit));
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the contains function on a C array
    ///
    /// @see     utils::algorithm::contains()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void containsTest ()
        {
        static const int TEST_ARRAY[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

        QVERIFY (contains (TEST_ARRAY, 0));
        QVERIFY (contains (TEST_ARRAY, 1));
        QVERIFY (contains (TEST_ARRAY, 5));

        QVERIFY (not contains (TEST_ARRAY, 12));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the findArrayIdx function on a C array
    ///
    /// @see     utils::algorithm::findArrayIdx()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void findArrayIdxTest ()
        {
        static const int TEST_ARRAY[] = { 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

        QCOMPARE (findArrayIdx (TEST_ARRAY, 0), 0);
        QCOMPARE (findArrayIdx (TEST_ARRAY, 1), 1);
        QCOMPARE (findArrayIdx (TEST_ARRAY, 1), 1);
        QCOMPARE (findArrayIdx (TEST_ARRAY, 55), std::size (TEST_ARRAY) - 1);

        QCOMPARE (findArrayIdx (TEST_ARRAY, 12), std::size (TEST_ARRAY));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Simple test of the EnumRange and it's associated iterator
    ///
    /// @see    utils::algorithm::EnumRange
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void enumRangeTest ()
        {
        enum test
            {
            TEST_1,
            TEST_2,
            TEST_3,
            TEST_4,
            TEST_5
            };

        EnumRange range{ TEST_1, TEST_5 };

        // Test of begin and end
        QCOMPARE (*range.begin (), TEST_1);
        QCOMPARE (*range.end (), TEST_5);

        auto it = range.begin ();

        // Test comparison operators
        QCOMPARE (it, range.begin ());
        QCOMPARE_NE (it, range.end ());

        // Test iterator advancement
        ++it;
        QCOMPARE (*it, TEST_2);

        it++;
        QCOMPARE (*it, TEST_3);

        ++it;
        QCOMPARE (*it, TEST_4);

        it++;
        QCOMPARE (*it, TEST_5);

        QCOMPARE (it, range.end ());
        QCOMPARE_NE (it, range.begin ());


        it--;
        QCOMPARE (*it, TEST_4);

        --it;
        QCOMPARE (*it, TEST_3);

        --it;
        QCOMPARE (*it, TEST_2);

        --it;
        QCOMPARE (*it, TEST_1);

        QCOMPARE (it, range.begin ());
        QCOMPARE_NE (it, range.end ());

        // Test advancement iterator order
        QCOMPARE (it++, range.begin ());

        auto it2 = it = range.begin ();
        ++it2;

        QCOMPARE (++it, it2);
        }
    };


QTEST_GUILESS_MAIN (AlgorithmTest);

#include "algorithmtest.moc"
