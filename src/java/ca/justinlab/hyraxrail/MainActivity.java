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
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.window.OnBackInvokedDispatcher;

import org.qtproject.qt.android.UsedFromNativeCode;
import org.qtproject.qt.android.bindings.QtActivity;

import java.util.Locale;

///////////////////////////////////////////////////////////////////////////////
/// Main entry point class for Hyrax Rail android app
///
///////////////////////////////////////////////////////////////////////////////
public class MainActivity extends QtActivity
    {
    private Battery         m_battery;      ///< Battery notification client
    private Locale          m_locale;       ///< Current system locale
    private BackAnimation   m_backAnimation;///< Animation called with the back gesture

    // Vibration enum, see utils::os::vibrationEffect

    private static final int VIBRATE_TICK           = 0;    ///< @see utils::os::VIBRATE_TICK
    private static final int VIBRATE_CLICK          = 1;    ///< @see utils::os::VIBRATE_CLICK
    private static final int VIBRATE_LONG_CLICK     = 2;    ///< @see utils::os::VIBRATE_LONG_CLICK
    private static final int VIBRATE_DOUBLE_CLICK   = 3;    ///< @see utils::os::VIBRATE_DOUBLE_CLICK


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
        m_locale = Resources.getSystem().getConfiguration().getLocales().get(0);

        super.onCreate (savedInstanceState);
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

        synchronized (m_locale)
            {
            Locale newLocale = newConfig.getLocales().get(0);

            // Detect changes to the locale and native native code
            if (newLocale != m_locale)
                {
                m_locale = newLocale;
                handleLocaleChange();
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Register the battery notification handler
    ///
    /// @param nativeObjPtr     Pointer to the native object (BatteryImpl)
    ///                         receiving the notifications
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @UsedFromNativeCode
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
    @UsedFromNativeCode
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
    @UsedFromNativeCode
    public String getLocale ()
        {
        synchronized (m_locale)
            {
            return m_locale.getLanguage();
            }
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Trigger haptic feedback
    ///
    /// @param  vibrate     Vibration type
    ///////////////////////////////////////////////////////////////////////////////
    @UsedFromNativeCode
    public void hapticFeedback (int vibrate)
        {
        int effect = -1;

        switch (vibrate)
            {
            case VIBRATE_TICK:
                {
                effect = VibrationEffect.EFFECT_TICK;
                break;
                }
            case VIBRATE_CLICK:
                {
                effect = VibrationEffect.EFFECT_CLICK;
                break;
                }
            case VIBRATE_LONG_CLICK:
                {
                effect = VibrationEffect.EFFECT_HEAVY_CLICK;
                break;
                }
            case VIBRATE_DOUBLE_CLICK:
                {
                effect = VibrationEffect.EFFECT_DOUBLE_CLICK;
                break;
                }
            }

        Vibrator vibrator = getApplicationContext ().getSystemService (Vibrator.class);
        vibrator.vibrate (VibrationEffect.createPredefined (effect));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Register the main window
    ///
    /// @param objPtr   Main window object pointer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @UsedFromNativeCode
    void registerMainWindow (long objPtr)
        {
        getOnBackInvokedDispatcher().
                registerOnBackInvokedCallback (OnBackInvokedDispatcher.PRIORITY_OVERLAY,
                                               m_backAnimation = new BackAnimation (objPtr));
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// De-register the main window
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @UsedFromNativeCode
    void unregisterMainWindow ()
        {
        getOnBackInvokedDispatcher().unregisterOnBackInvokedCallback (m_backAnimation);
        m_backAnimation = null;
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Notifies the native code that a change in locale has occurred
    ///
    /// @see    Java_ca_justinlab_hyraxrail_MainActivity_handleLocaleChange()
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void handleLocaleChange ();
    }