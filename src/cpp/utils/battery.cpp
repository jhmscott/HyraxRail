/**
 * @file        utils/battery.cpp
 * @brief       Device battery abstraction
 * @author      Justin Scott
 * @date        2026-07-02
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/battery.hpp>
#include <utils/log.hpp>
#include <utils/os.hpp>

#ifdef Q_OS_MACOS
#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#endif // Q_OS_MACOS

#ifdef Q_OS_ANDROID
#include <jni.h>

#include <QJniObject>
#endif // Q_OS_ANDROID



namespace utils
{

namespace // anonymous
{

///////////////////////////////////////////////////////////////////////////////
/// Battery implementation base class. Defines interface only, as actual
/// implementation is dependent on platform
///
/// @ingroup    PIMPL
///
/// @see        utils::Battery
///
///////////////////////////////////////////////////////////////////////////////
class BatteryImpl
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Virtual destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~BatteryImpl () {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the battery percentage
    ///
    /// @return     Battery percentage [0,100]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::optional<int> getBatteryPercent () const = 0;
    };

#if defined (Q_OS_WIN) || defined (DOXYGEN)

///////////////////////////////////////////////////////////////////////////////
/// Windows battery implementation. Uses a message only window to recieve
/// battery notifications
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class BatteryImplWindows :
    public BatteryImpl,
    protected os::win32::MessageOnlyWindow
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  battery     Battery instance to send notifications to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit BatteryImplWindows (Battery* battery) :
        m_battery (battery)
        {
        m_handle = RegisterPowerSettingNotification (getHandle (),
                                                    &GUID_BATTERY_PERCENTAGE_REMAINING,
                                                     DEVICE_NOTIFY_WINDOW_HANDLE);

        //  Get the initial state of the battery
        SYSTEM_POWER_STATUS status;

        if (FALSE == GetSystemPowerStatus (&status))
            {
            logWinWarning (GetSystemPowerStatus);
            }
        else
            {
            m_batteryPercentage = status.BatteryLifePercent;
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~BatteryImplWindows ()
        {
        UnregisterPowerSettingNotification (m_handle);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the battery percentage
    ///
    /// @return     Battery precentage [0,100]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::optional<int> getBatteryPercent () const override
        {
        std::optional<int> rc;

        if (BATTERY_PERCENTAGE_UNKNOWN != m_batteryPercentage)
            {
            rc = m_batteryPercentage;
            }

        return rc;
        }

private:
    Battery*        m_battery;                  ///< Battery instance to recieve notifications
    HPOWERNOTIFY    m_handle;                   ///< Notification handle
    DWORD           m_batteryPercentage =       ///< Battery percentage [0,100]
                            BATTERY_PERCENTAGE_UNKNOWN;

    ///////////////////////////////////////////////////////////////////////////////
    /// Wndproc to receive battery events
    ///
    /// @param[in]  hwnd        Window handle
    /// @param[in]  msg         Message type (WM_*)
    /// @param[in]  wParam      Wide param
    /// @param[in]  lParam      Long param
    ///
    /// @return     Message result
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual LRESULT message (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
        {
        LRESULT res = FALSE;

        switch (msg)
            {
            case WM_POWERBROADCAST:
                {
                switch (wParam)
                    {
                    case PBT_POWERSETTINGCHANGE:
                        {
                        const POWERBROADCAST_SETTING& setting =
                            *reinterpret_cast<POWERBROADCAST_SETTING*> (lParam);

                        if (TRUE == IsEqualGUID (setting.PowerSetting,
                                                 GUID_BATTERY_PERCENTAGE_REMAINING))
                            {
                            m_batteryPercentage = *reinterpret_cast<const DWORD*> (setting.Data);
                            emit m_battery->batteryPercentageChange (m_batteryPercentage);
                            }

                        res = TRUE;
                        break;
                        }
                    }
                break;
                }
            }

        return res;
        }
    };

#endif // defined (Q_OS_WIN) || defined (DOXYGEN)


#if defined (Q_OS_MACOS) || defined (DOXYGEN)


///////////////////////////////////////////////////////////////////////////////
/// Query the charge of the main system battery in percent
///
/// @return     Current battery percentage [0,100]
///
///////////////////////////////////////////////////////////////////////////////
static int getBatteryPercentage ()
    {
    int         percent     = -1;
    CFTypeRef   info        = IOPSCopyPowerSourcesInfo ();
    CFArrayRef  sources     = IOPSCopyPowerSourcesList (info);

    if (CFArrayGetCount (sources) > 0)
        {
        const void* value;
        int         maxCapacity;
        int         curCapacity;

        CFTypeRef       type = CFArrayGetValueAtIndex (sources, 0);
        CFDictionaryRef dict = static_cast<CFDictionaryRef> (IOPSGetPowerSourceDescription (info, type));


        value = CFDictionaryGetValue (dict, CFSTR (kIOPSCurrentCapacityKey));
        CFNumberGetValue (static_cast<CFNumberRef> (value),
                          kCFNumberSInt32Type,
                         &curCapacity);

        value = CFDictionaryGetValue (dict, CFSTR (kIOPSMaxCapacityKey));
        CFNumberGetValue (static_cast<CFNumberRef> (value),
                          kCFNumberSInt32Type,
                         &maxCapacity);

        percent = curCapacity * 100 / maxCapacity;
        }

    CFRelease (sources);
    CFRelease (info);

    return percent;
    }


///////////////////////////////////////////////////////////////////////////////
/// Apple battery implementation. Adds a run loop source to the main run loop
/// that recieves notifications about battery changes
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class BatteryImplApple : public BatteryImpl
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  battery     Battery interface instance to recieve notifications
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit BatteryImplApple (Battery* battery) :
        m_battery (battery)
        {
        CFRunLoopRef mainRunLoop = CFRunLoopGetMain();

        m_percent = getBatteryPercentage ();
        m_ref = IOPSNotificationCreateRunLoopSource (&BatteryImpl::batteryStateChanged,
                                                      this);

        CFRunLoopAddSource (mainRunLoop,
                            m_ref,
                            kCFRunLoopCommonModes);
        }


    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~BatteryImplApple ()
        {
        CFRunLoopSourceInvalidate (m_ref);
        CFRelease (m_ref);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the battery percentage
    ///
    /// @return     Battery precentage [0,100]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::optional<int> getBatteryPercent () const override
        {
        std::optional<int> rc;

        if (-1 != m_percent)
            {
            rc = m_percent;
            }

        return rc;
        }

private:
    Battery*            m_battery;  ///< Battery instance to recieve notifications
    CFRunLoopSourceRef  m_ref;      ///< Reference to the run source that is registered with the main loop
    int                 m_percent;  ///< Battery percentage [0,100]

    ///////////////////////////////////////////////////////////////////////////////
    /// Callback registered to recieve power notifications
    ///
    /// @param[in]  ud  User data passed in IOPSNotificationCreateRunLoopSource()
    ///                 Pointer to the BatteryImpl instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    static void batteryStateChanged (void* ud)
        {
        BatteryImpl& self = *static_cast<BatteryImpl*> (ud);

        self.m_percent = getBatteryPercentage ();
        emit self.m_battery->batteryPercentageChange (self.m_percent);
        }

    };

#endif // defined (Q_OS_MACOS) || defined (DOXYGEN)

#if defined (Q_OS_ANDROID)  || defined (DOXYGEN)


///////////////////////////////////////////////////////////////////////////////
/// Android battery implementation to recieve battery notification
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class BatteryImplAndroid : public BatteryImpl
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  battery     Battery interface object
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit BatteryImplAndroid (Battery* battery) :
        m_battery (battery),
        m_mainActivity (QNativeInterface::QAndroidApplication::context ())
        {
        m_mainActivity.callMethod<void> ("registerBatteryHandler",
                                         reinterpret_cast<jlong> (this));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Destructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ~BatteryImplAndroid ()
        {
        m_mainActivity.callMethod<void> ("unregisterBatteryHandler");
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the battery percentage
    ///
    /// @return     Battery percentage [0,100]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::optional<int> getBatteryPercent () const override { return m_percent; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the battery percentage
    ///
    /// @param[in]  pct     Battery percentage
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setBatteryPercent (int pct)
        {
        m_percent = pct;

        emit m_battery->batteryPercentageChange (pct);
        }
private:
    Battery*            m_battery;      ///< Battery instance to recieve notifications
    QJniObject          m_mainActivity; ///< Main application activity
    std::optional<int>  m_percent;      ///< Battery percentage [0,100]
    };
#endif // defined (Q_OS_ANDROID)  || defined (DOXYGEN)

// Alias the native type
#if defined (Q_OS_WINDOWS) || defined (DOXYGEN)
using BatteryImplNative = BatteryImplWindows;
#elif defined (Q_OS_MACOS)
using BatteryImplNative = BatteryImplApple;
#elif defined (Q_OS_ANDROID)
using BatteryImplNative = BatteryImplAndroid;
#endif


} // namespace anonymous


std::optional<int> Battery::getBatteryPercent () const
    {
    return m_impl->getBatteryPercent ();
    }

Battery::Battery () :
    m_impl (new BatteryImplNative{ this })
    {}

Battery::~Battery ()
    {
    delete m_impl;
    }

} // namespace utils


#if defined (Q_OS_ANDROID) || defined (DOXYGEN)
extern "C"
{

///////////////////////////////////////////////////////////////////////////////
/// Java "Native" function called by the java battery class when recieving a
/// a battery notifification
///
/// @param[in]  env         (unused) JNI environment
/// @param[in]  thisObj     (unused) Battery java object
/// @param[in]  instancePtr Pointer to the BatteryImpl clas
/// @param[in]  pct         Battery percentage [0,100]
///
/// @see        ca.justinlab.hyraxrail.Battery.batteryStatusChanged()
///
/// @ingroup    JNI_FUNC
///
///////////////////////////////////////////////////////////////////////////////
JNIEXPORT
void JNICALL Java_ca_justinlab_hyraxrail_Battery_batteryStatusChanged (JNIEnv*  env,
                                                                       jobject  thisObj,
                                                                       jlong    instancePtr,
                                                                       jint     pct)
    {
    utils::BatteryImplAndroid& battery = *reinterpret_cast<utils::BatteryImplAndroid*> (instancePtr);

    battery.setBatteryPercent (pct);
    }
} // extern "C"

#endif // defined (Q_OS_ANDROID) || defined (DOXYGEN)