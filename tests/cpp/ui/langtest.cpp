/**
 * @file        ui/lang.hpp
 * @brief       Test suite for the UI language utilities
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
class LangTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for uses24HourTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void uses24HourTest_data ()
        {
        QTest::addColumn<QLocale>   ("locale");
        QTest::addColumn<bool>      ("is24Hour");

        // Languages that use 24 hour format
        QTest::newRow ("French")    << QLocale{ "fr_CA" } << true;
        QTest::newRow ("German")    << QLocale{ "de_DE" } << true;
        QTest::newRow ("Dutch")     << QLocale{ "nl_NL" } << true;
        QTest::newRow ("Japanese")  << QLocale{ "ja_JP" } << true;

        // Languages that use 12 hour format
        QTest::newRow ("English")   << QLocale{ "en_US" } << false;
        QTest::newRow ("Tagalog")   << QLocale{ "tl_PH" } << false;
        QTest::newRow ("Hindi")     << QLocale{ "hi_IN" } << false;
        QTest::newRow ("Bengali")   << QLocale{ "bn_IN" } << false;
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the uses24HourFormat() function
    ///
    /// @see    ui::lang::uses24HourFormat()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void uses24HourTest ()
        {
        QFETCH (QLocale, locale);
        QFETCH (bool, is24Hour);

        QCOMPARE (ui::lang::uses24HourFormat (locale), is24Hour);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for translatorSetLocaleTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translatorSetLocaleTest_data ()
        {
        QTest::addColumn<QLocale> ("locale");
        QTest::addColumn<QString> ("translated");

        for (const auto& [locale, title, language] : testutils::SUPPORTED_LOCALES)
            {
            QTest::newRow (language) << locale  << title;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Basic test of setting the application locale via the translator singleton
    /// instance by checking the app name.
    ///
    /// @see    ui::lang::Translator::instance ()::setLocale()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void translatorSetLocaleTest ()
        {
        QFETCH (QLocale, locale);
        QFETCH (QString, translated);

        testutils::AppLocaleGuard grd{locale};

        QString appname = QApplication::applicationDisplayName ();

        // Check that the tanslators were installed correctly by
        // checking the application name is translated
        QCOMPARE (appname, translated);

        // This setLocale() also updates the default locale,
        // so check a default constructed QLocale
        QCOMPARE (QLocale{}, locale);
        }
    };

QTEST_MAIN (LangTest)

#include "langtest.moc"
