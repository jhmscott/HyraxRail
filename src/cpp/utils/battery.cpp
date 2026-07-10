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

namespace utils
{

namespace // anonymous
{
#if defined (Q_OS_WIN) || defined (DOXYGEN)

///////////////////////////////////////////////////////////////////////////////
/// Windows battery implementation. Uses a message only window to recieve
/// battery notifications
///
/// @ingroup    PIMPL
///
///////////////////////////////////////////////////////////////////////////////
class BatteryImpl : protected os::win32::MessageOnlyWindow
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  battery     Battery instance to send notifications to
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit BatteryImpl (Battery* battery) :
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
    ~BatteryImpl ()
        {
        UnregisterPowerSettingNotification (m_handle);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the battery percentage
    ///
    /// @return     Battery precentage [0,100]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::optional<int> getBatteryPercent () const
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


#ifdef Q_OS_MAC
class BatteryImpl
    {
public:
    explicit BatteryImpl (Battery* battery)
        {}

    int getBatteryPercent () const { return 100; }
    };

#endif // Q_OS_MAC

} // namespace anonymous


std::optional<int> Battery::getBatteryPercent () const
    {
    return m_impl->getBatteryPercent ();
    }

Battery::Battery () :
    m_impl (new BatteryImpl{ this })
    {}

Battery::~Battery ()
    {
    delete m_impl;
    }

} // namespace utils