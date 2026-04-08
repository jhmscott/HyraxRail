/**
 * @file        proto/ecos.hpp
 * @brief       Implementation of the ECoS protocol used
 *              by early Märklin and ESU controllers
 * @author      Justin Scott
 * @date        2026-01-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/protocols/base.hpp>

#include <optional>

namespace control
{

struct ecosVersion
    {
    version protocol;
    version application;
    version hardware;
    };


class ECoSProtocol : public ProtocolBase
    {
    PROTOCOL_DEFINE (ECoSProtocol,
                     "ECoS Protocol",
                     utils::device::TYPE_TCP,
                     15471,
                     utils::device::TYPE_TCP);
public:
    enum version
        {
        VERSION_01,
        VERSION_02
        };

    using dynamicId = uint16_t;

    enum staticId : dynamicId
        {
        ID_ECOS                 = 1,
        ID_PROGRAMMING_TRACK    = 5,
        ID_LOCOMOTIVES          = 10,
        ID_SWITCHING_ITEMS      = 11,
        ID_SHUTTLE_TRAIN        = 12,
        ID_DEVICE_MANAGER       = 20,
        ID_SNIFFER              = 25,
        ID_S88_MANAGER          = 26,
        ID_BOOSTER              = 27,
        ID_CONTROL_DESK         = 31,
        };

    enum cmd
        {
        CMD_QUERY_OBJECTS,
        CMD_SET,
        CMD_GET,
        CMD_CREATE,
        CMD_DELETE,
        CMD_REQUEST,
        CMD_RELEASE,
        CMD_LINK,

        NUM_CMD         // delimiter only
        };


    enum argType
        {
        ARG_VIEW,
        ARG_STOP,
        ARG_GO,
        ARG_INFO,
        ARG_STATUS,
        ARG_NAME,
        ARG_ADDR,
        ARG_PROTOCOL,
        ARG_APPEND,
        ARG_DISCARD,
        ARG_CONSIST,
        ARG_ROUTE,
        ARG_DELAY,
        ARG_SYMBOL,
        ARG_PROFILE,
        ARG_SNIFFER,
        ARG_FAVORIT,
        ARG_SPEEDINDICATOR,
        ARG_SPEED,
        ARG_SPEEDSTEP,
        ARG_DIR,
        ARG_STATE,
        ARG_ADDRTEXT,
        ARG_MODE,
        ARG_DURATION,
        ARG_PORTS,
        ARG_CONTROL,
        ARG_FORCE,
        ARG_FUNC,
        ARG_FUNCSYMBOL,
        ARG_ID,

        ARG_NAME1,
        ARG_NAME2,
        ARG_NAME3,

        NUM_ARGS
        };


    enum replyStatus
        {
        REPLY_OK,
        REPLY_NERROR_NOAPPEND,
        REPLY_NERROR_NOCONTROL,

        REPLY_EMPTY
        };

    enum locoStatus
        {
        LOCO_STOP,
        LOCO_GO,
        LOCO_SHUTDOWN
        };

    struct range
        {
        uint16_t min;
        uint16_t max;

        std::string serialize () const;
        };

    struct quad
        {
        uint16_t val1;
        uint16_t val2;
        uint16_t val3;
        uint16_t val4;

        std::string serialize () const;
        };

    struct arg
        {
        argType                     type;
        std::optional<std::string>  val;

        std::string serialize () const;

        arg (argType type) :
            type (type)
            {}

        arg (argType type, const std::string& val) :
            type (type),
            val (val)
            {}

        arg (argType type, const range& range) :
            type (type),
            val (range.serialize ())
            {}

        arg (argType type, int integer) :
            type (type),
            val (std::to_string (integer))
            {}

        arg (argType type, bool bl) :
            arg (type, bl ? 1 : 0)
            {}


        arg (argType type, const quad& quad) :
            type (type),
            val (quad.serialize ())
            {}
        };

    static constexpr utils::device::portNumber_t DEFAULT_PORT = 15471;

    struct replyLine
        {
        dynamicId           id;
        std::optional<arg>  arg;
        };

    struct reply
        {
        replyStatus             status = REPLY_EMPTY;
        std::vector<replyLine>  lines;

        operator bool () { return REPLY_EMPTY != status; }
        };


    ECoSProtocol (const utils::device::deviceInfo& deviceInfo) :
        ProtocolBase (deviceInfo, 1000)
        {}

    template<class... Args>
    reply queryObjects (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_QUERY_OBJECTS,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply queryObjects (Args&&... args)
        {
        static const staticId allowedIds[] =
            {
            ID_LOCOMOTIVES,
            ID_SWITCHING_ITEMS,
            ID_SHUTTLE_TRAIN,
            ID_DEVICE_MANAGER,
            ID_S88_MANAGER
            };

        // static_assert (contains (allowedIds, id), "Invalid ID for queryObjexts");


        return queryObjects (id, std::forward<Args> (args)...);
        }

    template<class... Args>
    reply get (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_GET,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply get (Args&&... args)
        {
        return get (id, std::forward<Args> (args)...);
        }


    template<class... Args>
    reply set (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_SET,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply set (Args&&... args)
        {
        return set (id, std::forward<Args> (args)...);
        }


    template<class... Args>
    reply request (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_REQUEST,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply request (Args&&... args)
        {
        return request (id, std::forward<Args> (args)...);
        }


    template<class... Args>
    reply release (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_RELEASE,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply release (Args&&... args)
        {
        return release (id, std::forward<Args> (args)...);
        }

    template<class... Args>
    reply create (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_CREATE,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply create (Args&&... args)
        {
        return create (id, std::forward<Args> (args)...);
        }

    template<class... Args>
    reply link (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_LINK,
                            id,
                            { std::forward<Args> (args)... });
        }

    template<staticId id, class... Args>
    reply link (Args&&... args)
        {
        return link (id, std::forward<Args> (args)...);
        }
private:
    void issueCommand (cmd cmd, dynamicId id, std::initializer_list<arg> args);

    reply readReply ();

    reply makeRequest (cmd cmd, dynamicId id, std::initializer_list<arg> args);

    };



} // namespace control