/**
 * @file        testutils_test/logspytest.hpp
 * @brief       Test suite for the LogSpy class test utility
 * @author      Justin Scott
 * @date        2026-08-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <testutils/logspy.hpp>

#include <QtTest>

#define LOGSPYTEST_LINE_START  271828


///////////////////////////////////////////////////////////////////////////////
/// Assert the fields of a log record are what we expect
///
/// @param[in]  record      Record to check
/// @param[in]  message     Expected error message
/// @param[in]  type        Log level
/// @param[in]  lineOffset  Number of lines sine #line LOGSPYTEST_LINE_START
///
///////////////////////////////////////////////////////////////////////////////
static void assertLogRecord (const testutils::logRecord&    record,
                             const QString&                 message,
                             QtMsgType                      type,
                             int                            lineOffset)
    {
    QCOMPARE (record.type, type);
    QCOMPARE (record.file, __FILE__);
    QCOMPARE (record.line, LOGSPYTEST_LINE_START + lineOffset);
    QCOMPARE (record.msg, message);
    }

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the LogSpy class test utility
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class LogSpyTest : public QObject
    {
    Q_OBJECT
private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Test of the LogSpy class
    ///
    /// @see    testutils::LogSpy
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void logSpyTest ()
        {
        testutils::LogSpy spy;

#line LOGSPYTEST_LINE_START
        qDebug ()   << "Debug message";
        qInfo ()    << "Info message";
        qWarning () << "Warning message";
        qCritical ()<< "Critical message";

        std::lock_guard lk{ spy };

        QCOMPARE (spy.size (), 4);

        assertLogRecord (spy[0], "Debug message",   QtDebugMsg,     0);
        assertLogRecord (spy[1], "Info message",    QtInfoMsg,      1);
        assertLogRecord (spy[2], "Warning message", QtWarningMsg,   2);
        assertLogRecord (spy[3], "Critical message",QtCriticalMsg,  3);
        }
    };

QTEST_GUILESS_MAIN (LogSpyTest)

#include "logspytest.moc"
