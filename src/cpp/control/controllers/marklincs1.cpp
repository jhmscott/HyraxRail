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
                                                               static_cast<ECoSProtocol::dynamicId> (id),    \
                                                               __VA_ARGS__))

#define issueStaticCommand(func, id, ...)   (m_thread.enqueue (&func<id __VA_OPT__ (,) \
                                                              GET_ARG_FOR_TYPES (__VA_ARGS__)> __VA_OPT__(,) \
                                                               __VA_ARGS__))

#define ARG(type, val) (ECoSProtocol::arg{ (type), (val) })


using namespace std::chrono_literals;

namespace control
{
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
                            return layout::Locomotive{ const_cast<MarklinCS1*> (this),
                                                       line.arg->val.value (),
                                                       line.id, };
                            });
            }
        }

    return locos;
    }

std::vector<layout::Actuator> MarklinCS1::getActuators () const
    {
    return getSwitchingItems<layout::Actuator> ();
    }

std::vector<layout::Route> MarklinCS1::getRoutes () const
    {
    return getSwitchingItems<layout::Route> ();
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

void MarklinCS1::setRoute (size_t id)
    {
    issueDynamicCommand (ECoSProtocol::set,
                         id,
                         ARG (ECoSProtocol::ARG_STATE, true));
    }

template<class T>
std::vector<T> MarklinCS1::getSwitchingItems () const
    {
    std::vector<T> actuators;

    auto future = issueStaticCommand (ECoSProtocol::queryObjects,
                                      ECoSProtocol::ID_SWITCHING_ITEMS);


    auto reply = future.get ();
    auto it = std::remove_if (reply.lines.begin (),
                              reply.lines.end (),
                              [] (const ECoSProtocol::replyLine& line) -> bool
                              {
                              if constexpr (std::is_same_v<T, layout::Actuator>)
                                  {
                                  return line.id >= 30000;
                                  }
                              else
                                  {
                                  return line.id < 30000;
                                  }
                              });

    reply.lines.erase (it, reply.lines.end ());

    std::vector<std::future<ECoSProtocol::reply>>   nameReplies;
    std::vector<ECoSProtocol::reply>                names;

    nameReplies.reserve (reply.lines.size ());
    names.reserve (reply.lines.size ());

    std::transform (reply.lines.begin (),
                    reply.lines.end (),
                    std::back_inserter (nameReplies),
                    [this] (const ECoSProtocol::replyLine& line)
                    {
                    return issueDynamicCommand (ECoSProtocol::get,
                                                line.id,
                                                ECoSProtocol::ARG_NAME1);
                    });


    std::vector<std::future<ECoSProtocol::reply>>   stateReplies;
    std::vector<ECoSProtocol::reply>                states;

    stateReplies.reserve (reply.lines.size ());
    states.reserve (reply.lines.size ());

    std::transform (reply.lines.begin (),
                    reply.lines.end (),
                    std::back_inserter (stateReplies),
                    [this] (const ECoSProtocol::replyLine& line)
                    {
                    return issueDynamicCommand (ECoSProtocol::get,
                                                line.id,
                                                ECoSProtocol::ARG_STATE);
                    });

    std::vector<std::future<ECoSProtocol::reply>>   iconReplies;
    std::vector<ECoSProtocol::reply>                icons;

    if constexpr (std::is_same_v<T, layout::Actuator>)
        {
        iconReplies.reserve (reply.lines.size ());
        icons.reserve (reply.lines.size ());

        std::transform (reply.lines.begin (),
                        reply.lines.end (),
                        std::back_inserter (iconReplies),
                        [this] (const ECoSProtocol::replyLine& line)
                        {
                        return issueDynamicCommand (ECoSProtocol::get,
                                                    line.id,
                                                    ECoSProtocol::ARG_SYMBOL);
                        });
        }

    std::transform (nameReplies.begin (),
                    nameReplies.end (),
                    std::back_inserter (names),
                    std::mem_fn (&std::future<ECoSProtocol::reply>::get));

    std::transform (stateReplies.begin (),
                    stateReplies.end (),
                    std::back_inserter (states),
                    std::mem_fn (&std::future<ECoSProtocol::reply>::get));


    if constexpr (std::is_same_v<T, layout::Actuator>)
        {
        std::transform (iconReplies.begin (),
                        iconReplies.end (),
                        std::back_inserter (icons),
                        std::mem_fn (&std::future<ECoSProtocol::reply>::get));
        }

    actuators.reserve (reply.lines.size ());

    static const std::map<int, layout::actuatorIcon> iconTable =
        {
            { 23, layout::ICON_TURNOUT_LEFT  },
            { 24, layout::ICON_TURNOUT_RIGHT }
        };

    for (size_t ii = 0; ii < reply.lines.size (); ++ii)
        {
        if constexpr (std::is_same_v<T, layout::Actuator>)
            {
            layout::actuatorIcon    icon = layout::NO_ICON;
            auto                    it = iconTable.find (atoi (icons[ii].lines[0].arg->val->c_str ()));

            if (iconTable.end () != it)
                {
                icon = it->second;
                }

            actuators.emplace_back (const_cast<MarklinCS1*> (this),
                                    *names[ii].lines[0].arg->val,
                                    icon,
                                    static_cast<size_t> (reply.lines[ii].id),
                                    "1" == states[ii].lines[0].arg->val);
            }
        else
            {
            actuators.emplace_back (const_cast<MarklinCS1*> (this),
                                    *names[ii].lines[0].arg->val,
                                    static_cast<size_t> (reply.lines[ii].id));
            }
        }

    return actuators;
    }


}