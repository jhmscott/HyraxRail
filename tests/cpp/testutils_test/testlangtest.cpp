/**
 * @file        testutils_test/testlangtest.hpp
 * @brief       Test suite for the UI language test utilities
 * @author      Justin Scott
 * @date        2026-09-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <testutils/lang.hpp>

#include <ui/lang.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the UI language utilities
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class TestLangTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for appLangGuardTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void appLangGuardTest_data ()
        {
        QTest::addColumn<QLocale> ("locale");

        for (const auto& [locale, _, language] : testutils::SUPPORTED_LOCALES)
            {
            QTest::newRow (language) << locale;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the ability of AppLocaleGuard to set the locale, and to restore it
    /// when it destructs
    ///
    /// @see    testutils::AppLocaleGuard
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void appLangGuardTest ()
        {
        QFETCH (QLocale, locale);

        QLocale original;

        {
        testutils::AppLocaleGuard grd{ locale };

        QCOMPARE (QLocale{}, locale);
        }

        QCOMPARE (QLocale{}, original);
        }
    };

QTEST_MAIN (TestLangTest)

#include "testlangtest.moc"
