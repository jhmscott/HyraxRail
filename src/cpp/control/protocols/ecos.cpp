/**
 * @file        proto/ecos.cpp
 * @brief       Implementation of the ECoS protocol used
 *              by early Märklin and ESU controllers
 * @author      Justin Scott
 * @date        2026-01-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <control/protocols/ecos.hpp>
#include <utils/string.hpp>

#include <QStringTokenizer>

#include <string>
#include <sstream>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace control
{
static constexpr const char* const ARG_STRINGS[] =
    {
    "view",             // ARG_VIEW
    "stop",             // ARG_STOP
    "go",               // ARG_GO
    "info",             // ARG_INFO
    "status",           // ARG_STATUS
    "name",             // ARG_NAME
    "addr",             // ARG_ADDR
    "protocol",         // ARG_PROTOCOL
    "append",           // ARG_APPEND
    "discard",          // ARG_DISCARD
    "consist",          // ARG_CONSIST
    "route",            // ARG_ROUTE
    "delay",            // ARG_DELAY
    "symbol",           // ARG_SYMBOL
    "profile",          // ARG_PROFILE
    "sniffer",          // ARG_SNIFFER
    "favorit",
    "speedIndicator", // ?
    "speed",
    "speedStep", // ?
    "dir",
    "state",
    "addrText", // ?
    "mode",
    "duration",
    "ports",
    "control",
    "force",
    "func",
    "funcsymbol",
    "name1",
    "name2",
    "name3"
    };


static constexpr const char* const REPLY_STATUS_STRINGS[]
    {
    "(OK)",
    "(NERROR_NOAPPEND)",
    "(NERROR_NOCONTROL)"
    };


static ECoSProtocol::argType argTypeFromString (std::string_view str)
    {
    size_t size  = findArrayIdx (ARG_STRINGS, str);

    if (std::size (ARG_STRINGS) == size)
        {
        // throw ParserError ();
        }

    return static_cast<ECoSProtocol::argType> (size);
    }



static ECoSProtocol::arg argFromString (std::string_view str)
    {
    size_t valStart;
    size_t valEnd;

    if (std::string_view::npos == (valStart = str.find ('[')))
        {
        return ECoSProtocol::arg{ argTypeFromString (str) };
        }
    else
        {
        if (std::string_view::npos == (valEnd = str.find (']', valStart + 1)))
            {
            // throw ParserError ();
            }

        std::string type{ str.substr (0, valStart)};
        std::string val{ str.substr (valStart + 1, valEnd - valStart - 1) };

        std::string str {utils::str::extract (val, "\"", "\"")};

        if (!str.empty ())
            {
            return ECoSProtocol::arg{ argTypeFromString (type), str };
            }
        else
            {
            return ECoSProtocol::arg{ argTypeFromString (type), val };
            }
        }
    }

std::string ECoSProtocol::arg::serialize () const
    {
    std::string res;

    res = ARG_STRINGS[type];

    if (val.has_value ())
        {
        res += "[" + *val + "]";
        }

    return res;
    }

std::string ECoSProtocol::range::serialize () const
    {
    return std::to_string (min) + ", " + std::to_string (max);
    }

std::string ECoSProtocol::quad::serialize () const
    {
    return std::to_string (val1) + ", " +
           std::to_string (val2) + ", " +
           std::to_string (val3) + ", " +
           std::to_string (val4) + ", ";
    }

void ECoSProtocol::issueCommand (cmd cmd, dynamicId id, std::initializer_list<arg> args)
    {
    static constexpr std::array<const char*, ECoSProtocol::NUM_CMD> CMD_STRINGS =
        {
        "queryObjects",
        "set",
        "get",
        "create",
        "delete",
        "request",
        "release"
        };

    std::stringstream ss;

    ss << CMD_STRINGS[cmd];

    ss << "(" << static_cast<uint> (id);

    for (const auto& arg : args)
        {
        ss << ", " << arg.serialize ();
        }

    ss << ")";

    std::string msg = ss.str ();

    m_device->write (msg.c_str (), msg.size ());
    }

ECoSProtocol::reply ECoSProtocol::readReply ()
    {
    reply       reply;
    size_t      numBytes    = m_device->bytesAvailable ();

    if (numBytes > 0)
        {
        QByteArray  rawReply    = m_device->readAll ();
        auto        lines       = utils::str::tokenize (rawReply.toStdString (), "\r\n"s);

        auto parseLine =
            [] (const std::string& line) -> replyLine
            {
            auto [id, arg] = utils::str::split (line, " ");
            replyLine   res;

            res.id = std::stod (std::string{ id });

            if (not arg.empty ())
                {
                res.arg = argFromString (arg);
                }

            return res;
            };

        if (lines.size () < 2)
            {
            // throw error
            }
        else
            {
            std::transform (lines.begin () + 1,
                            lines.end () - 1,
                            std::back_inserter (reply.lines),
                            parseLine);

            std::string_view    status = utils::str::extract (lines.back(), "<END ", ">");

            if (status.empty ())
                {
                // throw error
                }
            else
                {
                auto [code, str] = utils::str::split (status, " ");

                size_t size = findArrayIdx (REPLY_STATUS_STRINGS, str);

                if (std::size (REPLY_STATUS_STRINGS) == size)
                    {
                    // throw error
                    }

                reply.status = static_cast<replyStatus> (size);
                }
            }
        }

    return reply;
    }

ECoSProtocol::reply ECoSProtocol::makeRequest (cmd cmd, dynamicId id, std::initializer_list<arg> args)
    {
    issueCommand (cmd, id, args);

    waitForData ();

    return readReply ();
    }

} // namespace control