
/**
 * @file        mainwindow.hpp
 * @brief       Hyrax rail main application window
 * @author      Justin Scott
 * @date        2026-01-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QtWidgets/QMainWindow>

namespace ui
{
class MainWindow : public QMainWindow
    {
    Q_OBJECT

public:
    MainWindow (QWidget *parent = nullptr);
    ~MainWindow();

    };

}
