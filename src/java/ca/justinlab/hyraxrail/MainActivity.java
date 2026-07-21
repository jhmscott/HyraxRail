/**
 * @file        MainActivity.java
 * @brief       Entry point class for android.
 *              Handles init and interface between java and native code.
 * @author      Justin Scott
 * @date        2026-07-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


package ca.justinlab.hyraxrail;

import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;

import org.qtproject.qt.android.bindings.QtActivity;

///////////////////////////////////////////////////////////////////////////////
/// Main entry point class for Hyrax Rail android app
///
///////////////////////////////////////////////////////////////////////////////
public class MainActivity extends QtActivity
    {
    private Battery m_battery;  ///< Battery notification client

    ///////////////////////////////////////////////////////////////////////////////
    /// Called on application creation
    ///
    /// @param  savedInstanceState  Instance state from last session
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onCreate (Bundle savedInstanceState)
        {
        super.onCreate(savedInstanceState);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Register the battery notification handler
    ///
    /// @param nativeObjPtr     Pointer to the native object (BatteryImpl)
    ///                         receiving the notifications
    ///
    ///////////////////////////////////////////////////////////////////////////////
    public void registerBatteryHandler (long nativeObjPtr)
        {
        m_battery = new Battery (nativeObjPtr);

        IntentFilter filter = new IntentFilter (Intent.ACTION_BATTERY_CHANGED);
        registerReceiver (m_battery, filter);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// De-register the battery notification handle
    ///
    ///////////////////////////////////////////////////////////////////////////////
    public void unregisterBatteryHandler ()
        {
        unregisterReceiver (m_battery);
        }
    }