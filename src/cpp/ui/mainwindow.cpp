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

#ifdef Q_OS_ANDROID
#include <jni.h>
#include <QJniObject>
#endif // Q_OS_ANDROID


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

#ifdef Q_OS_ANDROID
    // Qt doesn't seem to set a palette for dark mode, keeping it the same as light mode
    // Below matches the default dark pallete on other platforms
    //
    // see:         https://forum.qt.io/topic/164433/qt-6.10.2-android-qt-quick-apps-ignore-dark-mode
    // Based on :   https://medo64.com/posts/dark-mode-for-qt-application
    m_lightPalette = qApp->palette ();

    m_darkPalette.setColor (QPalette::Window,           QColor( 37,  37,  37));
    m_darkPalette.setColor (QPalette::WindowText,       QColor(212, 212, 212));
    m_darkPalette.setColor (QPalette::Base,             QColor( 60,  60,  60));
    m_darkPalette.setColor (QPalette::AlternateBase,    QColor( 45,  45,  45));
    m_darkPalette.setColor (QPalette::PlaceholderText,  QColor(127, 127, 127));
    m_darkPalette.setColor (QPalette::Text,             QColor(212, 212, 212));
    m_darkPalette.setColor (QPalette::Button,           QColor( 45,  45,  45));
    m_darkPalette.setColor (QPalette::ButtonText,       QColor(212, 212, 212));
    m_darkPalette.setColor (QPalette::BrightText,       QColor(240, 240, 240));
    m_darkPalette.setColor (QPalette::Highlight,        QColor( 38,  79, 120));
    m_darkPalette.setColor (QPalette::HighlightedText,  QColor(240, 240, 240));

    m_darkPalette.setColor (QPalette::Light,            QColor( 60,  60,  60));
    m_darkPalette.setColor (QPalette::Midlight,         QColor( 52,  52,  52));
    m_darkPalette.setColor (QPalette::Dark,             QColor( 30,  30,  30) );
    m_darkPalette.setColor (QPalette::Mid,              QColor( 37,  37,  37));
    m_darkPalette.setColor (QPalette::Shadow,           QColor( 0,    0,   0));

    setPalette (qApp->styleHints ()->colorScheme ());

    connect (qApp->styleHints (),
            &QStyleHints::colorSchemeChanged,
             this,
            &MainWindow::setPalette);

    QNativeInterface::QAndroidApplication::runOnAndroidMainThread (
        [this] () -> void
        {
        QNativeInterface::
            QAndroidApplication::
                context ().callMethod<void> ("registerMainWindow",
                                             reinterpret_cast<jlong> (this));
        });
#endif // Q_OS_ANDROID

    connect (qApp,
            &QGuiApplication::commitDataRequest,
             this,
            &MainWindow::commitDataRequest,
             Qt::DirectConnection);

    connect (qApp,
            &QGuiApplication::applicationStateChanged,
             this,
            &MainWindow::applicationStateChanged,
             Qt::DirectConnection);
    }


MainWindow::~MainWindow ()
    {
    saveFastClockSettings ();

#ifdef Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread (
        [this] () -> void
        {
        QNativeInterface::
            QAndroidApplication::
                context ().callMethod<void> ("unregisterMainWindow");
        }).waitForFinished ();
#endif // Q_OS_ANDROID

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

QDialog* MainWindow::getModal ()
    {
    // Currently all dialog we use are modal
    // In the future, this may change and we should update this logic
    //
    return findChild<QDialog*> ();
    }


void MainWindow::applicationStateChanged (Qt::ApplicationState state)
    {
    switch (state)
        {
        case Qt::ApplicationSuspended:
            {
            qDebug () << "App suspended";
            break;
            }
        case Qt::ApplicationHidden:
            {
            qDebug () << "App suspended";
            break;
            }
        case Qt::ApplicationInactive:
            {
            qDebug () << "App inactive";
            break;
            }
        case Qt::ApplicationActive:
            {
            qDebug () << "App active";
            break;
            }
        }
    }

void MainWindow::dismissModalAnimation (double progress)
    {
    if (NULL != m_dismiss)
        {
        m_dismiss->setProgress (progress);
        }
    }

void MainWindow::dismissModalStart ()
    {
    QDialog* modal = getModal ();

    if (NULL != modal)
        {
        m_dismiss.reset (new DismissAnimation{ *modal });
        }
    }

void MainWindow::dismissModalCancel ()
    {
    m_dismiss.reset ();
    }

bool MainWindow::backPress ()
    {
    bool        allowBack;
    QDialog*    dialog = getModal ();

    if (NULL == dialog)
        {
        allowBack = true;
        }
    else
        {
        m_dismiss.reset ();
        dialog->reject ();

        allowBack = false;
        }

    return allowBack;
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


#if defined (Q_OS_ANDROID) || defined (DOXYGEN)
extern "C"
{

///////////////////////////////////////////////////////////////////////////////
/// Callled when the back animation progresses
///
/// @param[in]  env             (unused) JNI environment
/// @param[in]  thisObj         (unused) Back animation java object
/// @param[in]  mainWindowPtr   Pointer to the MainWindow object
/// @param[in]  progress        Animation progress
///
/// @see        ca.justinlab.hyraxrail.BackAnimation.backProgressed()
///
/// @ingroup    JNI_FUNC
///
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT
void JNICALL Java_ca_justinlab_hyraxrail_BackAnimation_backProgressed (JNIEnv*  env,
                                                                       jobject  thisObj,
                                                                       jlong    mainWindowPtr,
                                                                       jfloat   progress)
    {
    QMetaObject::invokeMethod (reinterpret_cast<ui::MainWindow*> (mainWindowPtr),
                              &ui::MainWindow::dismissModalAnimation,
                               progress);
    }

///////////////////////////////////////////////////////////////////////////////
/// Callled when the back animation starts
///
/// @param[in]  env             (unused) JNI environment
/// @param[in]  thisObj         (unused) Back animation java object
/// @param[in]  mainWindowPtr   Pointer to the MainWindow object
///
/// @see        ca.justinlab.hyraxrail.BackAnimation.backStarted()
///
/// @ingroup    JNI_FUNC
///
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT
void JNICALL Java_ca_justinlab_hyraxrail_BackAnimation_backStarted (JNIEnv*     env,
                                                                    jobject     thisObj,
                                                                    jlong       mainWindowPtr)
    {
    QMetaObject::invokeMethod (reinterpret_cast<ui::MainWindow*> (mainWindowPtr),
                              &ui::MainWindow::dismissModalStart);
    }


///////////////////////////////////////////////////////////////////////////////
/// Callled when the user cancels the cback operation
///
/// @param[in]  env             (unused) JNI environment
/// @param[in]  thisObj         (unused) Back animation java object
/// @param[in]  mainWindowPtr   Pointer to the MainWindow object
///
/// @see        ca.justinlab.hyraxrail.BackAnimation.backCancelled()
///
/// @ingroup    JNI_FUNC
///
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT
void JNICALL Java_ca_justinlab_hyraxrail_BackAnimation_backCancelled (JNIEnv*   env,
                                                                      jobject   thisObj,
                                                                      jlong     mainWindowPtr)
    {
    QMetaObject::invokeMethod (reinterpret_cast<ui::MainWindow*> (mainWindowPtr),
                              &ui::MainWindow::dismissModalCancel);
    }



///////////////////////////////////////////////////////////////////////////////
/// Callled when the back is completed
///
/// @param[in]  env             (unused) JNI environment
/// @param[in]  thisObj         (unused) Back animation java object
/// @param[in]  mainWindowPtr   Pointer to the MainWindow object
///
/// @see        ca.justinlab.hyraxrail.BackAnimation.backPressed()
///
/// @ingroup    JNI_FUNC
///
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT
void JNICALL Java_ca_justinlab_hyraxrail_BackAnimation_backPressed (JNIEnv*     env,
                                                                    jobject     thisObj,
                                                                    jlong       mainWindowPtr)
    {
    ui::MainWindow*     mainWindow = reinterpret_cast<ui::MainWindow*> (mainWindowPtr);
    bool                allowClose = false;

    QMetaObject::invokeMethod (mainWindow,
                              &ui::MainWindow::backPress,
                               Qt::BlockingQueuedConnection,
                              &allowClose);
    if (allowClose)
        {
        QNativeInterface::
            QAndroidApplication::
                context ().callMethod<jboolean> ("moveTaskToBack",
                                                 static_cast<jboolean> (JNI_TRUE));
        }
    }
}

#endif // defined (Q_OS_ANDROID) || defined (DOXYGEN)
