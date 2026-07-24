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
import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Bundle;

import org.qtproject.qt.android.bindings.QtActivity;

import java.util.Locale;

///////////////////////////////////////////////////////////////////////////////
/// Main entry point class for Hyrax Rail android app
///
///////////////////////////////////////////////////////////////////////////////
public class MainActivity extends QtActivity
    {
    private Battery m_battery;  ///< Battery notification client
    private Locale  m_locale;   ///< Current system locale

    ///////////////////////////////////////////////////////////////////////////////
    /// Called on application creation
    ///
    /// @param  savedInstanceState  Instance state from last session
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onCreate (Bundle savedInstanceState)
        {
        // save the initial locale
        m_locale = Resources.getSystem ().getConfiguration ().getLocales ().get (0);

        super.onCreate(savedInstanceState);
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a configuration change
    ///
    /// @param  newConfig       New configuration
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onConfigurationChanged (Configuration newConfig)
        {
        super.onConfigurationChanged (newConfig);

        Locale newLocale = newConfig.getLocales ().get (0);

        // Detect changes to the locale and native native code
        if (newLocale != m_locale)
            {
            m_locale = newLocale;
            handleLocaleChange ();
            }
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

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the system locale string. Used by native code to get locale after
    /// startup
    ///
    /// @return Locale string
    ///
    ///////////////////////////////////////////////////////////////////////////////
    public String getLocale ()
        {
        return m_locale.getLanguage();
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Notifies the native code that a change in locale has occurred
    ///
    /// @see    Java_ca_justinlab_hyraxrail_MainActivity_handleLocaleChange()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void handleLocaleChange ();
    }