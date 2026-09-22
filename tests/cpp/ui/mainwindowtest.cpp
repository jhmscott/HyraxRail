/**
 * @file        ui/mainwindowtest.hpp
 * @brief       Test suite for the main application window
 * @author      Justin Scott
 * @date        2026-09-21
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <testutils/lang.hpp>

#include <ui/mainwindow.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the main application window
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class MainWindowTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for setLocalTitleBarTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocaleTitleBarTest_data ()
        {
        QTest::addColumn<QLocale> ("locale");
        QTest::addColumn<QString> ("title");

        for (const auto& [locale, title, language] : testutils::SUPPORTED_LOCALES)
            {
            QTest::newRow (language) << locale << title;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test that setting the app locale updates the titlebar dynamically
    ///
    /// @see        ui::MainWindow::setTitle()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocaleTitleBarTest ()
        {
        QFETCH (QLocale, locale);
        QFETCH (QString, title);

        ui::MainWindow window;

        testutils::AppLocaleGuard grd{ locale };

        QApplication::processEvents ();

        QCOMPARE (window.windowTitle (), title);
        }
    };

QTEST_MAIN (MainWindowTest)

#include "mainwindowtest.moc"
