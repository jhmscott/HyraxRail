/**
 * @file        mainwindow.cpp
 * @brief       Hyrax rail main application window
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/lang.hpp>
#include <ui/mainwidget.hpp>
#include <ui/mainwindow.hpp>

#include <QApplication>
#include <QEvent>
#include <QTabWidget>

#include <res/version.h>

#include <utils/os.hpp>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // Q_OS_WIN


namespace ui
{
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    {
    setTitle ();
    setAttribute (Qt::WA_SetLocale);

    setCentralWidget (new ui::MainWidget{ this });
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

MainWindow::~MainWindow()
    {}

void MainWindow::changeEvent (QEvent* event)
    {
    switch (event->type ())
        {
        // Handles a call to setLocale()
        case QEvent::LocaleChange:
            {
            // This triggers the QEvent::LanguageChange event globally
            ui::lang::Translator::instance ().setLocale (locale ());
            break;
            }

        // Handles a change to the app translators
        case QEvent::LanguageChange:
            {
            setTitle ();
            break;
            }
        }

    QMainWindow::changeEvent (event);
    }

bool MainWindow::nativeEvent (const QByteArray& eventType, void* message, qintptr* result)
    {
#ifdef Q_OS_WIN
    MSG& msg = *static_cast<MSG*> (message);

    switch (msg.message)
        {
        case WM_SETTINGCHANGE:
            {
            // Detect system language change
            //
            // wParam : When the system sends this message as a result of a
            // change in locale settings, this parameter is zero.
            //
            // lParam : When the system sends this message as a result of a change in
            // locale settings, this parameter points to the string "intl".
            //
            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-settingchange
            //
            if (0 == msg.wParam &&
                0 != msg.lParam &&
                0 == wcscmp (L"intl",
                             reinterpret_cast<const wchar_t*> (msg.lParam)))
                {
                QLocale newLocale = ui::lang::system ();

                // This gets called multiple times for each language change
                // Only respond to the first
                //
                if (locale () != newLocale)
                    {
                    // This triggers the QEvent::LocaleChange event
                    // for this and all child widgets
                    //
                    setLocale (newLocale);
                    }
                }
            break;
            }
        }
#endif // Q_OS_WIN

    return false;
    }

void MainWindow::setTitle ()
    {
    const QString appName = tr ("Hyrax Rail");

    QApplication::setApplicationDisplayName (appName);
    setWindowTitle (appName);
    }

}
