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

class PingerImpl;

class Pinger
    {
public:
    struct result
        {
        std::chrono::milliseconds   roundtrip;
        ulong                       status;
        uchar                       ttl;
        uchar                       tos;
        uchar                       flags;
        };

    explicit Pinger (const QHostAddress& ip);

    ~Pinger ();

    result ping ();

private:
    PingerImpl* m_impl;

    };
}