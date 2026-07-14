/**
 * @file        ui/lang.hpp
 * @brief       Langauge and locale helpers
 * @author      Justin Scott
 * @date        2026-05-29
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/lang.hpp>

#include <utils/log.hpp>

#include <QApplication>
#include <QLibraryInfo>
#include <Qtimer>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // Q_OS_WIN

#ifdef Q_OS_MACOS
#include <../objective-c/lang.hpp>
#endif // Q_OS_MACOS

namespace ui::lang
{

Translator& Translator::instance ()
    {
    static Translator globalInstance;

    return globalInstance;
    }

void Translator::setLocale (const QLocale& locale)
    {
    QString translationLibray = QLibraryInfo::path (QLibraryInfo::TranslationsPath);

    QLocale::setDefault (locale);

    if (QLocale::English == locale.language ())
        {
        qApp->removeTranslator (&m_sysTranslator);
        qApp->removeTranslator (&m_appTranslator);
        }
    // don't install translators if we're already in English
    else
        {
        if (m_sysTranslator.load (locale,
                                  "qt",
                                  "_",
                                  translationLibray))
            {
            if (qApp->installTranslator (&m_sysTranslator))
                {
                qDebug () << "Installed qt_" << locale.name () << ".qm";
                }
            else
                {
                qDebug () << "Failed to install qt_" << locale.name () << ".qm";
                }
            }
        else
            {
            qDebug () << "No system translation for language" << locale.name ();
            }

        if (m_appTranslator.load (locale,
                                  "HyraxRail",
                                  "_",
                                  ":/translations"))
            {
            if (qApp->installTranslator (&m_appTranslator))
                {
                qDebug () << "Installed HyraxRail_" << locale.name () << ".qm";
                }
            else
                {
                qDebug () << "Failed to install HyraxRail_" << locale.name () << ".qm";
                }
            }
        else
            {
            qDebug () << "No app translation for language" << locale.name ();
            }
        }
    }
#ifdef Q_OS_MACOS
void Translator::onLanguageChanged ()
    {
    // Introduce a delay, because sometimes the locale returns the old value initially
    QTimer::singleShot (1000,
                        [this] () -> void
                        { setLocale (); });
    }
#endif // Q_OS_MACOS

QLocale system ()
    {
    QLocale locale;

#ifdef Q_OS_WIN
    wchar_t name[LOCALE_NAME_MAX_LENGTH];

    if (0 == GetUserDefaultLocaleName (name, std::size (name)))
        {
        logWinWarning (GetUserDefaultLocaleName);
        locale  = QLocale::system ();
        }
    else
        {
        locale = QLocale{ name };
        }
#endif // Q_OS_WIN

#ifdef Q_OS_MACOS
    locale = QLocale{ apple::systemLocale ().c_str () };
#endif // Q_OS_MACOS

    return locale;
    }

bool uses24HourFormat (const QLocale& locale)
    {
    QString timeFormat = locale.timeFormat (QLocale::ShortFormat);

    // If it contains 'H' (0-23 format) or doesn't contain 'a'/'p' (AM/PM markers), it's 24h
    return timeFormat.contains ('H') ||
       not timeFormat.contains ('a', Qt::CaseInsensitive);
    }

} // namespace ui::lang
