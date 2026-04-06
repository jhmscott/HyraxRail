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

#include <QTabWidget>

#include <res/version.h>

#include <utils/os.hpp>

namespace ui
{
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    {
    setCentralWidget (new ui::MainWidget{ this });
    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
    setWindowTitle (PRODUCT_NAME_FULL);

    QMetaObject::connectSlotsByName (this);

    // utils::os::notify (winId (), "Window Created", "Finished Creating the main window");

    }

MainWindow::~MainWindow()
{}

}
