/**
 * @file        utils/pinger.cpp
 * @brief       Interface for performing an ICMP ping
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/pinger.hpp>
#include <utils/log.hpp>

#ifdef Q_OS_WIN

#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include <comdef.h>
#include <wdmguid.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#endif // Q_OS_WIN

#ifdef  Q_OS_UNIX

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>

#endif //  Q_OS_UNIX

#ifdef Q_OS_LINUX
#include <icmp.h>
#endif // Q_OS_LINUX


namespace utils
{

namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
/// Pinger implementation base class. Defines interface only, as actual implementation is dependent on
/// platform and IP version
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class PingerImpl
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Ping the address. Virtual function, interface only
    ///
    /// @return     Ping result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Pinger::result ping () = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~PingerImpl() {}
    };

// Platform and version dependent implementations

#if defined (Q_OS_WIN) || defined (DOXYGEN)

///////////////////////////////////////////////////////////////////////////////
/// Common implementation of pinger for windows between IPv4 and IPv6
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class PingerWin :  public PingerImpl
    {
public:

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  hIcmpFile       Handle to the ICMP socket
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit PingerWin (HANDLE hIcmpFile) :
        m_hIcmpFile (hIcmpFile)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor, closes the ICMP socket
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~PingerWin () { IcmpCloseHandle (m_hIcmpFile); }

protected:
    HANDLE m_hIcmpFile = NULL;  ///< ICMP socket handle
    };

///////////////////////////////////////////////////////////////////////////////
/// Windows IPv4 Pinger implementation
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class IPv4Pinger : public PingerWin
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// IPv4 pinger constructor
    ///
    /// @param[in]  ip      IPv4 address (TODO: what endianess was this)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit IPv4Pinger (quint32 ip) :
        PingerWin (IcmpCreateFile ()),
        m_ip (_byteswap_ulong (ip))
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Perform a ping of the IPv4 address
    ///
    /// @return     Ping result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Pinger::result ping () override
        {
        Pinger::result      res;

        char                sendData[32] = "Data Buffer";
        size_t              size    = sizeof (ICMP_ECHO_REPLY) + sizeof (sendData);
        std::vector<uchar>  buffer;

        buffer.resize (size);

        DWORD dwRetVal = IcmpSendEcho (m_hIcmpFile,
                                       m_ip,
                                       sendData,
                                       sizeof (sendData),
                                       NULL,
                                       buffer.data (),
                                       static_cast<DWORD> (size),
                                       200);

        if (0 == dwRetVal)
            {
            logWinDebug (IcmpSendEcho,
                         QHostAddress{
                            _byteswap_ulong (m_ip) }.toString ());
            }
        else
            {
            ICMP_ECHO_REPLY& echoReply = *reinterpret_cast<PICMP_ECHO_REPLY>
                                                                    (buffer.data ());

            res.roundtrip   = std::chrono::milliseconds (echoReply.RoundTripTime);
            res.status      = echoReply.Status;
            res.success     = true;

            res.tos         = echoReply.Options.Tos;
            res.ttl         = echoReply.Options.Ttl;
            res.flags       = echoReply.Options.Flags;
            }

        return res;
        }

private:
    IPAddr m_ip;    ///< IPv4 address to ping
    };

///////////////////////////////////////////////////////////////////////////////
/// Windows IPv6 Pinger implementation
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class IPv6Pinger : public PingerWin
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Pv6 pinger constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit IPv6Pinger (Q_IPV6ADDR ip) :
        PingerWin (Icmp6CreateFile ()),
        m_ip ({ 0 })
        {
        std::copy (ip.c,
                   ip.c + std::size (ip.c),
                   m_ip.sin6_addr.u.Byte);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Perform a ping of the IPv6 address
    ///
    /// @return     Ping result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Pinger::result ping () override
        {
        Pinger::result      res;

        char                sendData[32] = "Data Buffer";
        size_t              size = sizeof (ICMPV6_ECHO_REPLY) +
                                   // sizeof (IO_STATUS_BLOCK) +
                                   sizeof (sendData) + 8;
        std::vector<uchar>  buffer;

        buffer.resize (size);

        sockaddr_in6            src{ 0 };
        IP_OPTION_INFORMATION   options;

        DWORD dwRetVal = Icmp6SendEcho2 (m_hIcmpFile,
                                         NULL,
                                         NULL,
                                         NULL,
                                        &src,
                                        &m_ip,
                                         sendData,
                                         static_cast<WORD> (sizeof (sendData)),
                                        &options,
                                         buffer.data (),
                                         static_cast<int> (buffer.size ()),
                                         200);


        if (0 == dwRetVal)
            {
            logWinDebug (Icmp6SendEcho2);
            }
        else
            {
            ICMPV6_ECHO_REPLY& echoReply = *reinterpret_cast<PICMPV6_ECHO_REPLY>
                                                                    (buffer.data ());

            res.roundtrip   = std::chrono::milliseconds (echoReply.RoundTripTime);
            res.status      = echoReply.Status;
            res.success     = true;

            res.tos         = options.Tos;
            res.ttl         = options.Ttl;
            res.flags       = options.Flags;
            }

        return res;
        }

private:
    sockaddr_in6 m_ip;  ///< IPv6 address
    };

#endif // deifned (Q_OS_WIN) || defined (DOXYGEN)

#if defined (Q_OS_UNIX)

///////////////////////////////////////////////////////////////////////////////
/// Calculate the ICMP checksum
///
/// @param[in]  b       data to calculate checksum for
/// @param[in]  len     Length of data in bytes
///
/// @return     checksum
///
///////////////////////////////////////////////////////////////////////////////
static ushort calculateChecksum (const void* b, int len)
    {
    const ushort*   buf = static_cast<const ushort*> (b);
    uint            sum = 0;
    ushort          result;

    for (sum = 0; len > 1; len -= 2)
        {
        sum += *buf++;
        }

    if (len == 1)
        {
        sum += *reinterpret_cast<const uchar*> (buf);
        }

    sum  = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    result = ~sum;

    return result;
    }


///////////////////////////////////////////////////////////////////////////////
/// POSIX IPv4 Pinger implementation
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class IPv4Pinger : public PingerImpl
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// IPv4 pinger constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit IPv4Pinger (quint32 ip):
        m_ip ({  htonl (ip) })
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Perform a ping of the IPv4 address
    ///
    /// @return     Ping result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual Pinger::result ping () override
        {
        Pinger::result      res;
\
        struct icmp         icmpHdr;
        struct sockaddr_in  addr;
        int                 sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
        int                 id  = htons(getpid ());

        memset (&addr, 0, sizeof addr);

        addr.sin_family = AF_INET;
        addr.sin_addr   = m_ip;
        addr.sin_len    = sizeof (addr);

        memset (&icmpHdr, 0, sizeof (icmpHdr));

        icmpHdr.icmp_type   = ICMP_ECHO;
        icmpHdr.icmp_code   = 0;
        icmpHdr.icmp_id     = id;
        icmpHdr.icmp_seq    = 0;

        if (sock < 0)
            {
            qWarning () << "Failed to open socket";
            }
        else
            {
            unsigned char   data[sizeof (icmpHdr) + 56];
            int             rc;
            struct timeval  timeout = { 1, 0 }; //wait max 1 seconds for a reply
            fd_set          read_set;
            socklen_t       slen;
            struct icmp     rcv_hdr;

#ifdef Q_OS_MACOS
            int hold = 1;
            setsockopt(sock,
                       SOL_SOCKET,
                       0x1104, // SO_RECV_ANYIF,
                       (char *)&hold,
                       sizeof(hold));
#endif // Q_OS_MACOS

            memcpy (data, &icmpHdr, sizeof (icmpHdr));

            struct icmp* icmpData = reinterpret_cast<icmp*> (data);

            icmpData->icmp_cksum = calculateChecksum (data, std::size (data));

            std::chrono::time_point<std::chrono::system_clock> sentTime =
                std::chrono::system_clock::now ();

            rc = sendto (sock,
                         data,
                         sizeof (data),
                         0,
                         reinterpret_cast <struct sockaddr*> (&addr),
                         sizeof (addr));

            if (rc <= 0)
                {
                qWarning () << "sendto() failed: rc = " << rc;
                }
            else
                {
                FD_ZERO (&read_set);
                FD_SET (sock, &read_set);

                //wait for a reply with a timeout
                rc = select (sock + 1, &read_set, NULL, NULL, &timeout);

                if (0 == rc)
                    {
                    char str[INET_ADDRSTRLEN];

                    inet_ntop (AF_INET, &addr.sin_addr, str, std::size (str));

                    qDebug () << "Ping" << str << "timeout";
                    }
                else if (rc < 0)
                    {
                    qWarning () << "select() failed: rc = " << rc;
                    }
                else
                    {
                    slen    = 0;
                    rc      = recvfrom (sock, data, sizeof (data), 0, NULL, &slen);

                    if (rc <= 0)
                        {
                        qDebug () << "recvfrom() failed: rc = " << rc;
                        }
                    else if (rc < sizeof (rcv_hdr))
                        {
                        qDebug () << "Response smaller than ICMP header: " << rc << " bytes";
                        }
                    else
                        {
                        memcpy (&rcv_hdr, data, sizeof (rcv_hdr));

                        if ('E' == rcv_hdr.icmp_type)
                            {
                            res.roundtrip   = std::chrono::duration_cast<std::chrono::milliseconds> (
                                                            std::chrono::system_clock::now () - sentTime);
                            res.flags       = 0;
                            res.status      = 0;
                            res.ttl         = 64;
                            res.success     = true;
                            }
                        }
                    }
                }
            close (sock);
            }

        return res;
        }

private:
    struct in_addr m_ip;    ///< IPv4 address to ping
    };

// TODO: IPv6 POSIX implementation
class IPv6Pinger : public PingerImpl
    {
    public:
        explicit IPv6Pinger (Q_IPV6ADDR ip)
            {
            std::copy (ip.c,
                       ip.c + std::size (ip.c),
                       m_ip.s6_addr);
            }


    private:
        struct in6_addr m_ip;
    };


#endif // defined (Q_OS_UNIX)

} // namespace anonymous

// Exported class functions

Pinger::Pinger (const QHostAddress& ip)
    {
    bool ok = true;

    if (QHostAddress::IPv4Protocol == ip.protocol ())
        {
        m_impl = new IPv4Pinger{ ip.toIPv4Address (&ok) };
        }
    else // (QHostAddress::IPv6Protocol == ip.protocol ())
        {
        // TODO: implement ICMP on IPv6 on Unix
#ifndef Q_OS_UNIX
        m_impl = new IPv6Pinger{ ip.toIPv6Address () };
#endif
        }

    if (not ok)
        {
        delete m_impl;
        throw std::runtime_error{ "Invalid Host Address" };
        }
    }

Pinger::~Pinger ()
    {
    delete m_impl;
    }

Pinger::result Pinger::ping ()
    {
    return m_impl->ping ();
    }
} // namespace utils
