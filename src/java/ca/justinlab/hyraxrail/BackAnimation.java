/**
 * @file        BackAnimation.java
 * @brief       Implements a OnBackAnimationCallback interface,
 *              passing back animation progress to the main Qt window
 * @author      Justin Scott
 * @date        2026-07-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

package ca.justinlab.hyraxrail;

import android.window.BackEvent;
import android.window.OnBackAnimationCallback;

import androidx.annotation.FloatRange;
import androidx.annotation.NonNull;


///////////////////////////////////////////////////////////////////////////////
/// App back animation, passes animation to main Qt window
///
///////////////////////////////////////////////////////////////////////////////
public class BackAnimation implements OnBackAnimationCallback
    {
    long m_windowPtr;   ///< Main window pointer

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param mainWindowPtr    Pointer to the main window object
    ///
    ///////////////////////////////////////////////////////////////////////////////
    BackAnimation (long mainWindowPtr) { m_windowPtr = mainWindowPtr; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Back has been cancelled
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onBackCancelled () {  backCancelled (m_windowPtr); }


    ///////////////////////////////////////////////////////////////////////////////
    /// Back animation has progressed
    ///
    /// @param backEvent    Back event progression
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onBackProgressed (@NonNull BackEvent backEvent)
        {
        backProgressed(m_windowPtr, backEvent.getProgress ());
        }

    ///////////////////////////////////////////////////////////////////////////////
    /// Back animation has started
    ///
    /// @param backEvent    Back event information
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onBackStarted (@NonNull BackEvent backEvent) { backStarted(m_windowPtr); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Back animation has completed, and the app should perform the back operation
    ///
    ///////////////////////////////////////////////////////////////////////////////
    @Override
    public void onBackInvoked () { backPressed (m_windowPtr); }


    ///////////////////////////////////////////////////////////////////////////////
    /// Notify the main Qt window that the back animation has progressed
    ///
    /// @param  mainWindowPtr   Main window to notify
    /// @param  progress        Animation progress
    ///
    /// @see    Java_ca_justinlab_hyraxrail_BackAnimation_backProgressed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void backProgressed (long mainWindowPtr, @FloatRange(from = 0.0, to = 1.0) float progress);

    ///////////////////////////////////////////////////////////////////////////////
    /// Notify the main Qt window that the back animation has started
    ///
    /// @param  mainWindowPtr   Main window to notify
    ///
    /// @see    Java_ca_justinlab_hyraxrail_BackAnimation_backStarted
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void backStarted (long mainWindowPtr);

    ///////////////////////////////////////////////////////////////////////////////
    /// Notify the main Qt window that the back operation has been cancelled
    ///
    /// @param  mainWindowPtr   Main window to notify
    ///
    /// @see    Java_ca_justinlab_hyraxrail_BackAnimation_backCancelled
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void backCancelled (long mainWindowPtr);

    ///////////////////////////////////////////////////////////////////////////////
    /// Notify the main Qt window that the back operation has been completed, and
    /// the back operation should be taken
    ///
    /// @param  mainWindowPtr   Main window to notify
    ///
    /// @see    Java_ca_justinlab_hyraxrail_BackAnimation_backPressed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    private native void backPressed (long mainWindowPtr);

    }
