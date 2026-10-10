/**
 * @file        main.cpp
 * @brief       Hyrax rail main entry point
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <QtWidgets/QApplication>
#include <QFontDatabase>

#include <control/protocols/ecos.hpp>

#include <ui/lang.hpp>
#include <ui/mainwindow.hpp>
#include <ui/splash.hpp>

#include <utils/os.hpp>

#include <res/version.h>

#include <QDebug>
#include <QTranslator>
#include <QImageReader>


///////////////////////////////////////////////////////////////////////////////
/// Main application entry point
///
/// @param[in]  argc            Argument count
/// @param[in]  argv            Array of argument values (size is argc)
///
/// @return     Process exit code
///
///////////////////////////////////////////////////////////////////////////////
int main (int argc, char *argv[])
    {
    utils::os::setThreadName ("Main Thread");

    QApplication app(argc, argv);

    ui::lang::Translator::instance ().setLocale ();

    QApplication::setApplicationName    (PRODUCT_NAME_SHORT);
    QApplication::setOrganizationName   (COMPANY_NAME);
    QApplication::setOrganizationDomain (COMPANY_DOMAIN);
    QApplication::setApplicationVersion (VERSION_STRING);

#ifdef Q_OS_MACOS
    QApplication::setWindowIcon         (QIcon{ ":/icons/app/conductor-hyrax.icns" });
#else
    QApplication::setWindowIcon         (QIcon{ ":/icons/app/conductor-hyrax.ico" });
#endif

#ifdef Q_OS_ANDROID
    QFont defaultFont = app.font ();

    defaultFont.setPointSize (16);
    app.setFont (defaultFont);
#endif
    ui::SplashScreen  splash;

    splash.show ();

    QFontDatabase::addApplicationFont (":/fonts/CascadiaMono.ttf");

    int rc;

    {
    auto controllers = control::loadControllersFromUserSettings (
        [&] (const std::string& controller)
        {
        splash.showControllerMessage (controller);
        });

    splash.showAppStartingMessage ();

    ui::MainWindow window{ *controllers };

    window.show ();
    splash.finish (&window);

    rc = app.exec ();
    }

    return rc;
    }
