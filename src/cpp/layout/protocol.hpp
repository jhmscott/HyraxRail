/**
 * @file        layout/protocol.hpp
 * @brief       Track protocol definitions
 * @author      Justin Scott
 * @date        2026-08-22
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <bitset>

#include <utils/resources.hpp>

namespace layout
{

/// Protocol to communicate between the controller and loco
enum trackProtocol
    {
    TRACK_PROTO_DCC14,  ///< NMRA DCC, Digital Command Control @image html dcc.svg width=32
    TRACK_PROTO_DCC28,  ///< NMRA DCC, Digital Command Control @image html dcc.svg width=32
    TRACK_PROTO_DCC128, ///< NMRA DCC, Digital Command Control @image html dcc.svg width=32

    TRACK_PROTO_MFX,    ///< Märklin Digital @image html mfx.svg width=32

    TRACK_PROTO_MM14,   ///< Märklin-Motorola @image html mm.svg width=32
    TRACK_PROTO_MM27,   ///< Märklin-Motorola @image html mm.svg width=32
    TRACK_PROTO_MM28,   ///< Märklin-Motorola @image html mm.svg width=32

    TRACK_PROTO_UNKNOWN,///< Unknown protocol @image html question.svg width=32

    NUM_TRACK_PROTO     ///< Delimiter only
    };


/// Information about the track protocol
struct protocolInfo
    {
    utils::resources::Icon  icon;           ///< Icon to use for the resource
    const char*             description;    ///< Text description of the protocol
    uint                    numAddresses;   ///< Number of addresses supported by this protocol
    };


/// Lookup from tackProtocol enum value to protocol info
inline const protocolInfo PROTOCOLS[] =
    {
    { "trains/dcc",     "NMRA DCC 14",              127     },
    { "trains/dcc",     "NMRA DCC 28",              127     },
    { "trains/dcc",     "NMRA DCC 128",             9999    }, // technically, 10,239, but most controllers limit this to 9,999

    { "trains/mfx",     "MFX (Märklin Digital)",    16000   },

    { "trains/mm",      "Märklin-Motorola 14",      80      },
    { "trains/mm",      "Märklin-Motorola 27",      80      },
    { "trains/mm",      "Märklin-Motorola 28",      255     },

    { "misc/question",  "",                         0       }
    };
ASSERT_ARRAY_LENGTH (PROTOCOLS, NUM_TRACK_PROTO);


/// Set of protocols
using protocolMask = std::bitset<TRACK_PROTO_UNKNOWN>;

/// Convience for mask containing all protocols
inline constexpr protocolMask ALL_TRACK_PROTOCOLS{ (1 << layout::TRACK_PROTO_UNKNOWN) - 1 };

} // namespace layout