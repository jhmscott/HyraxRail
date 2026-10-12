/**
 * @file        ui/splashtest.hpp
 * @brief       Test suite for the splash screen
 * @author      Justin Scott
 * @date        2026-10-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <common.hpp>

#include <ui/splash.hpp>

#include <QtTest>

using namespace std::string_literals;


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the splash screen
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class SplashTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for controllerProgressTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void controllerProgressTest_data ()
        {
        QTest::addColumn<std::string> ("name");

        QTest::newRow ("basic")     << "Test Controller"s;
        QTest::newRow ("unicode")   << "测试控制器"s;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting a controller name
    ///
    /// @see    ui::SplashScreen::showControllerMessage()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void controllerProgressTest ()
        {
        QFETCH (std::string, name);

        ui::SplashScreen splash;

        splash.showControllerMessage (name);

        QCOMPARE (splash.message (), QString{ "Connecting to %1..." }.arg (name));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test showing the app starting message
    ///
    /// @see    ui::SplashScreen::showAppStartingMessage()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void appStartingTest ()
        {
        ui::SplashScreen splash;

        splash.showAppStartingMessage ();

        QCOMPARE (splash.message (), "Starting App...");
        }
    };

QTEST_MAIN (SplashTest)

#include "splashtest.moc"
