/**
 * @file        utils/mathtest.hpp
 * @brief       Test suite for the math functions
 * @author      Justin Scott
 * @date        2026-08-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/math.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the math utility library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class MathTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the divCeil() function
    ///
    /// @see    utils::math::divCeil()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void divideCeilingTest ()
        {
        for (int ii = 0; ii < 5000; ++ii)
            {
            // start at 1, disallow integer division by zero
            for (long jj = 1; jj < 5000; ++jj)
                {
                double id = ii;
                double jd = jj;

                // compare integer only algorithm with using floating point
                QCOMPARE (utils::math::divCeil (ii, jj),
                          static_cast<int> (std::ceil (id / jd)));
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the utils::math::roundToInt() function
    ///
    /// @see    utils::math::roundToInt()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void roundTest ()
        {
        for (double ii = 0.0; ii < 4500.0; ii += 0.123456)
            {
            QCOMPARE (utils::math::roundToInt (ii),
                      std::round (ii));
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the utils::math::sqr() function
    ///
    /// @see    utils::math::sqr()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void squareTest ()
        {
        for (double ii = 0.0; ii < 4500.0; ii += 0.1)
            {
            QCOMPARE (utils::math::sqr (ii), std::pow (ii, 2));
            }
        }


};

QTEST_GUILESS_MAIN (MathTest)

#include "mathtest.moc"
