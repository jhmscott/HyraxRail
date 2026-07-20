/**
 * @file        MainActivity
 * @brief       Entry point class for android.
 *              Handles init and interface between java and native code.
 * @author      Justin Scott
 * @date        2026-07-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


package ca.justinlab.hyraxrail;

import android.os.Bundle;
import org.qtproject.qt.android.bindings.QtActivity;
import android.util.Log;

public class MainActivity extends QtActivity
    {

    @Override
    public void onCreate(Bundle savedInstanceState)
        {
        super.onCreate(savedInstanceState);
        }

    public int getAndroidBatteryStatus()
        {
        // TODO
        return 100;
        }
    }