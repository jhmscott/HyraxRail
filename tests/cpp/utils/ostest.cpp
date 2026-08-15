/**
 * @file        utils/ostest.hpp
 * @brief       Test suite for the OS functions
 * @author      Justin Scott
 * @date        2026-08-14
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/os.hpp>

#include <QtTest>

using namespace utils::os;

static const char* const THREAD_NAME = "Test thread";

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
#ifndef Q_OS_MACOS
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

    };

QTEST_GUILESS_MAIN (OsTest)

#include "ostest.moc"
