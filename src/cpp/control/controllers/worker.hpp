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
#include <utils/traits.hpp>

#include <chrono>
#include <tuple>
#include <queue>
#include <thread>
#include <mutex>
#include <future>


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
    /// Enumerated health value
    /// Each value corresponds to an icon in the UI
    enum healthLevel
        {
        // Network connection health
        HEALTH_DEAD,        ///< No connection
                            ///  @image html cell-signal-x.svg width=32
        HEALTH_LOW,         ///< Poor connection
                            ///  @image html cell-signal-low.svg width=32
        HEALTH_MEDIUM,      ///< Average connection
                            ///  @image html cell-signal-medium.svg width=32
        HEALTH_HIGH,        ///< Good connection
                            ///  @image html cell-signal-high.svg width=32
        HEALTH_FULL,        ///< Great connection
                            ///  @image html cell-signal-full.svg width=32

        // Com port health
        HEALTH_CONNECTED,   ///< COM port connected
                            ///  @image html plugs-connected.svg width=32
        HEALTH_DISCONNECTED,///< COM port disconnected
                            ///  @image html unplug.svg width=32

        // Shared, for when a controller is not configured
        HEALTH_UNAVAILABLE, ///< Controller not configured
                            ///  @image html cell-signal-grey.svg width=32

        NUM_HEALTH_VALUES   ///< Delimiter only
        };

    /// Connection health information
    struct health
        {
        healthLevel                 level;  ///< Enumerated health value
        std::chrono::milliseconds   ping;   ///< Connection ping (only for socket connections)

        ///////////////////////////////////////////////////////////////////////////////
        /// Equality operator
        ///
        /// @param[in]  other       Health to compare to
        ///
        ///////////////////////////////////////////////////////////////////////////////
        bool operator== (const health& other) const
            {
            return other.level == level && other.ping == ping;
            }

        ///////////////////////////////////////////////////////////////////////////////
        /// Inequality operator
        ///
        /// @param[in]  other       Health to compare to
        ///
        ///////////////////////////////////////////////////////////////////////////////
        bool operator!= (const health& other) const { return !(*this == other); }
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
    health getConnectionHealth () const { std::lock_guard lk{ m_healthLock }; return m_health; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the thread has successfully connected to the host
    ///
    /// @return     True if connected
    ///
    ///////////////////////////////////////////////////////////////////////////////
    bool isConnected () const
        {
        std::lock_guard lk{ m_healthLock };

        return HEALTH_DISCONNECTED != m_health.level &&
                HEALTH_UNAVAILABLE != m_health.level &&
                       HEALTH_DEAD != m_health.level;
        }

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

    ///////////////////////////////////////////////////////////////////////////////
    /// Waits for the network queue to empty
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void waitForNetworkQueue () const;
private:
    ///////////////////////////////////////////////////////////////////////////////
    /// Polymorphic task base class
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

        ///////////////////////////////////////////////////////////////////////////////
        /// Get the time this task was added to the queue
        ///
        /// @return     creation time point
        ///
        ///////////////////////////////////////////////////////////////////////////////
        auto getCreationTime () const { return m_creation; }

    private:
        /// Task creation time
        std::chrono::system_clock::time_point m_creation = std::chrono::system_clock::now ();
        };


    ///////////////////////////////////////////////////////////////////////////////
    /// Derived templated task
    ///
    /// @tparam     Func        Task function type
    /// @tparam     PassedArgs  Argument types to pass to task function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    template<class Func, class... PassedArgs >
    class ProtocolTask : public TaskBase
        {
    public:
        using Derived   = typename utils::traits::memberFuncTraits<Func>::class_t; ///< Class derived type
        using RetT      = typename utils::traits::memberFuncTraits<Func>::ret_t;   ///< Function return type

        ///////////////////////////////////////////////////////////////////////////////
        /// Constructor. Create a task from a protocol's member function
        ///
        /// @param[in]  func    Protocol member function
        /// @param[in]  args    Arguments to pass to func
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
    mutable std::mutex                      m_mtx;              ///< Queue lock
    std::condition_variable                 m_cv;               ///< Enueue signal
    mutable std::mutex                      m_healthLock;       ///< Protects the connection health status
    std::condition_variable                 m_healthChange;     ///< Signals the health as changed
    std::atomic_bool                        m_continue = true;  ///< Continue flag, set to false to signal shutdown
    bool                                    m_suspended = false;///< If the app is suspended
    std::unique_ptr<utils::Pinger>          m_pinger = NULL;    ///< Connection pinger instance
    health                                  m_health =          ///< Connection health
                                                { HEALTH_DEAD,  std::chrono::milliseconds{ 0 } };
    QMetaObject::Connection                 m_stateConnection;  ///< Application state change signal
    mutable std::condition_variable         m_emptySignal;      ///< Signals the queue has been emptied

    ///////////////////////////////////////////////////////////////////////////////
    /// Worker thread loop function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void loop ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a change in the application state
    ///
    /// @param[in]  state       New state
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void applicationStateChanged (Qt::ApplicationState state);
    };

}
