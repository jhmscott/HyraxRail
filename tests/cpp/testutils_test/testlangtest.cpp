/**
 * @file        testutils_test/testlangtest.hpp
 * @brief       Test suite for the UI language test utilities
 * @author      Justin Scott
 * @date        2026-09-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <testutils/assert.hpp>
#include <testutils/lang.hpp>

#include <ui/lang.hpp>

#include <QDir>
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

    ///////////////////////////////////////////////////////////////////////////////
    /// Smoke test to keep the SUPPORTED_LOCALES constant in sync with the
    /// translation files
    ///
    /// @see    testutils::SUPPORTED_LOCALES
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void supportedLocaleTest ()
        {
        static const QString PREFIX = "HyraxRail_";
        static const QString EXT    = ".qm";

        auto translationFiles   = QDir{ ":/translations" }.entryList ();

        std::vector<QString> supported;
        std::vector<QString> translations;

        supported.reserve (std::size (testutils::SUPPORTED_LOCALES));
        translations.reserve (translationFiles.size () + 1);

        // Extract locale codes from the locale info
        std::transform (testutils::SUPPORTED_LOCALES,
                        testutils::SUPPORTED_LOCALES +
                        std::size (testutils::SUPPORTED_LOCALES),
                        std::back_inserter (supported),
                        [] (const testutils::localeInfo& info) -> QString
                        { return info.locale.name (); });

        // Extract locale code from the translation file names
        std::transform (translationFiles.begin (),
                        translationFiles.end (),
                        std::back_inserter (translations),
            [] (const QString& path) -> QString
            {
            qsizetype start = path.indexOf (PREFIX) + PREFIX.size ();
            qsizetype end   = path.indexOf (EXT);

            // Extract the locale code
            QString localeCode = path.sliced (start, end - start);

            // Normalize the locale
            return QLocale{ localeCode }.name ();
            });

        // English is built in, no translation file available
        translations.push_back ("en_US");

        // Sort both so we can do a straight comparison
        std::sort (supported.begin (),
                   supported.end ());
        std::sort (translations.begin (),
                   translations.end ());

        COMPARE_RANGE (supported, translations);
        }
    };

QTEST_MAIN (TestLangTest)

#include "testlangtest.moc"
