/**
 * @file        utils/commontest.hpp
 * @brief       Test suite for the common header functions
 * @author      Justin Scott
 * @date        2026-08-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#define _CRT_SECURE_NO_WARNINGS

#include <common.hpp>

#include <QtTest>

#include <sstream>

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the common header
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class CommonTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test the equality and inequality operations of the version struct
    ///
    /// @see    version::operator==()
    /// @see    version::operator!=()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void versionEqualityTest ()
        {
        for (uint ii = 0; ii < 20; ++ii)
            {
            for (uint jj = 0; jj < 20; ++jj)
                {
                for (uint kk = 0; kk < 20; ++kk)
                    {
                    QVERIFY ((version{ ii, jj, kk }) == (version{ ii, jj, kk }));

                    if (ii != kk)
                        {
                        QVERIFY ((version{ ii, jj, kk }) != (version{ kk, jj, ii }));
                        }

                    if (ii != jj)
                        {
                        QVERIFY ((version{ ii, jj, kk }) != (version{ jj, ii, kk }));
                        }

                    if (jj != kk)
                        {
                        QVERIFY ((version{ ii, jj, kk }) != (version{ ii, kk, jj }));
                        }
                    }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the stream formatting of the version struct
    ///
    /// @see    version::operator<<()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void versionFormattingTest ()
        {
        char expected[100];

        for (uint ii = 0; ii < 20; ++ii)
            {
            for (uint jj = 0; jj < 20; ++jj)
                {
                for (uint kk = 0; kk < 20; ++kk)
                    {
                    std::stringstream ss;

                    sprintf (expected, "%u.%u.%u", ii, jj, kk);

                    ss << version{ ii, jj, kk };

                    QCOMPARE (ss.str (), expected);
                    }
                }
            }
        }
    };


QTEST_GUILESS_MAIN (CommonTest);

#include "commontest.moc"
