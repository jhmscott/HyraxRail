/**
 * @file        Battery.java
 * @brief       Battery notification handler.
 *              Provides Java implementation of thr BatteryImpl class in battery.cpp
 * @author      Justin Scott
 * @date        2026-07-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


package ca.justinlab.hyraxrail;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.BatteryManager;

///////////////////////////////////////////////////////////////////////////////
/// Battery notification receiver
///
///////////////////////////////////////////////////////////////////////////////
public class Battery extends BroadcastReceiver
    {
    private final long m_nativeObjPtr;  ///< Pointer to the BatteryImpl instance

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param  nativeObjPtr    Pointer to the BatteryImpl class instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    public Battery (long nativeObjPtr)
        {
        m_nativeObjPtr = nativeObjPtr;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Receives and handles a battery notification
    ///
    /// @param context      Notification context
    /// @param intent       Notification intent (contains event data)
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onReceive (Context context, Intent intent)
        {
        if (Intent.ACTION_BATTERY_CHANGED.equals (intent.getAction ()))
            {
            // Get current battery level and total scale
            int level = intent.getIntExtra (BatteryManager.EXTRA_LEVEL, -1);
            int scale = intent.getIntExtra (BatteryManager.EXTRA_SCALE, -1);

            float batteryPct = (level / (float) scale) * 100;

            batteryStatusChanged (m_nativeObjPtr, (int) batteryPct);
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Notify native code the battery status has changed. Implemented by
    /// Java_ca_justinlab_hyraxrail_Battery_batteryStatusChanged in battery.cpp
    ///
    /// @param nativeObjPtr     Pointer to the BatteryImpl class instance
    /// @param pct              Battery charge in percentage [0,100]
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void batteryStatusChanged (long nativeObjPtr, int pct);
    }
