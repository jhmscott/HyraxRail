/**
 * @file        testutils/lang.hpp
 * @brief       Utilities for testing languages/localization
 * @author      Justin Scott
 * @date        2026-09-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/lang.hpp>

#include <QLocale>
#include <QString>


namespace testutils
{
/// Information about a language for testing
struct localeInfo
    {
    QLocale     locale;     ///< App locale
    QString     appName;    ///< App title in that locale
    const char* name;       ///< English name of the language to use in data driven tests
    };

///////////////////////////////////////////////////////////////////////////////
/// RAII class that sets the app locale via the translator class in it's
/// constructor and restores it in the destructor
///
///////////////////////////////////////////////////////////////////////////////
class AppLocaleGuard
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  locale      New locale
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit AppLocaleGuard (const QLocale& locale)
        {
        // Note, m_locale is default constructed before this
        // meaning it will be the original locale

        ui::lang::Translator::instance ().setLocale (locale);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~AppLocaleGuard ()
        {
        ui::lang::Translator::instance ().setLocale (m_locale);
        }

private:
    QLocale m_locale;   ///< Original locale
    };

/// List of supported app languages to test
inline localeInfo SUPPORTED_LOCALES[] =
    {
        { QLocale{ "nl_NL" }, "Klipdas Spoorwegen", "Dutch"     },
        { QLocale{ "en_US" }, "Hyrax Rail",         "English"   },
        { QLocale{ "fr" },    "Râle Daman",         "French"    },
        { QLocale{ "vi_VN" }, "Đường sắt Hyrax",    "Vietnamese"}
    };

} // namespace testutils