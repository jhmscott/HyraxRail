/**
 * @file        mainwindow.cpp
 * @brief       Hyrax rail main application window
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/mainwidget.hpp>
#include <ui/mainwindow.hpp>

#include <QApplication>
#include <QTabWidget>

#include <res/version.h>

#include <utils/os.hpp>

namespace ui
{
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    {
    static const QString appName = tr ("Hyrax Rail");

    QApplication::setApplicationDisplayName (appName);

    setCentralWidget (new ui::MainWidget{ this });
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    setWindowTitle (appName);
    }

MainWindow::~MainWindow()
    {}

}
