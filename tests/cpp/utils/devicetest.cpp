/**
 * @file        utils/devicetest.hpp
 * @brief       Test suite for the device utilities
 * @author      Justin Scott
 * @date        2026-08-09
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/device.hpp>

#include <QFile>
#include <QtTest>

///////////////////////////////////////////////////////////////////////////////
/// Test suite for the device utility library
///
/// @ingroup    UNIT_TEST
///
///////////////////////////////////////////////////////////////////////////////
class DeviceTest : public QObject
    {
    Q_OBJECT
private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the getDeviceType() function
    ///
    /// @see    utils::device::getDeviceType()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void testDeviceTypes ()
        {
        QTcpSocket tcp;

        QCOMPARE (utils::device::getDeviceType (tcp),
                  utils::device::TYPE_TCP);


        QUdpSocket udp;

        QCOMPARE (utils::device::getDeviceType (udp),
                  utils::device::TYPE_UDP);


        QSerialPort com;

        QCOMPARE (utils::device::getDeviceType (com),
                  utils::device::TYPE_SERIAL);

        QFile file;

        QCOMPARE (utils::device::getDeviceType (file),
                  utils::device::TYPE_UNSUPPORTED);


        QProcess process;

        QCOMPARE (utils::device::getDeviceType (process),
                  utils::device::TYPE_UNSUPPORTED);
        }
    };


QTEST_GUILESS_MAIN (DeviceTest);

#include "devicetest.moc"
