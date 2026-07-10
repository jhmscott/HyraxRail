/**
 * @file        mainwindow.cpp
 * @brief       Hyrax rail main application window
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/automation/clock.hpp>

#include <ui/lang.hpp>
#include <ui/mainwidget.hpp>
#include <ui/mainwindow.hpp>

#include <QApplication>
#include <QCheckBox>
#include <QEvent>
#include <QMessageBox>
#include <QSessionManager>
#include <QTabWidget>

#include <qevent.h>

#include <res/version.h>

#include <utils/os.hpp>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>    // Needed for windows native events
#endif // Q_OS_WIN


using namespace std::chrono_literals;


namespace ui
{

MainWindow::MainWindow (QWidget *parent) :
    QMainWindow (parent)
    {
    restoreFastClockSettings ();

    setTitle ();
    setAttribute (Qt::WA_SetLocale);

    setCentralWidget (new ui::MainWidget{ this });
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect (qApp,
            &QGuiApplication::commitDataRequest,
             this,
            &MainWindow::commitDataRequest,
             Qt::DirectConnection);
    }


MainWindow::~MainWindow ()
    {
    saveFastClockSettings ();
    }


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

        default:
            {
            // Don't care
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


void MainWindow::closeEvent (QCloseEvent* event)
    {
    if (not handleClose ())
        {
        event->ignore ();
        }
    }


void MainWindow::setTitle ()
    {
    const QString appName = tr ("Hyrax Rail");

    QApplication::setApplicationDisplayName (appName);
    setWindowTitle (appName);
    }


bool MainWindow::handleClose ()
    {
    bool    allowClose  = true;
    auto&   clock       = control::FastClock::instance ();

    if (clock.isRunning () &&
        clockShutdownType::NOT_SET == m_clockShutdownType)
        {
        QMessageBox msg{ this };

        msg.setWindowTitle (tr ("Shutdown"));
        msg.setText (tr ("The Fast Clock is still running.\n"
                         "Would you like to pause it or keep it running?"));

        msg.addButton (tr ("Run"),      QMessageBox::YesRole);
        msg.addButton (tr ("Pause"),    QMessageBox::NoRole);
        msg.addButton (tr ("Cancel"),   QMessageBox::RejectRole);

        msg.setCheckBox (new QCheckBox{ tr ("Always do this on shutdown"), this });
        msg.setIcon (QMessageBox::Question);

        msg.exec ();

        switch (msg.buttonRole (msg.clickedButton ()))
            {
            case QMessageBox::YesRole:      // Run
                {
                m_clockShutdownType = clockShutdownType::RUN;
                m_rememberType      = msg.checkBox ()->isChecked ();
                break;
                }
            case QMessageBox::NoRole:       // Pause
                {
                m_clockShutdownType = clockShutdownType::PAUSE;
                m_rememberType      = msg.checkBox ()->isChecked ();
                break;
                }
            case QMessageBox::RejectRole:   // Cancel
                {
                allowClose = false;
                break;
                }
            default:
                {
                qWarning () << "Unexpected button result";
                break;
                }
            }
        }

    return allowClose;
    }


void MainWindow::saveFastClockSettings ()
    {
    QSettings   settings{ QSettings::UserScope };
    auto&       clock = control::FastClock::instance ();

    settings.setValue ("Clock/running", clock.isRunning ());
    settings.setValue ("Clock/ratio",   clock.getRatio ());
    settings.setValue ("Clock/time",    clock.qDateTime ());

    settings.setValue ("Clock/remember",m_rememberType);
    settings.setValue ("Clock/type",    static_cast<int> (m_clockShutdownType));
    settings.setValue ("Clock/shutdown",utils::time::fromTP (std::chrono::system_clock::now ()));
    }


void MainWindow::restoreFastClockSettings ()
    {
    QSettings   settings{ QSettings::UserScope };
    auto&       clock = control::FastClock::instance ();

    if (settings.contains ("Clock/time"))
        {
        clockShutdownType   type = static_cast<clockShutdownType> (settings.value ("Clock/type").toInt ());
        auto                time = utils::time::toTP (settings.value ("Clock/time").toDateTime ());

        clock.setRunning (settings.value ("Clock/running").toBool ());
        clock.setRatio (settings.value ("Clock/ratio").toUInt ());

        switch (type)
            {
            default:
            case clockShutdownType::PAUSE:
                {
                clock.setTime (time);
                break;
                }

            case clockShutdownType::RUN:
                {
                // Extrapolate the current time based on the ammount of time spent shutdown
                auto shutdownTime       = utils::time::toTP (
                                                settings.value (
                                                    "Clock/shutdown").toDateTime ());
                auto shutdownDuration   = std::chrono::system_clock::now () - shutdownTime;

                clock.setTime (time + shutdownDuration * clock.getRatio ());
                break;
                }
            }

        if ((m_rememberType = settings.value ("Clock/remember").toBool ()))
            {
            m_clockShutdownType = type;
            }
        }
    }


void MainWindow::commitDataRequest (QSessionManager& manager)
    {
    if (manager.allowsInteraction ())
        {
        if (handleClose ())
            {
            manager.release ();
            }
        else
            {
            manager.cancel ();
            }
        }
    }
}
