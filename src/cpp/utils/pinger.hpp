/**
 * @file        utils/pinger.hpp
 * @brief       Interface for performing an ICMP ping
 * @author      Justin Scott
 * @date        2026-01-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QHostAddress>

#include <chrono>

namespace utils
{

// Implementation class for pimpl pattern
class PingerImpl;

///////////////////////////////////////////////////////////////////////////////
/// Utility to ping an address
///
///////////////////////////////////////////////////////////////////////////////
class Pinger
    {
public:
    // result of a ping operation
    struct result
        {
        std::chrono::milliseconds   roundtrip;  ///< Ping roundtrip time in milliseconds
        ulong                       status;     ///< Status code
        uchar                       ttl;        ///< IP header TTL value (time to live)
        uchar                       tos;        ///< IP header TOS value (type of service)
        uchar                       flags;      ///< IP header flags field
        };

        
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  ip          Address to ping
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit Pinger (const QHostAddress& ip);

    
    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~Pinger ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Ping this address
    ///
    /// @return     Result of the ping operation
    /// 
    ///////////////////////////////////////////////////////////////////////////////
    result ping ();

private:
    PingerImpl* m_impl;     ///< Pointer to implementation. Pimpl pattern

    };
} // namespace utils
