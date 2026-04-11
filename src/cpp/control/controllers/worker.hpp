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

// Unspecialized version
template<class T>
struct memberFuncTraits
    {

    };


///////////////////////////////////////////////////////////////////////////////
/// Type trait to get information about a member functiojn
///
/// @tparam     Class           Class the function is a member of
/// @tparam     RetT             Function return type
/// @tparam     Args...      Function arguments
///
///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
/// Implementation for the resolve function
///
/// @tparam     Type of value to resolve
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
struct resolveImpl
    {
    ///////////////////////////////////////////////////////////////////////////////
    /// Return the value passed
    ///
    /// @param[in]  val     Value
    ///
    /// @return     val
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static auto resolve (T&& val) { return val; }
    };

///////////////////////////////////////////////////////////////////////////////
/// Implementation for the resolve function for std::future
///
/// @tparam     Type future resolves to
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
struct resolveImpl<std::future<T>>
    {
    ///////////////////////////////////////////////////////////////////////////////
    ///  Return the value contained in the future
    ///
    ///  @param[in] val     Future to resolve
    ///
    ///  @return    Value from future
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static auto resolve (std::future<T>&& val) { return val.get (); }
    };

///////////////////////////////////////////////////////////////////////////////
/// Resolve a value. For most types this just gets passed through. But for futures, it resolves the value.
/// This allows you to pass the results of a previous task to the next task
///
/// @tparam     T       Type of value to resolve
///
/// @param[in]  val     Value to resolve
///
/// @return     Resolved value
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
auto resolve (T&& val)
    {
    return resolveImpl<T>::resolve (std::forward<T> (val));
    }

} // namespace workthread_internal


///////////////////////////////////////////////////////////////////////////////
/// Represents the thread for communicating with the controller
///
///////////////////////////////////////////////////////////////////////////////
class ConnectionWorkerThread
    {
public:
    // Enumerated health value
    // Each value corresponds to an icon in the UI
    enum healthLevel
        {
        // Network connection health
        HEALTH_DEAD,        ///< No connection
        HEALTH_LOW,         ///< Poor connection
        HEALTH_MEDIUM,      ///< Average connection
        HEALTH_HIGH,        ///< Good connection
        HEALTH_FULL,        ///< Great connection

        // Com port health
        HEALTH_CONNECTED,   ///< COM port connected
        HEALTH_DISCONNECTED,///< COM port disconnected

        // Shared, for when a controller is not configured
        HEALTH_UNAVAILABLE, ///< Controller not configured

        NUM_HEALTH_VALUES   ///< Delimiter only
        };

    // Connection health information
    struct health
        {
        healthLevel                 level;  ///< Enumerated health value
        std::chrono::milliseconds   ping;   ///< Connection ping (only for socket connections)
        };
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllerName      Controller instance name
    /// @param[in]  proto                          Connection protocol. Takes ownership
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ConnectionWorkerThread (std::string_view controllerName, std::unique_ptr<ProtocolBase>&& proto);
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor. Closes the connection
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~ConnectionWorkerThread ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Add a task to the worker thread queue
    ///
    /// @tparam     Func        Function type
    /// @tparam     Args... Function argument types
    ///
    /// @param[in]  func        Task function. Must be a member function of the protocol class
    /// @param[in]  args        Arguments to pass to function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Func, class... Args>
    auto enqueue (Func func, Args&&... args)
        {
        std::lock_guard     lk (m_mtx);
        auto                newTask = new ProtocolTask{ func, std::forward<Args> (args)... };

        m_taskQueue.emplace (newTask);
        m_cv.notify_all ();

        return newTask->getFuture ();
        }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the health of the connection
    ///
    /// @return     Connection health
    ///
    ///////////////////////////////////////////////////////////////////////////////
    health getConnectionHealth () const { return m_health; }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the protocol type's meta class
    ///
    /// @return     Protocol meta class instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const control::ProtocolMetaClassBase& getProtocol () const { return m_proto->getMetaClass (); }
        
    ///////////////////////////////////////////////////////////////////////////////
    /// Get the connection device information
    ///
    /// @return     Device info
    ///
    ///////////////////////////////////////////////////////////////////////////////
    utils::device::deviceInfo getDeviceInfo () const { return m_proto->getDeviceInfo (); }
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Polymporphic task base class
    ///
    ///////////////////////////////////////////////////////////////////////////////
    class TaskBase
        {
    public:
        ///////////////////////////////////////////////////////////////////////////////
        /// Virtual destructor
        ///
        ///////////////////////////////////////////////////////////////////////////////
        virtual ~TaskBase () {}
            
        ///////////////////////////////////////////////////////////////////////////////
        /// Call the task functions
        ///
        /// @param[in]  protocol        Protocol to call task for
        ///
        ///////////////////////////////////////////////////////////////////////////////
        virtual void callContained (ProtocolBase& protocol) = 0;
        };

        
    ///////////////////////////////////////////////////////////////////////////////
    /// Derived templated task
    ///
    /// @tparam     Func                        Task function type
    /// @tparam     PassedArgs...    Argument types to pass to task function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Func, class... PassedArgs >
    class ProtocolTask : public TaskBase
        {
    public:
        using Derived   = typename memberFuncTraits<Func>::class_t;
        using RetT      = typename memberFuncTraits<Func>::ret_t;

        ///////////////////////////////////////////////////////////////////////////////
        /// Constructor. Create a task from a protocol's member function
        ///
        /// @param[in]  func            Protocol member function
        /// @param[in]  args...     Arguments to pass to func
        ///
        ///////////////////////////////////////////////////////////////////////////////
        ProtocolTask (Func func, PassedArgs&&... args) :
            m_args ({ std::forward<PassedArgs> (args)... }),
            m_func (func)
            {}
            
        ///////////////////////////////////////////////////////////////////////////////
        /// Execute the contained task
        ///
        /// @param[in]  proto           Protocol instance
        ///
        ///////////////////////////////////////////////////////////////////////////////
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
        
        ///////////////////////////////////////////////////////////////////////////////
        /// Get the future for this task. Undefined to call this more than once
        ///
        /// @return     Task future
        ///
        ///////////////////////////////////////////////////////////////////////////////
        std::future<RetT> getFuture ()
            {
            return m_promise.get_future ();
            }
    private:
        std::tuple<PassedArgs...>   m_args;     ///< Arguments to call function with
        std::promise<RetT>          m_promise;  ///< Promise fulfilled by this task
        Func                        m_func;     ///< Function to call

        };

    std::unique_ptr<ProtocolBase>           m_proto;            ///< Protocol being exchanged
    std::queue<std::unique_ptr<TaskBase>>   m_taskQueue;        ///< Queue of network tasks
    std::thread                             m_thread;           ///< Connection thread
    std::mutex                              m_mtx;              ///< Queue lock
    std::condition_variable                 m_cv;               ///< Enueue signal
    std::atomic_bool                        m_continue = true;  ///< Continue flag, set to false to signal shutdown

    utils::Pinger*                          m_pinger = NULL;    ///< Connection pinger instance
    health                                  m_health = { HEALTH_DEAD,  std::chrono::milliseconds{ 0 } };

    ///////////////////////////////////////////////////////////////////////////////
    /// Worker thread loop function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void loop ();
    };

}
