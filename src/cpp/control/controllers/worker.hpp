/**
 * @file        controller/worker.hpp
 * @brief       Network communication worker thread for
 *              model train control
 * @author      Justin Scott
 * @date        2026-01-11
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/protocols/base.hpp>

#include <utils/pinger.hpp>

#include <chrono>
#include <tuple>
#include <queue>
#include <thread>
#include <mutex>
#include <future>

template<class T>
struct memberFuncTraits
    {

    };

template<class Class, class RetT, class... Args>
struct memberFuncTraits<RetT (Class::*) (Args...)>
    {
    using class_t   = Class;
    using ret_t     = RetT;
    using args_t    = std::tuple<Args...>;
    };

template<class Class, class RetT, class... Args>
struct memberFuncTraits<RetT (Class::*) (Args...) const >
    {
    using class_t   = Class;
    using ret_t     = RetT;
    using args_t    = std::tuple<Args...>;
    };

namespace control
{

namespace workthread_internal
{
template<class T>
struct resolveImpl
    {
    static auto resolve (T&& val) { return val; }
    };

template<class T>
struct resolveImpl<std::future<T>>
    {
    static auto resolve (std::future<T>&& val) { return val.get (); }
    };


template<class T>
auto resolve (T&& val)
    {
    return resolveImpl<T>::resolve (std::forward<T> (val));
    }
}

class ConnectionWorkerThread
    {
public:
    enum healthLevel
        {
        // Network connection health
        HEALTH_DEAD,
        HEALTH_LOW,
        HEALTH_MEDIUM,
        HEALTH_HIGH,
        HEALTH_FULL,

        // Com port health
        HEALTH_CONNECTED,
        HEALTH_DISCONNECTED,

        // Shared, for when a controller is not configured
        HEALTH_UNAVAILABLE,

        NUM_HEALTH_VALUES
        };

    struct health
        {
        healthLevel                 level;
        std::chrono::milliseconds   ping;
        };

    ConnectionWorkerThread (std::string_view controllerName, std::unique_ptr<ProtocolBase>&& proto);

    ~ConnectionWorkerThread ();

    template<class Func, class... Args>
    auto enqueue (Func func, Args&&... args)
        {
        std::lock_guard     lk (m_mtx);
        auto                newTask = new ProtocolTask{ func, std::forward<Args> (args)... };

        m_taskQueue.emplace (newTask);
        m_cv.notify_all ();

        return newTask->getFuture ();
        }

    health getConnectionHealth () const { return m_health; }

    const control::ProtocolMetaClassBase& getProtocol () const { return m_proto->getMetaClass (); }

    utils::device::deviceInfo getDeviceInfo () const { return m_proto->getDeviceInfo (); }
private:
    class TaskBase
        {
    public:
        virtual ~TaskBase () {}

        virtual void callContained (ProtocolBase&) = 0;
        };

    template<class Func, class... PassedArgs >
    class ProtocolTask : public TaskBase
        {
    public:
        using Derived   = typename memberFuncTraits<Func>::class_t;
        using RetT      = typename memberFuncTraits<Func>::ret_t;

        ProtocolTask (Func func, PassedArgs&&... args) :
            m_args ({ std::forward<PassedArgs> (args)... }),
            m_func (func)
            {}

        virtual void callContained (ProtocolBase& proto)
            {
            Derived* derived = static_cast<Derived*> (&proto);

            auto caller = [&] (auto&&... args)
                {
                return (derived->*m_func) (workthread_internal::resolve (std::forward<PassedArgs> (args))...);
                };

            try
                {
                if constexpr (std::is_void_v<RetT>)
                    {
                    std::apply (caller, std::move (m_args));
                    m_promise.set_value ();
                    }
                else
                    {
                    m_promise.set_value (std::apply (caller, std::move (m_args)));
                    }
                }
            catch (std::exception& ex)
                {
                try
                    {
                    m_promise.set_exception (std::make_exception_ptr (ex));
                    }
                catch (...)
                    {}
                }
            }

        std::future<RetT> getFuture ()
            {
            return m_promise.get_future ();
            }
    private:
        std::tuple<PassedArgs...>   m_args;     ///< Arguments to call function with
        std::promise<RetT>          m_promise;
        Func                        m_func;     ///< Function to call

        };

    std::unique_ptr<ProtocolBase>           m_proto;            ///< Protocol being exchanged
    std::queue<std::unique_ptr<TaskBase>>   m_taskQueue;        ///< Queue of network tasks
    std::thread                             m_thread;           ///< Connection thread
    std::mutex                              m_mtx;              ///< Queue lock
    std::condition_variable                 m_cv;               ///< Enueue signal
    std::atomic_bool                        m_continue = true;  ///< Continue flag, set to false to signal shutdown

    utils::Pinger*                          m_pinger = NULL;
    health                                  m_health = { HEALTH_DEAD,  std::chrono::milliseconds{ 0 } };

    void loop ();
    };

}