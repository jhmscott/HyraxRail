/**
 * @file        utils/ostest.hpp
 * @brief       Test suite for the OS functions
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/ext/fakeit.hpp>

#include <utils/os.hpp>

#include <QtTest>

using namespace utils::os;

static const char* const THREAD_NAME = "Test thread";


///////////////////////////////////////////////////////////////////////////////
/// Fake message only window class, needed to provide a concrete instance to call
/// a constructor for
///
///////////////////////////////////////////////////////////////////////////////
class FakeMsgOnlyWindow : public win32::MessageOnlyWindow
    {
private:
    virtual LRESULT message (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
        return 0;
        }
    };

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the OS utility library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class OsTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting the current thread name
    ///
    /// @see    utils::os::setThreadName()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setCurrentThreadNameTest ()
        {
        std::thread th{
            [] () -> void
            {
            setThreadName (THREAD_NAME);

            QCOMPARE (getThreadName (), THREAD_NAME);
            } };

        th.join ();
        }

    // This isn't possible on Mac
    // Shakes fist at apple
#if !defined (Q_OS_MACOS) || defined (DOXYGEN)
    ///////////////////////////////////////////////////////////////////////////////
    /// Test setting a std::thread name
    ///
    /// @see    utils::os::setThreadName()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setStdThreadNameTest ()
        {
        std::mutex mtx;

        // Hold this lock, to prevent the QCOMPARE until we have set the thread name
        std::unique_lock lk{ mtx };

        std::thread th{
            [&mtx] () -> void
            {
            std::lock_guard lk{ mtx };

            QCOMPARE (getThreadName (), THREAD_NAME);
            } };

        setThreadName (THREAD_NAME, th);
        QCOMPARE (getThreadName (th), THREAD_NAME);

        lk.unlock ();

        th.join ();
        }
#endif // Q_OS_MACOS


#if defined (Q_OS_WIN) || defined (DOXYGEN)
    ///////////////////////////////////////////////////////////////////////////////
    /// Tests the message only window class can receive messages via SendMessage()
    ///
    /// @see    utils::os::win32::MessageOnlyWindow
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void messageOnlyWindowTest ()
        {
        using MockedWindow = fakeit::Mock<win32::MessageOnlyWindow>;

        FakeMsgOnlyWindow   fake;
        MockedWindow        windowMock{ fake };

        fakeit::When (Method (windowMock, message)).Return (TRUE);

        HWND hwnd = windowMock.get ().getHandle ();

        SendMessage (hwnd,
                     WM_ACTIVATE,
                     0,
                     0);

        fakeit::Verify (Method (windowMock, message).Using (hwnd, WM_ACTIVATE, 0, 0));
        }
#endif // Q_OS_WIN

    };

QTEST_GUILESS_MAIN (OsTest)

#include "ostest.moc"
