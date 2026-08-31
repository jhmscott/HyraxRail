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
#include <QtEndian>
#include <QtTest>

/// Set of 4 octets in an IPv4 address
struct ipQuad
    {
    uint8_t addr[4];    ///< Address octets
    };

using namespace utils::device;

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

        QCOMPARE (getDeviceType (tcp), TYPE_TCP);


        QUdpSocket udp;

        QCOMPARE (getDeviceType (udp), TYPE_UDP);


        QSerialPort com;

        QCOMPARE (getDeviceType (com), TYPE_SERIAL);

        QFile file;

        QCOMPARE (getDeviceType (file), TYPE_UNSUPPORTED);


        QProcess process;

        QCOMPARE (getDeviceType (process), TYPE_UNSUPPORTED);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case data setup for DeviceTest::hostInfoIPv4Test()
    ///
    /// @see    DeviceTest::hostInfoIPv4Test()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void hostInfoIPv4Test_data ()
        {
        QTest::addColumn<ipQuad> ("addr");

        QTest::newRow ("10.0.0.0/8")        << ipQuad{ 10, 1, 2, 4 };
        QTest::newRow ("172.16.0.0/12")     << ipQuad{ 172, 18, 253, 252 };
        QTest::newRow ("192.168.0.0/16")    << ipQuad{ 192, 168, 212, 123 };
        QTest::newRow ("Public Address")    << ipQuad{ 142, 250, 73, 99 };
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test basic parsing of an IPv4 string
    ///
    /// @see    utils::device::HostInfo::fromString()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void hostInfoIPv4Test ()
        {
        QFETCH (ipQuad, addr);

        QCOMPARE (HostInfo::fromString (QString{ "%1.%2.%3.%4" }.arg ((int) addr.addr[0])
                                                                .arg ((int) addr.addr[1])
                                                                .arg ((int) addr.addr[2])
                                                                .arg ((int) addr.addr[3]),
                                        HostInfo::type::IP).toAddress (),
                  QHostAddress{ qToBigEndian (*reinterpret_cast<quint32*> (&addr)) });
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test parsing of a hostname to IP address
    ///
    /// @see    utils::device::HostInfo::fromString()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void hostInfoHostnameTest ()
        {
        QCOMPARE (HostInfo::fromString ("localhost",
                                        HostInfo::type::HOSTNAME).toAddress (),
                  QHostAddress{ "127.0.0.1" });
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the isDeviceSupported() function
    ///
    /// @see    utils::device::isDeviceSupported()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void deviceMaskTest ()
        {
        for (uint64_t ii = 0; ii < (1 << NUM_TYPES); ++ii)
            {
            for (int jj = 0; jj < NUM_TYPES; ++jj)
                {
                QCOMPARE (isDeviceSupported (static_cast<type> (jj),
                                             mask{ ii }),
                          0 != (ii & (1LLU << jj)));
                }

            // This should be false for all masks
            QVERIFY (not isDeviceSupported (TYPE_UNSUPPORTED,
                                            mask{ ii }));
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test case data setup for DeviceTest::ioDeviceMaskTest()
    ///
    /// @see    DeviceTest::ioDeviceMaskTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void ioDeviceMaskTest_data ()
        {
        QTest::addColumn<QIODevice*> ("device");
        QTest::addColumn<type> ("devType");

        QTest::newRow ("UDP") << (QIODevice*) new QUdpSocket{ this }  << TYPE_UDP;
        QTest::newRow ("TCP") << (QIODevice*) new QTcpSocket{ this }  << TYPE_TCP;
        QTest::newRow ("COM") << (QIODevice*) new QSerialPort{ this } << TYPE_SERIAL;
        QTest::newRow ("File")<< (QIODevice*) new QFile{ this }       << TYPE_UNSUPPORTED;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test the isDeviceSupported() function with a QIODevice
    ///
    /// @see    utils::device::isDeviceSupported()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void ioDeviceMaskTest ()
        {
        QFETCH (QIODevice*, device);
        QFETCH (type,       devType);

        for (uint64_t ii = 0; ii < (1 << NUM_TYPES); ++ii)
            {
            mask myMask{ ii };

            QCOMPARE (isDeviceSupported (*device, myMask),
                      TYPE_UNSUPPORTED != devType && myMask[devType]);
            }
        }
    };


QTEST_GUILESS_MAIN (DeviceTest);

#include "devicetest.moc"
