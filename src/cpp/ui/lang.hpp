/**
 * @file        ui/lang.hpp
 * @brief       Langauge and locale helpers
 * @author      Justin Scott
 * @date        2026-05-29
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QTranslator>
#include <QEvent>

///////////////////////////////////////////////////////////////////////////////
/// Generates boiler plate to handle a language change. Only use if you aren't
/// already using QWidget::changeEvent()
///
/// @param[in]  base        Base class type
/// @param[in]  body        Body of language change handler
///
///////////////////////////////////////////////////////////////////////////////
#define UILANG_ON_CHANGE(base, body)                    \
    virtual void changeEvent (QEvent* event) override   \
        {                                               \
        switch (event->type ())                         \
            {                                           \
            case QEvent::LanguageChange:                \
                {                                       \
                body;                                   \
                break;                                  \
                }                                       \
            }                                           \
        base::changeEvent (event);                      \
        }

namespace ui::lang
{

///////////////////////////////////////////////////////////////////////////////
/// Get the system locale. Unlike QLocale::system(), this will get the system
/// language dynamically, meaning it can be used if the language changes after
/// the app starts
///
/// @return     System locale
///
/// @todo       Currently only implemented on windows. For other systems, this
///             behaves the same as QLocale::system()
///
///////////////////////////////////////////////////////////////////////////////
QLocale system ();


///////////////////////////////////////////////////////////////////////////////
/// Singleton to manage the app translators
///
///////////////////////////////////////////////////////////////////////////////
class Translator
    {
public:
    // Non copyable
    Translator (const Translator&) = delete;
    Translator& operator= (const Translator&) = delete;

    // non movable
    Translator (Translator&&) = delete;
    Translator& operator= (Translator&&) = delete;

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the translator singleton instance
    ///
    /// @return     Singleton instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static Translator& instance ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the translators to a new locale
    ///
    /// @param[in]  locale      (optional) New locale. Defaults to system locale
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocale (const QLocale& locale = system ());

private:
    QTranslator     m_sysTranslator;    ///< QT translator
    QTranslator     m_appTranslator;    ///< App translator

    ///////////////////////////////////////////////////////////////////////////////
    /// Default constructor. Private to ensure singleton
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Translator () = default;
    };


///////////////////////////////////////////////////////////////////////////////
/// Check if a given local uses a 24 hour or 12 hour clock format
///
/// @param[in]  locale      Locale to check
///
/// @return     True if locale uses a 24 hour format
///             False if locale usea a 12 hour format
///
///////////////////////////////////////////////////////////////////////////////
bool uses24HourFormat (const QLocale& locale);


} // namespace ui::lang