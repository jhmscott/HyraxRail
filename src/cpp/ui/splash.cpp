/**
 * @file        splash.cpp
 * @brief       Hyrax rail splash screen
 * @author      Justin Scott
 * @date        2026-10-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <res/version.h>

#include <ui/splash.hpp>

#include <utils/resources.hpp>

#include <QBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QThread>

namespace ui
{

static const int       SPLASH_MSG_ALIGN = Qt::AlignRight | Qt::AlignBottom;
static const QColor    SPLASH_MSG_COLOR = Qt::yellow;


///////////////////////////////////////////////////////////////////////////////
/// Create the background bitmap pixmap for the splash screen
///
/// @return     Splash screen background
///
///////////////////////////////////////////////////////////////////////////////
static QPixmap makeSplashBitmap ()
    {
#ifdef PLATFORM_MOBILE
    QPixmap     pixmap{ ":/images/splash-portrait.png" };
    QScreen*    screen  = QGuiApplication::primaryScreen ();
    QSize       size    = screen->size ();

    // Scale to the screen size, leaving extra space
    auto scaled = pixmap.scaled (size.width (),
                                 size.height (),
                                 Qt::KeepAspectRatioByExpanding,
                                 Qt::SmoothTransformation);

    // Crop off extra space
    return scaled.copy ((scaled.width ()  - size.width ())  / 2,
                        (scaled.height () - size.height ()) / 2,
                        size.width (),
                        size.height ());
#else
    QPixmap pixmap{ ":/images/splash.png" };

    return pixmap.scaled (512,
                          256,
                          Qt::KeepAspectRatio,
                          Qt::SmoothTransformation);
#endif // PLATFORM_MOBILE
    }


SplashScreen::SplashScreen ():
    QSplashScreen (makeSplashBitmap (), Qt::WindowStaysOnTopHint)
    {
    QVBoxLayout*    layout  = new QVBoxLayout{ this };
    QFont           font    = QApplication::font ();

    QLabel* title       = new QLabel{ QApplication::applicationDisplayName (), this };
    QLabel* copyright   = new QLabel{ tr ("Copyright © %1").arg (COPYRIGHT_YEAR), this };

    font.setPixelSize (45);
    font.setBold (true);

    title->setFont (font);
    title->setStyleSheet ("color: white;");

    font.setPixelSize(utils::resources::FONT_SIZE_LARGE);
    font.setBold (false);

    copyright->setFont (font);
    copyright->setStyleSheet ("color: white;");

    layout->addWidget (title,       0, Qt::AlignLeft  | Qt::AlignTop);
    layout->addWidget (copyright,   0, Qt::AlignLeft  | Qt::AlignBottom);

    setLayout (layout);
    }

void SplashScreen::showControllerMessage (const std::string& controller)
    {
    showMessage (tr ("Connecting to %1...").arg (controller),
                 SPLASH_MSG_ALIGN,
                 SPLASH_MSG_COLOR);
    }

void SplashScreen::showAppStartingMessage ()
    {
    showMessage (tr ("Starting App..."),
                 SPLASH_MSG_ALIGN,
                 SPLASH_MSG_COLOR);
    }

}
