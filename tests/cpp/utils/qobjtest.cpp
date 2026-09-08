/**
 * @file        utils/pptest.hpp
 * @brief       Test suite for the pre-processor utility library
 * @author      Justin Scott
 * @date        2026-09-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/qobj.hpp>

#include <QtTest>


///////////////////////////////////////////////////////////////////////////////
/// Test suite for the pre-processor utility library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class QObjTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for signalGuardTest
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void signalGuardTest_data ()
        {
        QTest::addColumn<bool> ("initial");
        QTest::addColumn<bool> ("block");

        QTest::newRow ("Always Unblocked")  << false << false;
        QTest::newRow ("Always Blocked")    << true  << true;
        QTest::newRow ("Unblocked")         << true  << false;
        QTest::newRow ("Blocked")           << false << true;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the signal guard class
    ///
    /// @see    utils::qobj::SignalGuard
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void signalGuardTest ()
        {
        QFETCH (bool, initial);
        QFETCH (bool, block);

        QObject obj;
        QSignalSpy spy{ &obj, &QObject::objectNameChanged };

        obj.blockSignals (initial);

        {
        utils::qobj::SignalGuard grd{ obj, block };

        QCOMPARE (obj.signalsBlocked (), block);

        obj.setObjectName ("Test name");
        }

        // Test signal state restored
        QCOMPARE (obj.signalsBlocked (), initial);

        if (block)
            {
            QCOMPARE (spy.count (), 0);
            }
        else
            {
            QCOMPARE (spy.count (), 1);
            }
        }
    };

QTEST_GUILESS_MAIN (QObjTest)

#include "qobjtest.moc"
