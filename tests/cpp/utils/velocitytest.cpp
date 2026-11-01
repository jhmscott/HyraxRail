/**
 * @file        utils/velocitytest.cpp
 * @brief       Test suite for the velocity calculator
 * @author      Justin Scott
 * @date        2026-10-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/velocity.hpp>

#include <QtTest>

static constexpr QVector2D ZERO_VECTOR{ 0.f, 0.f };

static const QPointF PATH_POINTS[] =
    {
    QPointF{ 2.0, -23.0 },
    QPointF{ 2.0, -31.0 },
    QPointF{ 9.0, -38.0 },
    QPointF{ 7.0, -40.0 },
    QPointF{ -9.0, -38.0 },
    QPointF{ -2.0, -31.0 },
    QPointF{ -2.0, -23.0 },
    QPointF{ 9.0, -38.0 },
    QPointF{ 7.0, -40.0 },
    QPointF{ -9.0, -38.0 },
    QPointF{ -2.0, -31.0 },
    QPointF{ -2.0, -23.0 },
    QPointF{ 0.0, -34.0 },
    QPointF{ 2.0, -31.0 },
    QPointF{ 9.0, -38.0 },
    QPointF{ -2.0, -31.0 },
    QPointF{ -7.0, -40.0 },
    QPointF{ -9.0, -38.0 },
    QPointF{ 2.0, -31.0 },
    QPointF{ 9.0, -38.0 },
    QPointF{ -2.0, -31.0 },
    QPointF{ 2.0, -31.0 },
    QPointF{ 9.0, -38.0 },
    QPointF{ -2.0, -23.0 }
    };

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the velocity calculator
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class VelocityTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests a velocity calculator with an insufficent number of points
    ///
    /// @see    utils::VelocityCalculator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void emptyCalculatorTest ()
        {
        utils::VelocityCalculator calc{ 10, 1.0 };

        // Empty calculator, zero points. Should be the zero vector
        QCOMPARE (calc.getVelocity (), ZERO_VECTOR);

        calc.addPoint ({ 100.0f, 100.0f }, 1);

        // Need at least two points. Still should be zero
        QCOMPARE (calc.getVelocity (), ZERO_VECTOR);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for twoPointTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void twoPointTest_data ()
        {
        QTest::addColumn<QPointF> ("point1");
        QTest::addColumn<QPointF> ("point2");

        QTest::addColumn<ulong> ("time1");
        QTest::addColumn<ulong> ("time2");

        static const ulong      TIME_CONSTANTS[]    = { 0, 1, 2, 3, 5, 7, 11, 13, 17 };
        static const QPointF    POINT_CONSTANSTS[]  =
            {
                // Origin
                { 0.0f, 0.0f },

                // Along x and y axis
                { 5.0f, 0.0f },
                { 0.0f, 5.0f },
                {-5.0f, 0.0f },
                { 0.0f,-5.0f },

                // Each quadrant
                { 5.0f, 5.0f },
                { 5.0f,-5.0f },
                {-5.0f,-5.0f },
                {-5.0f, 5.0f },
            };

        for (const ulong time1 : TIME_CONSTANTS)
            {
            for (int delta = 1; delta < std::size (TIME_CONSTANTS); ++delta)
                {
                const ulong time2 = time1 + TIME_CONSTANTS[delta];

                for (const QPointF& start : POINT_CONSTANSTS)
                    {
                    for (const QPointF& end : POINT_CONSTANSTS)
                        {
                        QTest::addRow ("(%g,%g) -> (%g,%g) at %ul ms for %ul ms",
                                       start.x (),
                                       start.y (),
                                       end.x (),
                                       end.y (),
                                       time1,
                                       delta)
                            << start
                            << end
                            << time1
                            << time2;
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Simplest case, only two points to calculate velocity from. This is just basic
    /// algebra
    ///
    /// @see    utils::VelocityCalculator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void twoPointTest ()
        {
        QFETCH (QPointF, point1);
        QFETCH (QPointF, point2);
        QFETCH (ulong, time1);
        QFETCH (ulong, time2);

        utils::VelocityCalculator calc{ 10, 1.0 };

        calc.addPoint (point1, time1);
        calc.addPoint (point2, time2);

        QVector2D velocity = calc.getVelocity ();

        ulong deltaT = time2 - time1;

        QCOMPARE (velocity.x (), (point2.x () - point1.x ()) / deltaT);
        QCOMPARE (velocity.y (), (point2.y () - point1.y ()) / deltaT);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for pointLimitTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void pointLimitTest_data ()
        {
        QTest::addColumn<size_t> ("maxPoints");

        for (size_t ii = 2; ii < std::size (PATH_POINTS) - 1; ++ii)
            {
            QTest::addRow ("%d", ii) << ii;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests setting a point limit
    ///
    /// @see    utils::VelocityCalculator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void pointLimitTest ()
        {
        QFETCH (size_t, maxPoints);

        utils::VelocityCalculator actual{ maxPoints, 0.9 };
        utils::VelocityCalculator reference{ std::size (PATH_POINTS), 0.9 };

        for (size_t ii = 0; ii < std::size (PATH_POINTS); ++ii)
            {
            actual.addPoint (PATH_POINTS[ii], static_cast<ulong> (ii));

            if (ii > (std::size (PATH_POINTS) - maxPoints - 1))
                {
                reference.addPoint (PATH_POINTS[ii], static_cast<ulong> (ii));
                }
            }

        // Limiting to N points should give the same result as just using the last N points
        QCOMPARE (actual.getVelocity (), reference.getVelocity ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for decayTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void decayTest_data ()
        {
        QTest::addColumn<double> ("decay");

        for (double decay = 0.1; decay < 1.0; decay += 0.1)
            {
            QTest::addRow ("%.lf", decay) << decay;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the decay parameter
    ///
    /// @see    utils::VelocityCalculator
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void decayTest ()
        {
        QFETCH (double, decay);

        utils::VelocityCalculator calc{ std::size (PATH_POINTS), decay };

        for (size_t ii = 0; ii < std::size (PATH_POINTS); ++ii)
            {
            calc.addPoint (PATH_POINTS[ii], static_cast<ulong> (ii));
            }

        QVector2D expected{ 0.0f, 0.0f };
        double    divisor   = 0.0;
        double    weight    = 1.0;

        for (size_t ii = std::size (PATH_POINTS) - 1; ii > 0; --ii)
            {
            utils::VelocityCalculator twoPointCalc{ 2, 1.0 };

            twoPointCalc.addPoint (PATH_POINTS[ii - 1], 0);
            twoPointCalc.addPoint (PATH_POINTS[ii], 1);

            expected += twoPointCalc.getVelocity () * weight;
            divisor  += weight;
            weight   *= decay;
            }

        expected /= divisor;

        QCOMPARE (calc.getVelocity (), expected);
        }
    };

QTEST_GUILESS_MAIN (VelocityTest)

#include "velocitytest.moc"
