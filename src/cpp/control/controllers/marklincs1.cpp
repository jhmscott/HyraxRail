/**
 * @file        controller/marklincs1.cpp
 * @brief       Controller implementation for the Märklin
 *              Central Station 1. Model number 60212
 *              This supports the oirginal fimrware,
 *              not the "reloaded" firmware from ESU
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/controllers/marklincs1.hpp>

#include <utils/pp.hpp>


#define GET_ARG_TYPE_0
#define GET_ARG_TYPE_1      ECoSProtocol::arg
#define GET_ARG_TYPE_2      ECoSProtocol::arg, ECoSProtocol::arg
#define GET_ARG_TYPE_3      ECoSProtocol::arg, ECoSProtocol::arg, ECoSProtocol::arg
#define GET_ARG_TYPE_4      ECoSProtocol::arg, ECoSProtocol::arg, ECoSProtocol::arg, ECoSProtocol::arg


#define GET_ARG_FOR_TYPES(...)  UTILPP_CONCAT (GET_ARG_TYPE_, UTILPP_VA_NUM_ARGS (__VA_ARGS__))


#define issueDynamicCommand(func, id, ...)  (m_thread.enqueue (&func<GET_ARG_FOR_TYPES (__VA_ARGS__)>,       \
                                                               static_cast<ECoSProtocol::dynamicId> (id)     \
                                                               __VA_OPT__(,)                                 \
                                                               __VA_ARGS__))

#define issueStaticCommand(func, id, ...)   (m_thread.enqueue (&func<id __VA_OPT__ (,) \
                                                              GET_ARG_FOR_TYPES (__VA_ARGS__)> __VA_OPT__(,) \
                                                               __VA_ARGS__))

#define issueDynamicBatchCommands(reply, func, arg, results)        \
    results.reserve (reply.lines.size ());                          \
    std::transform (reply.lines.begin (),                           \
                    reply.lines.end (),                             \
                    std::back_inserter (results),                   \
                    [this] (const ECoSProtocol::replyLine& line)    \
                    {                                               \
                    return issueDynamicCommand (func,               \
                                                line.id,            \
                                                arg);               \
                    });

#define ARG(type, val) (ECoSProtocol::arg{ (type), (val) })


using namespace std::chrono_literals;

namespace control
{

static const std::map<int, layout::actuatorIcon> ICON_TABLE =
    {
        { 23, layout::ICON_TURNOUT_LEFT  },
        { 24, layout::ICON_TURNOUT_RIGHT }
    };

//////////////////////////////////////////////////////////////////////////////
/// Extract the icon information from a reply
///
/// @param[in]  reply       Result of get(ARG_SYMBOL) request
///
/// @return     Contained icon
///
//////////////////////////////////////////////////////////////////////////////
static layout::actuatorIcon getActuatorIcon (const ECoSProtocol::reply& reply)
    {

    layout::actuatorIcon    icon    = layout::NO_ICON;
    auto                    it      = ICON_TABLE.find (atoi (reply.lines[0].arg->val->c_str ()));

    if (ICON_TABLE.end () != it)
        {
        icon = it->second;
        }

    return icon;
    }

static void resolveReplyVector (std::vector<std::future<ECoSProtocol::reply>>&  replyFutures,
                                std::vector<ECoSProtocol::reply>&               replies)
    {
    replies.reserve (replyFutures.size ());

    std::transform (replyFutures.begin (),
                    replyFutures.end (),
                    std::back_inserter (replies),
                    std::mem_fn (&std::future<ECoSProtocol::reply>::get));
    }

MarklinCS1::MarklinCS1 (const std::string& friendlyName, std::unique_ptr<ProtocolBase>&& proto) :
    ControllerBase (friendlyName, std::move (proto))
    {
    }

MarklinCS1::~MarklinCS1 ()
    {
    }

std::vector<layout::Locomotive> MarklinCS1::getLocomotives () const
    {
    std::vector<layout::Locomotive> locos;

    auto future = issueStaticCommand (ECoSProtocol::queryObjects,
                                      ECoSProtocol::ID_LOCOMOTIVES,
                                      ECoSProtocol::ARG_NAME);

    if (std::future_status::ready == future.wait_for (500ms))
        {
        auto res = future.get ();

        if (control::ECoSProtocol::REPLY_OK == res.status)
            {
            std::transform (res.lines.begin (),
                            res.lines.end (),
                            std::back_inserter (locos),
                            [this] (const control::ECoSProtocol::replyLine& line)
                            {
                            static const std::map<std::string, layout::trackProtocol> protocols
                                {
                                    // Märklin Digital
                                    { "MFX",    layout::TRACK_PROTO_MFX },

                                    // NMRA DCC (Digital Command Control)
                                    { "DCC14",  layout::TRACK_PROTO_DCC },
                                    { "DCC28",  layout::TRACK_PROTO_DCC },
                                    { "DCC128", layout::TRACK_PROTO_DCC },

                                    // Märklin-Motorola
                                    { "MM14",   layout::TRACK_PROTO_MM  },
                                    { "MM27",   layout::TRACK_PROTO_MM  },
                                    { "MM28",   layout::TRACK_PROTO_MM  },
                                };


                            auto future     = issueDynamicCommand (ECoSProtocol::get,
                                                                   line.id,
                                                                   ECoSProtocol::ARG_PROTOCOL);
                            auto protoReply = future.get ();
                            auto it         = protocols.find (protoReply.lines[0].arg->val.value ());

                            layout::trackProtocol proto = layout::TRACK_PROTO_UNKNOWN;

                            if (protocols.end () != it)
                                {
                                proto = it->second;
                                }

                            return layout::Locomotive{ const_cast<MarklinCS1*> (this),
                                                       line.arg->val.value (),
                                                       proto,
                                                       line.id, };
                            });
            }
        }

    return locos;
    }

std::vector<layout::Actuator> MarklinCS1::getActuators () const
    {
    std::vector<layout::Actuator> actuators;

    auto future = issueStaticCommand (ECoSProtocol::queryObjects,
                                      ECoSProtocol::ID_SWITCHING_ITEMS);


    auto reply = future.get ();
    auto it = std::remove_if (reply.lines.begin (),
                              reply.lines.end (),
                              [] (const ECoSProtocol::replyLine& line) -> bool
                              { return line.id >= 30000; });

    reply.lines.erase (it, reply.lines.end ());

    std::vector<std::future<ECoSProtocol::reply>>   nameReplies;
    std::vector<ECoSProtocol::reply>                names;

    issueDynamicBatchCommands (reply,
                               ECoSProtocol::get,
                               ECoSProtocol::ARG_NAME1,
                               nameReplies);

    std::vector<std::future<ECoSProtocol::reply>>   stateReplies;
    std::vector<ECoSProtocol::reply>                states;

    issueDynamicBatchCommands (reply,
                               ECoSProtocol::get,
                               ECoSProtocol::ARG_STATE,
                               stateReplies);

    std::vector<std::future<ECoSProtocol::reply>>   iconReplies;
    std::vector<ECoSProtocol::reply>                icons;

    std::vector<std::future<ECoSProtocol::reply>>   modeReplies;
    std::vector<ECoSProtocol::reply>                modes;

    std::vector<std::future<ECoSProtocol::reply>>   addressReplies;
    std::vector<ECoSProtocol::reply>                addresses;

    std::vector<std::future<ECoSProtocol::reply>>   durationReplies;
    std::vector<ECoSProtocol::reply>                durations;

    issueDynamicBatchCommands (reply,
                               ECoSProtocol::get,
                               ECoSProtocol::ARG_SYMBOL,
                               iconReplies);

    issueDynamicBatchCommands (reply,
                                ECoSProtocol::get,
                                ECoSProtocol::ARG_MODE,
                                modeReplies);

    issueDynamicBatchCommands (reply,
                                ECoSProtocol::get,
                                ECoSProtocol::ARG_ADDR,
                                addressReplies);

    issueDynamicBatchCommands (reply,
                               ECoSProtocol::get,
                               ECoSProtocol::ARG_DURATION,
                               durationReplies);

    resolveReplyVector (nameReplies, names);
    resolveReplyVector (stateReplies, states);
    resolveReplyVector (iconReplies,    icons);
    resolveReplyVector (modeReplies,    modes);
    resolveReplyVector (addressReplies, addresses);
    resolveReplyVector (durationReplies,durations);

    actuators.reserve (reply.lines.size ());

    for (size_t ii = 0; ii < reply.lines.size (); ++ii)
        {
        layout::actuatorMode mode;

        if ("PULSE" == modes[ii].lines[0].arg->val)
            {
            mode = layout::actuatorMode::PULSE;
            }
        else
            {
            mode = layout::actuatorMode::SWITCH;
            }

        actuators.emplace_back (const_cast<MarklinCS1*> (this),
                                *names[ii].lines[0].arg->val,
                                getActuatorIcon (icons[ii]),
                                mode,
                                atoi (addresses[ii].lines[0].arg->val->c_str ()),
                                atoi (durations[ii].lines[0].arg->val->c_str ()),
                                static_cast<size_t> (reply.lines[ii].id),
                                "1" == states[ii].lines[0].arg->val);
        }

    return actuators;
    }

std::vector<layout::Route> MarklinCS1::getRoutes () const
    {
    std::vector<layout::Route> routes;

    auto future = issueStaticCommand (ECoSProtocol::queryObjects,
                                      ECoSProtocol::ID_SWITCHING_ITEMS);


    auto reply = future.get ();
    auto it = std::remove_if (reply.lines.begin (),
                              reply.lines.end (),
                              [] (const ECoSProtocol::replyLine& line) -> bool
                              { return line.id < 30000; });

    reply.lines.erase (it, reply.lines.end ());

    std::vector<std::future<ECoSProtocol::reply>>   nameReplies;
    std::vector<ECoSProtocol::reply>                names;
    std::vector<std::future<ECoSProtocol::reply>>   stateReplies;
    std::vector<ECoSProtocol::reply>                states;

    issueDynamicBatchCommands (reply,
                               ECoSProtocol::get,
                               ECoSProtocol::ARG_NAME1,
                               nameReplies);

    issueDynamicBatchCommands (reply,
                               ECoSProtocol::get,
                               ECoSProtocol::ARG_STATE,
                               stateReplies);

    resolveReplyVector (nameReplies, names);
    resolveReplyVector (stateReplies, states);

    routes.reserve (reply.lines.size ());

    for (size_t ii = 0; ii < reply.lines.size (); ++ii)
        {
        size_t  routeId         = static_cast<size_t> (reply.lines[ii].id);
        auto    memberFutures   = issueDynamicCommand (ECoSProtocol::queryObjects,
                                                        routeId);
        auto    members         = memberFutures.get ();
        layout::routeList routeList;

        routeList.reserve (members.lines.size ());

        std::transform (members.lines.begin (),
                        members.lines.end (),
                        std::back_inserter (routeList),
                        [this, routeId] (const ECoSProtocol::replyLine& line) -> layout::routeMember
                        {
                        auto                stateFuture = issueDynamicCommand (ECoSProtocol::get,
                                                                                routeId,
                                                                                ARG (ECoSProtocol::ARG_ID, line.id),
                                                                                ECoSProtocol::ARG_STATE);

                        layout::Actuator    actuator    = getActuatorSingle (line.id);
                        auto                stateRes    = stateFuture.get ();
                        bool                state       = "1" == stateRes.lines[0].arg->val;

                        return { std::move (actuator), state };
                        });

        routes.emplace_back (const_cast<MarklinCS1*> (this),
                             *names[ii].lines[0].arg->val,
                              routeList,
                              routeId);
        }

    return routes;
    }

layout::Route MarklinCS1::createRoute (const std::string& name, const layout::routeList& actuators)
    {
    ECoSProtocol::dynamicId routeId = 0;

    issueStaticCommand (ECoSProtocol::create,
                        ECoSProtocol::ID_SWITCHING_ITEMS,
                        ECoSProtocol::ARG_ROUTE);

    auto future = issueStaticCommand (ECoSProtocol::create,
                                      ECoSProtocol::ID_SWITCHING_ITEMS,
                                      ECoSProtocol::ARG_APPEND);

    auto res = future.get ();

    if (ECoSProtocol::REPLY_OK == res.status)
        {
        routeId = atoi (res.lines[0].arg->val->c_str ());

        requestRouteControl (routeId);

        setRouteName    (routeId, name);
        setRouteMembers (routeId, actuators);

        releaseRouteControl (routeId);
        }

    return layout::Route{ this, name, actuators, routeId };
    }

layout::Actuator MarklinCS1::createActuator (const std::string&     name,
                                             uint                   address,
                                             layout::actuatorIcon   icon,
                                             layout::actuatorMode   mode,
                                             uint                   duration)
    {
    ECoSProtocol::dynamicId actuatorId = 0;

    issueStaticCommand (ECoSProtocol::create,
                        ECoSProtocol::ID_SWITCHING_ITEMS);

    auto future = issueStaticCommand (ECoSProtocol::create,
                                      ECoSProtocol::ID_SWITCHING_ITEMS,
                                      ECoSProtocol::ARG_APPEND);

    auto res = future.get ();

    if (ECoSProtocol::REPLY_OK == res.status)
        {
        actuatorId = atoi (res.lines[0].arg->val->c_str ());

        requestActuatorControl (actuatorId);

        setActuatorName     (actuatorId, name);
        setActuatorAddress  (actuatorId, address);
        setActuatorIcon     (actuatorId, icon);
        setActuatorMode     (actuatorId, mode);
        setActuatorDuration (actuatorId, duration);

        releaseActuatorControl (actuatorId);
        }

    return layout::Actuator{ this, name, icon, mode, address, duration, actuatorId, false };
    }


void MarklinCS1::eStop (bool stop)
    {
    if (stop)
        {
        issueStaticCommand (ECoSProtocol::set,
                            ECoSProtocol::ID_ECOS,
                            ECoSProtocol::ARG_STOP);
        }
    else
        {
        issueStaticCommand (ECoSProtocol::set,
                            ECoSProtocol::ID_ECOS,
                            ECoSProtocol::ARG_GO);
        }
    }

bool MarklinCS1::isEStopped ()
    {
    bool eStop = false;
    auto res = issueStaticCommand (ECoSProtocol::get,
                                   ECoSProtocol::ID_ECOS,
                                   ECoSProtocol::ARG_STATUS);

    try
        {
        auto reply = res.get ();
        eStop = "STOP" == *reply.lines[0].arg->val;
        }
    catch (...)
        {

        }


    return eStop;
    }


layout::Actuator MarklinCS1::getActuatorSingle (size_t id) const
    {
    auto nameFuture     = issueDynamicCommand (ECoSProtocol::get,
                                               id,
                                               ECoSProtocol::ARG_NAME1);
    auto stateFuture    = issueDynamicCommand (ECoSProtocol::get,
                                               id,
                                               ECoSProtocol::ARG_STATE);
    auto symFuture      = issueDynamicCommand (ECoSProtocol::get,
                                               id,
                                               ECoSProtocol::ARG_SYMBOL);
    auto modeFuture     = issueDynamicCommand (ECoSProtocol::get,
                                               id,
                                               ECoSProtocol::ARG_MODE);
    auto addrFuture     = issueDynamicCommand (ECoSProtocol::get,
                                               id,
                                               ECoSProtocol::ARG_ADDR);
    auto durFuture      = issueDynamicCommand (ECoSProtocol::get,
                                               id,
                                               ECoSProtocol::ARG_DURATION);

    auto name   = nameFuture.get ();
    auto state  = stateFuture.get ();
    auto sym    = symFuture.get ();
    auto mode   = modeFuture.get ();
    auto addr   = addrFuture.get ();
    auto dur    = durFuture.get ();


    return layout::Actuator
        {
        const_cast<MarklinCS1*> (this),
        *name.lines[0].arg->val,
        getActuatorIcon (sym),
        "PULSE" == mode.lines[0].arg->val ?
        layout::actuatorMode::PULSE :
        layout::actuatorMode::SWITCH,
        static_cast<uint> (atoi (addr.lines[0].arg->val->c_str ())),
        static_cast<uint> (atoi (dur.lines[0].arg->val->c_str ())),
        id,
        "1" == state.lines[0].arg->val
        };
    }

void MarklinCS1::setSpeed (size_t id, int8_t speed)
    {
    uint8_t speedAbs = static_cast<uint8_t> (std::abs (speed));

    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_SPEED, speedAbs));
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_DIR, speed < 0 ? 1 : 0));
    }

void MarklinCS1::setFunc (size_t id, uint8_t func, bool enable)
    {
    auto pair = ECoSProtocol::range{ func, static_cast<uint16_t> (enable ? 1 : 0) };

    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_FUNC, pair));
    }

void MarklinCS1::requestControl (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::request,
                         id,
                         ECoSProtocol::ARG_CONTROL,
                         ECoSProtocol::ARG_FORCE);
    }

void MarklinCS1::releaseControl (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::release,
                         id,
                         ECoSProtocol::ARG_CONTROL);
    }

void MarklinCS1::requestActuatorControl (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::request,
                         id,
                         ECoSProtocol::ARG_CONTROL,
                         ECoSProtocol::ARG_FORCE);
    }

void MarklinCS1::releaseActuatorControl (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::release,
                         id,
                         ECoSProtocol::ARG_CONTROL);
    }

void MarklinCS1::removeActuator (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::deleteId, id);
    }

void MarklinCS1::requestRouteControl (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::request,
                         id,
                         ECoSProtocol::ARG_CONTROL,
                         ECoSProtocol::ARG_FORCE);
    }

void MarklinCS1::releaseRouteControl (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::release,
                         id,
                         ECoSProtocol::ARG_CONTROL);
    }

std::vector<layout::funcInfo> MarklinCS1::getFunctions (size_t id) const
    {
    std::vector<layout::funcInfo>                       functions;

    std::array<std::future<ECoSProtocol::reply>, 32>    futures;
    std::array<ECoSProtocol::reply, 32>                 replies;

    for (int ii = 0; ii < 16; ++ii)
        {
        futures[ii] = issueDynamicCommand (ECoSProtocol::get,
                                           id,
                                           ARG (ECoSProtocol::ARG_FUNCSYMBOL, ii));
        }

    for (int ii = 0; ii < 16; ++ii)
        {
        futures[ii + 16] = issueDynamicCommand (ECoSProtocol::get,
                                                id,
                                                ARG (ECoSProtocol::ARG_FUNC, ii));
        }

    std::transform (futures.begin (),
                    futures.end (),
                    replies.begin (),
                    std::mem_fn (&std::future<ECoSProtocol::reply>::get));

    functions.reserve (16);

    for (int ii = 0; ii < 16; ++ii)
        {
        static std::map<int, layout::funcInfo::icon_t> iconMap =
            {
             { 2,  layout::funcInfo::ICON_FUNC_LIGHT_HEADLIGHT },
             { 9,  layout::funcInfo::ICON_FUNC_MISC_PANTOGRAPH },
             { 14, layout::funcInfo::ICON_FUNC_SOUND_OPERATING },
             { 7,  layout::funcInfo::ICON_FUNC_SOUND_HORN },
             { 1,  layout::funcInfo::ICON_FUNC_MISC_ABV },
             { 15, layout::funcInfo::ICON_FUNC_SOUND_BRAKES },
             { 16, layout::funcInfo::ICON_FUNC_LIGHT_CAB },
             { 11, layout::funcInfo::ICON_FUNC_MISC_SLOW },
             { 19, layout::funcInfo::ICON_FUNC_SOUND_COUPLING },
             { 13, layout::funcInfo::ICON_FUNC_SOUND_GENERIC },
            };
        const ECoSProtocol::reply& reply = replies[ii];

        layout::funcInfo::icon_t icon = layout::funcInfo::ICON_FUNC_NUMBER;

        auto [idx, iconStr] = utils::str::split (*reply.lines[0].arg->val, ", ");
        int   iconInt = atoi (std::string{ iconStr }.c_str ());

        if (iconInt > 0)
            {
            auto it = iconMap.find (iconInt);
            auto [_, state] = utils::str::split (*replies[ii + 16].lines[0].arg->val, ", ");

            if (iconMap.end () != it)
                {
                icon = it->second;
                }

            functions.emplace_back (layout::funcInfo
                                    { "",
                                     icon,
                                     static_cast<uint8_t>
                                            (atoi (std::string{idx}.c_str ())),
                                     "1" == state });
            }
        }

    return functions;
    }
void MarklinCS1::setActuator (size_t id, bool val)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_STATE, val));
    }

void MarklinCS1::setActuatorMode (size_t id, layout::actuatorMode mode)
    {
    std::string modeString;

    if (layout::actuatorMode::PULSE == mode)
        {
        modeString = "PULSE";
        }
    else
        {
        modeString = "SWITCH";
        }

    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_MODE, modeString));
    }

void MarklinCS1::setActuatorName (size_t id, const std::string& name)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_NAME1, "\"" + name + "\""));
    }

void MarklinCS1::setActuatorAddress (size_t id, uint address)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_ADDR,
                              std::to_string (address)));
    }

void MarklinCS1::setActuatorDuration (size_t id, uint duration)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_DURATION,
                              std::to_string (duration)));
    }

void MarklinCS1::setActuatorIcon (size_t id, layout::actuatorIcon icon)
    {
    auto it = std::find_if (ICON_TABLE.begin (),
                            ICON_TABLE.end (),
                            [icon] (const std::pair<int, layout::actuatorIcon>& pair) -> bool
                            { return pair.second == icon; });

    if (ICON_TABLE.end () != it)
        {
        issueDynamicCommand (ECoSProtocol::set,
                             id,
                             ARG (ECoSProtocol::ARG_SYMBOL,
                                  std::to_string (it->first)));
        }
    }

void MarklinCS1::setRoute (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_STATE, true));
    }

void MarklinCS1::removeRoute (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::deleteId, id);
    }

void MarklinCS1::setRouteName (size_t id, const std::string& name)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_NAME1, "\"" + name + "\""));
    }

void MarklinCS1::setRouteMembers (size_t id, const layout::routeList & members)
    {
    for (auto& [actuator, state] : members)
        {
        ECoSProtocol::dynamicId actId = static_cast<ECoSProtocol::dynamicId> (actuator.getId ());

        issueDynamicCommand (ECoSProtocol::link,
                             id,
                             ARG (ECoSProtocol::ARG_ID,     actId),
                             ARG (ECoSProtocol::ARG_STATE,  state));
        }
    }

}