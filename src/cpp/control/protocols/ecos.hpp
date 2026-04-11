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

// ECoS Protocol version information
struct ecosVersion
    {
    version protocol;       ///< Procol version
    version application;    ///< Application version
    version hardware;       ///< HArdware version
    };


///////////////////////////////////////////////////////////////////////////////
/// ESU's ECoS protocl, version 0.1
///
///////////////////////////////////////////////////////////////////////////////
class ECoSProtocol : public ProtocolBase
    {
    PROTOCOL_DEFINE (ECoSProtocol,
                     "ECoS Protocol",
                     utils::device::TYPE_TCP,   // Default type
                     15471,                     // Default port
                     utils::device::TYPE_TCP);  // Only support TCP
public:
    using dynamicId = uint16_t;         ///< Dynamic ID (i.e. individual loco or actuator)

    ///////////////////////////////////////////////////////////////////////////////
    /// Static IDs. These are hard coded IDs taken from the protocol standard. Pinned to be the same size as
    /// the dynamic ID so they can be used interchangably
    ///
    ///////////////////////////////////////////////////////////////////////////////
    enum staticId : dynamicId
        {
        ID_ECOS                 = 1,    ///< Controller itself
        ID_PROGRAMMING_TRACK    = 5,    ///< Programming track
        ID_LOCOMOTIVES          = 10,   ///< List of configured locomotives
        ID_SWITCHING_ITEMS      = 11,   ///< List of configured switching items (actuators and routes)
        ID_SHUTTLE_TRAIN        = 12,   ///< Shuttle train configuration
        ID_DEVICE_MANAGER       = 20,   ///< Device manager
        ID_SNIFFER              = 25,   ///< Sniffer bus
        ID_S88_MANAGER          = 26,   ///< S-88 (sensor) manager
        ID_BOOSTER              = 27,   ///< Booster control
        ID_CONTROL_DESK         = 31,   ///< Control desk
        };

    // Supported commands
    enum cmd
        {
        CMD_QUERY_OBJECTS,  ///< Return a list of objects for this ID
        CMD_SET,            ///< Set a value
        CMD_GET,            ///< Get a value
        CMD_CREATE,         ///< Create a new instance of an object
        CMD_DELETE,         ///< Delete an object
        CMD_REQUEST,        ///< Request access to an object (view or control)
        CMD_RELEASE,        ///< Release access to an object (view or control)
        CMD_LINK,           ///< Link two or more object

        NUM_CMD             ///< delimiter only
        };


    // Command argument types
    enum argType
        {
        ARG_VIEW,           ///< Request view access to an object
        ARG_STOP,           ///< Stop command (Emegency/global stop)
        ARG_GO,             ///< Go command (clears emergency stop)
        ARG_INFO,           ///< Get info for an object
        ARG_STATUS,         ///< Status of an object
        ARG_NAME,           ///< UI name of an object
        ARG_ADDR,           ///< DCC/MFX address
        ARG_PROTOCOL,       ///< Protocol (i.e. Motorolla, MFX, DCC)
        ARG_APPEND,         ///< Add to the end of a list
        ARG_DISCARD,        ///< Discard
        ARG_CONSIST,        ///< Create a consist
        ARG_ROUTE,          ///< Create a route
        ARG_DELAY,          ///< Delay value
        ARG_SYMBOL,         ///< Symbol to represent an object in the UI
        ARG_PROFILE,        ///< Profile
        ARG_SNIFFER,        ///< Sniffer
        ARG_FAVORIT,        ///< Set/get the "favourite" flag
        ARG_SPEEDINDICATOR, ///< Speed indictaor
        ARG_SPEED,          ///< Speed value
        ARG_SPEEDSTEP,      ///< Size of a speed step
        ARG_DIR,            ///< Direction (forward or reverse)
        ARG_STATE,          ///< State (boolean)
        ARG_ADDRTEXT,       ///< Address Text
        ARG_MODE,           ///< Mode
        ARG_DURATION,       ///< duration
        ARG_PORTS,          ///< Sensor ports
        ARG_CONTROL,        ///< Control of an object (exclusive)
        ARG_FORCE,          ///< Force a previous argument
        ARG_FUNC,           ///< Locomotive function
        ARG_FUNCSYMBOL,     ///< Symbol for a function in the UI
        ARG_ID,             ///< Reference another ID

        ARG_NAME1,          ///< Switching item UI name line 1
        ARG_NAME2,          ///< Switching item UI name line 2
        ARG_NAME3,          ///< Switching item UI name line 3

        NUM_ARGS            ///< Delimiter only
        };


    // Status of a command, extracted from the reply
    enum replyStatus
        {
        REPLY_OK,               ///< Command suceeded
        REPLY_NERROR_NOAPPEND,  ///< Could not append the object
        REPLY_NERROR_NOCONTROL, ///< You do not have control of this object

        REPLY_EMPTY             ///< Not a real status, used to signal an unitialized status object
        };

    // Status of a locomotive
    enum locoStatus
        {
        LOCO_STOP,      ///< Locomotive stopped
        LOCO_GO,        ///< Locomotive running
        LOCO_SHUTDOWN   ///< Shutdown
        };

    // range value used in an argument
    struct range
        {
        uint16_t min;   ///< Minimum value
        uint16_t max;   ///< Maximum value
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Serialize this argument to send
        ///
        /// @return     Serialized range
        ///
        ///////////////////////////////////////////////////////////////////////////////
        std::string serialize () const;
        };
        
    // Set of four integer values used in an argument
    struct quad
        {
        uint16_t val1;
        uint16_t val2;
        uint16_t val3;
        uint16_t val4;
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Serialize this argument to send
        ///
        /// @return     Serialized range
        ///
        ///////////////////////////////////////////////////////////////////////////////
        std::string serialize () const;
        };

    // Represents a command argument
    struct arg
        {
        argType                     type;   ///< Argument type
        std::optional<std::string>  val;    ///< Argument value
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Serialize this argument to send
        ///
        /// @return     Serialized range
        ///
        ///////////////////////////////////////////////////////////////////////////////
        std::string serialize () const;
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Implicit constructor from argument type. Allows you to just pass the enumerated type when the argument
        /// requires no value
        ///
        /// @param[in]  type        Argument type
        ///
        ///////////////////////////////////////////////////////////////////////////////
        arg (argType type) :
            type (type)
            {}
        
        ///////////////////////////////////////////////////////////////////////////////
        /// String argument contructor
        ///
        /// @param[in]  type            Argument type
        /// @param[in]  vale            String argument value
        ///
        ///////////////////////////////////////////////////////////////////////////////
        arg (argType type, const std::string& val) :
            type (type),
            val (val)
            {}

        
        ///////////////////////////////////////////////////////////////////////////////
        /// Range argument constructor
        ///
        /// @param[in]  type            Argument type
        /// @param[in]  range          Range argument value
        ///
        ///////////////////////////////////////////////////////////////////////////////
        arg (argType type, const range& range) :
            type (type),
            val (range.serialize ())
            {}
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Integer argument constructor
        ///
        /// @param[in]  type        Argument type
        /// @param[in]  integer Argument integer value
        ///
        ///////////////////////////////////////////////////////////////////////////////
        arg (argType type, int integer) :
            type (type),
            val (std::to_string (integer))
            {}
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Boolean argument constructor
        ///
        /// @param[in]  type        Argument type
        /// @param[in]  b1            boolean argument value
        ///
        ///////////////////////////////////////////////////////////////////////////////
        arg (argType type, bool bl) :
            arg (type, bl ? 1 : 0)
            {}
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Quad argument constructor
        ///
        /// @param[in]  type        Argument type
        /// @param[in]  quad        Argument quad value
        ///
        ///////////////////////////////////////////////////////////////////////////////
        arg (argType type, const quad& quad) :
            type (type),
            val (quad.serialize ())
            {}
        };

    // Defauklt ECoS POrt
    static constexpr utils::device::portNumber_t DEFAULT_PORT = 15471;

    // Represents a line in a reply
    struct replyLine
        {
        dynamicId           id;     ///< ID of object
        std::optional<arg>  arg;    ///< Response argument
        };

    // Represents an ECoS replu
    struct reply
        {
        replyStatus             status = REPLY_EMPTY;   ///< Status code
        std::vector<replyLine>  lines;                  ///< Reply contents
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Implicit cast to bool, used to check if the reply is not empty
        ///
        /// @return     True if not empty
        ///
        ///////////////////////////////////////////////////////////////////////////////
        operator bool () { return REPLY_EMPTY != status; }
        };

        
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  deviceInfo      Device info to connect to controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ECoSProtocol (const utils::device::deviceInfo& deviceInfo) :
        ProtocolBase (deviceInfo, 1000)
        {}
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Query a given set of objects
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply queryObjects (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_QUERY_OBJECTS,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Query a given set of objects
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
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

        // todo: compile time checking of static IDs
        // static_assert (contains (allowedIds, id), "Invalid ID for queryObjexts");


        return queryObjects (id, std::forward<Args> (args)...);
        }

        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get an object or value
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply get (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_GET,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get an object or value
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<staticId id, class... Args>
    reply get (Args&&... args)
        {
        return get (id, std::forward<Args> (args)...);
        }

        
    ///////////////////////////////////////////////////////////////////////////////
    /// Set an object or value
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply set (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_SET,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Set an object or value
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<staticId id, class... Args>
    reply set (Args&&... args)
        {
        return set (id, std::forward<Args> (args)...);
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request acces to an object
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply request (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_REQUEST,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Request access to an object
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<staticId id, class... Args>
    reply request (Args&&... args)
        {
        return request (id, std::forward<Args> (args)...);
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Relase acces to an object
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply release (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_RELEASE,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Release access to an object
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<staticId id, class... Args>
    reply release (Args&&... args)
        {
        return release (id, std::forward<Args> (args)...);
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Create an object
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply create (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_CREATE,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Create an object
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<staticId id, class... Args>
    reply create (Args&&... args)
        {
        return create (id, std::forward<Args> (args)...);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Link two or more objects
    ///
    /// @tparam     Args     Command argument types
    ///
    /// @param[in]  id          Object ID
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class... Args>
    reply link (dynamicId id, Args&&... args)
        {
        return makeRequest (CMD_LINK,
                            id,
                            { std::forward<Args> (args)... });
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Link two or more objects
    ///
    /// @tparam     id          Object ID
    /// @tparam     Args      Command argument types
    ///
    /// @param[in]  args      Command arguments
    ///
    /// @return     Reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<staticId id, class... Args>
    reply link (Args&&... args)
        {
        return link (id, std::forward<Args> (args)...);
        }
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Issue a command to the ECoS controlelr
    ///
    /// @param[in]  cmd     Command code
    /// @param[in]  id       Object ID
    /// @param[in]  args   Command arguments
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void issueCommand (cmd cmd, dynamicId id, std::initializer_list<arg> args);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Read the reply to the last command from the device
    ///
    /// @return     reply from controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    reply readReply ();
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Make a request. This sends a command to the controller, parses and returns the reply
    ///
    /// @param[in]  cmd     Command code
    /// @param[in]  id       Object ID
    /// @param[in]  args  Command arguments
    ///
    ///////////////////////////////////////////////////////////////////////////////
    reply makeRequest (cmd cmd, dynamicId id, std::initializer_list<arg> args);

    };



} // namespace control
