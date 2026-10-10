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
#include <QThread>

namespace ui
{

static const int       SPLASH_MSG_ALIGN = Qt::AlignRight | Qt::AlignBottom;
static const QColor    SPLASH_MSG_COLOR = Qt::yellow;

SplashScreen::SplashScreen ():
    QSplashScreen (QPixmap{ ":/images/splash.png" },
                   Qt::WindowStaysOnTopHint)
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

    layout->addWidget (title,       0, Qt::AlignRight | Qt::AlignTop);
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