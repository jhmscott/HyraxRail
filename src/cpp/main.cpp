/**
 * @file        main.cpp
 * @brief       Hyrax rail main entry point
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <QtWidgets/QApplication>

#include <control/protocols/ecos.hpp>

#include <ui/mainwindow.hpp>

#include <utils/os.hpp>

#include <res/version.h>

#include <QDebug>
#include <QLibraryInfo>
#include <QTranslator>

///////////////////////////////////////////////////////////////////////////////
/// Main application entry point
///
/// @param[in]  argc            Argument count
/// @param[in]  argv            Array of argument values (size is argc)
///
/// @return     Process exit code
///
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
    {
    utils::os::setThreadName ("Main Thread");

    QApplication    app(argc, argv);
    QTranslator     sysTranslator;
    QTranslator     appTranslator;
    QLocale         locale              = QLocale::system ();
    QString         translationLibray   = QLibraryInfo::path (QLibraryInfo::TranslationsPath);

    QLocale::setDefault (locale);

    // don't install translators if we're already in English
    if (QLocale::English != locale.language ())
        {
        if (sysTranslator.load (locale,
                                "qt",
                                "_",
                                translationLibray))
            {
            if (app.installTranslator (&sysTranslator))
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

        if (appTranslator.load (locale,
                                "HyraxRail",
                                "_",
                                ":/translations"))
            {
            if (app.installTranslator (&appTranslator))
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

    QApplication::setApplicationName    (PRODUCT_NAME_SHORT);
    QApplication::setOrganizationName   (COMPANY_NAME);
    QApplication::setOrganizationDomain (COMPANY_DOMAIN);
    QApplication::setApplicationVersion (VERSION_STRING);
    QApplication::setWindowIcon         (QIcon{ ":/icons/app/conductor-hyrax.ico" });

    int rc;

    {
    ui::MainWindow window;

    window.show();

    rc = app.exec ();
    }

    return rc;
    }
