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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>

#endif //  Q_OS_UNIX



namespace utils
{


///////////////////////////////////////////////////////////////////////////////
/// Pinger implementation base class. Defines interface only, as actual implementation is dependent on
/// platform and IP version
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
    
#ifdef Q_OS_WIN

///////////////////////////////////////////////////////////////////////////////
/// Common implementation of pinger for windows between IPv4 and IPv6
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
///////////////////////////////////////////////////////////////////////////////
class IpV4Pinger : public PingerWin
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// IPv4 pinger constructor
    ///
    /// @param[in]  ip      IPv4 address (TODO: what endianess was this)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit IpV4Pinger (quint32 ip) :
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
///////////////////////////////////////////////////////////////////////////////
class IpV6Pinger : public PingerWin
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Pv6 pinger constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit IpV6Pinger (Q_IPV6ADDR ip) :
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

            res.tos         = options.Tos;
            res.ttl         = options.Ttl;
            res.flags       = options.Flags;
            }

        return res;
        }

private:
    sockaddr_in6 m_ip;  ///< IPv6 address
    };

#endif // Q_OS_WIN

#ifdef Q_OS_UNIX

///////////////////////////////////////////////////////////////////////////////
/// POSIX IPv4 Pinger implementation
///
///////////////////////////////////////////////////////////////////////////////
class IpV4Pinger : public PingerImpl
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// IPv4 pinger constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit IpV4Pinger (quint32 ip):
        m_ip (__builtin_bswap32 (ip))
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

        struct icmphdr      icmpHdr;
        struct sockaddr_in  addr;
        int                 sequence = 0;
        int                 sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

        memset (&addr, 0, sizeof addr);

        addr.sin_family = AF_INET;
        addr.sin_addr   = m_ip;

        memset (&icmpHdr, 0, sizeof icmpHdr);

        icmpHdr.type        = ICMP_ECHO;
        icmpHdr.un.echo.id  = 1234;//arbitrary id


        if (sock < 0)
            {
            qWarn () << "Failed to open socket";
            }
        else
            {
            while (true)
                {
                unsigned char   data[2048];
                int             rc;
                struct timeval  timeout = { 3, 0 }; //wait max 3 seconds for a reply
                fd_set          read_set;
                socklen_t       slen;
                struct icmphdr  rcv_hdr;

                icmpHdr.un.echo.sequence = sequence++;

                memcpy (data, &icmpHdr, sizeof (icmpHdr));
                memcpy (data + sizeof (icmpHdr), "hello", 5); //icmp payload

                std::chrono::time_point<std::chrono::system_clock> sentTime =
                    std::chrono::system_clock::now ();

                rc = sendto (sock,
                             data,
                             sizeof (icmpHdr) + 5,
                             0,
                             static_cast <struct sockaddr*> (&addr),
                             sizeof (addr));
                if (rc <= 0)
                    {
                    break;
                    }

                memset (&read_set, 0, sizeof (read_set));
                FD_SET (sock, &read_set);

                //wait for a reply with a timeout
                rc = select (sock + 1, &read_set, NULL, NULL, &timeout);

                if (rc == 0)
                    {
                    continue;
                    }
                else if (rc < 0)
                    {
                    break;
                    }

                slen    = 0;
                rc      = recvfrom (sock, data, sizeof (data), 0, NULL, &slen);

                if (rc <= 0)
                    {
                    break;
                    }
                else if (rc < sizeof (rcv_hdr))
                    {
                    break;
                    }

                memcpy (&rcv_hdr, data, sizeof (rcv_hdr));

                if (ICMP_ECHOREPLY == rcv_hdr.type)
                    {
                    res.roundtrip   = std::chrono::system_clock::now () - sentTime;
                    res.flags       = 0;
                    res.status      = 0;
                    res.ttl         = 64;
                    break;
                    }
                else
                    {
                    break;
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
        explicit IpV6Pinger (Q_IPV6ADDR ip)
            {
            std::copy (ip.c,
                       ip.c + std::size (ip.c),
                       m_ip.s6_addr);
            }


    private:
        struct in6_addr m_ip;
    };


#endif // Q_OS_UNIX

// Exported class functions

Pinger::Pinger (const QHostAddress& ip)
    {
    bool ok = true;

    if (QHostAddress::IPv4Protocol == ip.protocol ())
        {
        m_impl = new IpV4Pinger{ ip.toIPv4Address (&ok) };
        }
    else // (QHostAddress::IPv6Protocol == ip.protocol ())
        {
        m_impl = new IpV6Pinger{ ip.toIPv6Address () };
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
