/**
 * @file        utils/pptest.hpp
 * @brief       Test suite for the pre-processor utility library
 * @author      Justin Scott
 * @date        2026-09-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <testutils/ext/fakeit.hpp>

#include <utils/qobj.hpp>

#include <QtTest>

using namespace utils::qobj;

///////////////////////////////////////////////////////////////////////////////
/// Base class to allow mocking
///
///////////////////////////////////////////////////////////////////////////////
class FakeSignalClientBase
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Virtual slot implementation to allow mocking
    ///
    /// @param[in]  name        New object name
    ///
    /// @remarks    Fakeit has issues with pass by reference. For now, making this
    ///             pass by value. But maybe look into this and possibly open a PR
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void slotImpl (QString name) {}
    };

///////////////////////////////////////////////////////////////////////////////
/// Fake signal client to test member function slots
///
/// @see    QObjTest::signalClientMemberFunctionTest()
///
///////////////////////////////////////////////////////////////////////////////
class FakeSignalClient : public FakeSignalClientBase, public SignalClient
    {
public:
    QObject sender; ///< Object to send the signal

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor, connects the object name changed signal to the "slot" member
    /// function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    FakeSignalClient () { connect (&sender, &QObject::objectNameChanged, &FakeSignalClient::slot); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Slot to receive the object name signal
    ///
    /// @param[in]  name        New object name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void slot (const QString& name) { slotImpl (name); }

    };

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
        SignalGuard grd{ obj, block };

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

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the RaiiConnection class and it's ability to disconnect a connection
    ///
    /// @see    utils::qobj::RaiiConnection
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void raiiConnectionTest ()
        {
        bool called = false;

        auto slot = [&] () { called = true; };

        QObject underTest;

        {
        RaiiConnection connection = QObject::connect (&underTest,
                                                      &QObject::objectNameChanged,
                                                      slot);

        underTest.setObjectName ("Rename 1");

        // We are still in the connections scope, so the slot should be called
        QVERIFY (called);
        }

        called = false;

        underTest.setObjectName ("Rename 2");

        // We're not in the scope anymore, the slot shouldn't have been called
        QVERIFY (not called);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the SignalClient with a member function slot
    ///
    /// @see    utils::qobj::SignalClient
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void signalClientMemberFunctionTest ()
        {
        static const QString TEST_NAME = "Test Object Name";

        FakeSignalClient                    fake;
        fakeit::Mock<FakeSignalClientBase>  mock{ fake };

        fakeit::When (Method (mock, slotImpl)).AlwaysReturn ();

        fake.sender.setObjectName (TEST_NAME);

        fakeit::Verify (Method (mock, slotImpl).Using (TEST_NAME));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the SignalClient with a non-member function slot
    ///
    /// @see    utils::qobj::SignalClient
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void signalClientNonMemberFunctionTest ()
        {
        static const QString TEST_NAME = "Test Object Name";

        QString actualName;
        auto    slot = [&] (const QString& name) { actualName = name; };

        QObject sender;

        {
        SignalClient client;

        client.connect (&sender, &QObject::objectNameChanged, slot);

        sender.setObjectName (TEST_NAME);

        // We are in the client's scope, so this should set the name
        QCOMPARE (actualName, TEST_NAME);
        }

        actualName = "";

        sender.setObjectName (TEST_NAME);

        // Client has been destructed, this shouldn't do anything
        QCOMPARE (actualName, "");
        }
    };

QTEST_GUILESS_MAIN (QObjTest)

#include "qobjtest.moc"
