/**
 * @file        utils/devicetest.hpp
 * @brief       Test suite for the device utilities
 * @author      Justin Scott
 * @date        2026-08-09
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <testutils/assert.hpp>

#include <utils/device.hpp>

#include <QFile>
#include <QtEndian>
#include <QtTest>

Q_DECLARE_METATYPE (utils::device::HostInfo);
Q_DECLARE_METATYPE (utils::device::deviceInfo);

/// Set of 4 octets in an IPv4 address
struct ipQuad
    {
    uint8_t addr[4];    ///< Address octets
    };

using namespace utils::device;


///////////////////////////////////////////////////////////////////////////////
/// Test data generator for all standard baud rates
///
///////////////////////////////////////////////////////////////////////////////
static void generateBaudRates ()
    {
    QTest::addColumn<QString> ("name");
    QTest::addColumn<int> ("baud");

    for(int baud : QSerialPortInfo::standardBaudRates ())
        {
#ifdef Q_OS_WIN
        QTest::addRow ("Windows port baud %d", baud) << "COM1" << baud;
#else
        QTest::addRow ("Unix port baud %d", baud) << "/dev/tty0" << baud;
#endif
        }
    }

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

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareHostInfoIpTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareHostInfoIpTest_data ()
        {
        QTest::addColumn<QHostAddress> ("ip");

        QTest::newRow ("10.0.0.0/8")        << QHostAddress{ "10.0.0.0" };
        QTest::newRow ("172.16.0.0/12")     << QHostAddress{ "172.16.0.0" };
        QTest::newRow ("192.168.0.0/16")    << QHostAddress{ "192.168.0.0" };
        QTest::newRow ("Public Address")    << QHostAddress{ "142.250.73.99" };
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test comparing two host infos holding IPs
    ///
    /// @see    utils::device::HostInfo::operator==()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareHostInfoIpTest ()
        {
        QFETCH (QHostAddress, ip);

        HostInfo info1{ ip };
        HostInfo info2{ ip };

        COMPARE_OP_EQ (info1, info2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareHostInfoHostname()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareHostInfoHostnameTest_data ()
        {
        QTest::addColumn<QString> ("host");

        QTest::newRow ("localhost") << "localhost";
        QTest::newRow ("google")    << "google.ca";
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test comparing two host infos holding Hostnames
    ///
    /// @see    utils::device::HostInfo::operator==()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareHostInfoHostnameTest ()
        {
        QFETCH (QString, host);

        auto info1 = HostInfo::fromString (host, HostInfo::type::HOSTNAME);
        auto info2 = HostInfo::fromString (host, HostInfo::type::HOSTNAME);

        COMPARE_OP_EQ (info1, info2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareHostInfoNotEqual()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareHostInfoNotEqualTest_data ()
        {
        QTest::addColumn<HostInfo> ("host1");
        QTest::addColumn<HostInfo> ("host2");

        QTest::newRow ("Different IP")
            << HostInfo::fromString ("1.1.1.1", HostInfo::type::IP)
            << HostInfo::fromString ("1.1.1.12", HostInfo::type::IP);

        QTest::newRow ("Different Host")
            << HostInfo::fromString ("host1", HostInfo::type::HOSTNAME)
            << HostInfo::fromString ("host2", HostInfo::type::HOSTNAME);

        QTest::newRow ("Host and IP")
            << HostInfo::fromString ("localhost", HostInfo::type::HOSTNAME)
            << HostInfo::fromString ("127.0.0.1", HostInfo::type::IP);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test comparing two host infos holding IPs
    ///
    /// @see    utils::device::HostInfo::operator!=()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareHostInfoNotEqualTest ()
        {
        QFETCH (HostInfo, host1);
        QFETCH (HostInfo, host2);

        COMPARE_OP_NE (host1, host2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareSerialInfoTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSerialInfoTest_data ()
        {
        generateBaudRates ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test comparing two serial infos
    ///
    /// @see    utils::device::serialInfo::operator==()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSerialInfoTest ()
        {
        QFETCH (QString, name);
        QFETCH (int, baud);

        serialInfo info1{ QSerialPortInfo{ name }, baud };
        serialInfo info2{ QSerialPortInfo{ name }, baud };

        COMPARE_OP_EQ (info1, info2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareSerialInfoNotEqualTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSerialInfoNotEqualTest_data ()
        {
        QTest::addColumn<QString> ("name1");
        QTest::addColumn<int> ("baud1");
        QTest::addColumn<QString> ("name2");
        QTest::addColumn<int> ("baud2");

#ifdef Q_OS_UNIX
        QTest::newRow ("Same Baud") << "/dev/ttyp0" << 1234 << "/dev/ttyp1" << 1234;
        QTest::newRow ("Same Name") << "/dev/ttyp0" << 1234 << "/dev/ttyp0" << 5678;
        QTest::newRow ("Both different") << "/dev/ttyp0" << 1234 << "/dev/ttyp1" << 5678;
#else
        QTest::newRow ("Same Baud") << "COM1" << 1234 << "COM2" << 1234;
        QTest::newRow ("Same Name") << "COM1" << 1234 << "COM1" << 5678;
        QTest::newRow ("Both different") << "COM1" << 1234 << "COM2" << 5678;
#endif
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test comparing two unalike serial infos
    ///
    /// @see    utils::device::serialInfo::operator!=()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSerialInfoNotEqualTest ()
        {
        QFETCH (QString, name1);
        QFETCH (int, baud1);

        QFETCH (QString, name2);
        QFETCH (int, baud2);

        serialInfo info1{ QSerialPortInfo{ name1 }, baud1 };
        serialInfo info2{ QSerialPortInfo{ name2 }, baud2 };

        COMPARE_OP_NE (info1, info2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareSocketInfoTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSocketInfoTest_data ()
        {
        QTest::addColumn<HostInfo> ("host");
        QTest::addColumn<int> ("port");

        QTest::addRow ("Hostname")
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 80;

        QTest::addRow ("IP")
            << HostInfo::fromString ("8.8.8.8", HostInfo::type::IP)
            << 53;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two socket infos
    ///
    /// @see    utils::device::socketInfo::operator==()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSocketInfoTest ()
        {
        QFETCH (HostInfo, host);
        QFETCH (int, port);

        socketInfo socket1{ host, (portNumber_t) port };
        socketInfo socket2{ host, (portNumber_t) port };

        COMPARE_OP_EQ (socket1, socket2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareSocketInfoNotEqualTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSocketInfoNotEqualTest_data ()
        {
        QTest::addColumn<HostInfo> ("host1");
        QTest::addColumn<int> ("port1");

        QTest::addColumn<HostInfo> ("host2");
        QTest::addColumn<int> ("port2");


        QTest::addRow ("Hostname Same")
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 80
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 443;

        QTest::addRow ("Hostname Different")
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 80
            << HostInfo::fromString ("google.com",
                                     HostInfo::type::HOSTNAME)
            << 80;

        QTest::addRow ("IP Same")
            << HostInfo::fromString ("8.8.8.8", HostInfo::type::IP)
            << 53
            << HostInfo::fromString ("8.8.8.8", HostInfo::type::IP)
            << 853;

        QTest::addRow ("IP Different")
            << HostInfo::fromString ("8.8.8.8", HostInfo::type::IP)
            << 53
            << HostInfo::fromString ("8.8.4.4", HostInfo::type::IP)
            << 53;

        QTest::addRow ("Hostname and IP")
            << HostInfo::fromString ("142.250.73.67",
                                     HostInfo::type::HOSTNAME)
            << 80
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 80;

        QTest::addRow ("All different")
            << HostInfo::fromString ("142.250.73.67",
                                     HostInfo::type::IP)
            << 80
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 443;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two unalike socket infos
    ///
    /// @see    utils::device::socketInfo::operator!=()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareSocketInfoNotEqualTest ()
        {
        QFETCH (HostInfo, host1);
        QFETCH (int, port1);

        QFETCH (HostInfo, host2);
        QFETCH (int, port2);

        socketInfo socket1{ host1, (portNumber_t) port1 };
        socketInfo socket2{ host2, (portNumber_t) port2 };

        COMPARE_OP_NE (socket1, socket2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareDeviceInfoSerialTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareDeviceInfoSerialTest_data ()
        {
        generateBaudRates ();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two serial device infos
    ///
    /// @see    utils::device::deviceInfo::operator==()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareDeviceInfoSerialTest ()
        {
        QFETCH (QString, name);
        QFETCH (int, baud);

        deviceInfo dev1{ TYPE_SERIAL, serialInfo{ QSerialPortInfo{ name }, baud } };
        deviceInfo dev2{ TYPE_SERIAL, serialInfo{ QSerialPortInfo{ name }, baud } };

        COMPARE_OP_EQ (dev1, dev2);
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareDeviceInfoSocketTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareDeviceInfoSocketTest_data ()
        {
        QTest::addColumn<HostInfo> ("host");
        QTest::addColumn<int> ("port");
        QTest::addColumn<type> ("type");

        QTest::addRow ("Hostname")
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 80
            << TYPE_TCP;

        QTest::addRow ("IP")
            << HostInfo::fromString ("8.8.8.8", HostInfo::type::IP)
            << 53
            << TYPE_TCP;


        QTest::addRow ("Hostname")
            << HostInfo::fromString ("google.ca",
                                     HostInfo::type::HOSTNAME)
            << 80
            << TYPE_UDP;

        QTest::addRow ("IP")
            << HostInfo::fromString ("8.8.8.8", HostInfo::type::IP)
            << 53
            << TYPE_UDP;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two socket device infos
    ///
    /// @see    utils::device::deviceInfo::operator==()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareDeviceInfoSocketTest ()
        {
        QFETCH (HostInfo, host);
        QFETCH (int, port);
        QFETCH (utils::device::type, type);

        deviceInfo dev1{ type, socketInfo{ host, (portNumber_t) port } };
        deviceInfo dev2{ type, socketInfo{ host, (portNumber_t) port } };

        COMPARE_OP_EQ (dev1, dev2);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Test data for compareDeviceInfoNotEqualTest()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareDeviceInfoNotEqualTest_data ()
        {
        static const deviceInfo DEVICES[] =
            {
                // Serial ports
                { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM1" }, 9600  } },
                { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM2" }, 9600  } },
                { TYPE_SERIAL, serialInfo{ QSerialPortInfo{ "COM1" }, 115200  } },

                // Sockets
                { TYPE_TCP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 80 } },
                { TYPE_TCP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 443 } },
                { TYPE_TCP, socketInfo{ HostInfo::fromString ("8.8.8.8",   HostInfo::type::IP), 80 } },
                { TYPE_TCP, socketInfo{ HostInfo::fromString ("8.8.4.4",   HostInfo::type::IP), 443 } },

                { TYPE_UDP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 80 } },
                { TYPE_UDP, socketInfo{ HostInfo::fromString ("google.ca", HostInfo::type::HOSTNAME), 443 } },
                { TYPE_UDP, socketInfo{ HostInfo::fromString ("8.8.8.8",   HostInfo::type::IP), 80 } },
                { TYPE_UDP, socketInfo{ HostInfo::fromString ("8.8.4.4",   HostInfo::type::IP), 443 } },
            };


        QTest::addColumn<deviceInfo> ("dev1");
        QTest::addColumn<deviceInfo> ("dev2");

        for (size_t ii = 0; ii < std::size (DEVICES); ++ii)
            {
            for (size_t jj = 0; jj < std::size (DEVICES); ++jj)
                {
                if (ii != jj)
                    {
                    QTest::addRow ("DEVICES[%d] != DEVCIES[%d]", ii, jj) << DEVICES[ii] << DEVICES[jj];
                    }
                }
            }

        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Compare two unalike device infos
    ///
    /// @see    utils::device::deviceInfo::operator!=()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void compareDeviceInfoNotEqualTest ()
        {
        QFETCH (deviceInfo, dev1);
        QFETCH (deviceInfo, dev2);

        COMPARE_OP_NE (dev1, dev2);
        }
    };


QTEST_GUILESS_MAIN (DeviceTest);

#include "devicetest.moc"
