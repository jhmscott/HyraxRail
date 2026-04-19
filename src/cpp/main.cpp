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

    QApplication app(argc, argv);

    QApplication::setApplicationName        (PRODUCT_NAME_SHORT);
    QApplication::setApplicationDisplayName (PRODUCT_NAME_FULL);
    QApplication::setOrganizationName       (COMPANY_NAME);
    QApplication::setOrganizationDomain     (COMPANY_DOMAIN);
    QApplication::setApplicationVersion     (VERSION_STRING);
    QApplication::setWindowIcon             (QIcon{ ":/icons/app/conductor-hyrax.ico" });

    int rc;

    {
    ui::MainWindow window;

    window.show();

    rc = app.exec ();
    }

    return rc;
    }
