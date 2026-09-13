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
    };

QTEST_MAIN (AssertTest)

#include "asserttest.moc"
