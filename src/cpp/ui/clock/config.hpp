/**
 * @file        clock/config.hpp
 * @brief       Configuration form for the fast clock
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/automation/clock.hpp>

#include <ui/clock/clock.hpp>
#include <ui/common/tiereddropdown.hpp>
#include <ui/common/toggleswitch.hpp>
#include <ui/lang.hpp>

#include <QDateTimeEdit>
#include <QFormLayout>
#include <QSpinBox>
#include <QWidget>


namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Configuration form widget for the fast clock settings
///
///////////////////////////////////////////////////////////////////////////////
class ConfigForm : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ConfigForm (QWidget* parent);

signals:
    ///////////////////////////////////////////////////////////////////////////////
    /// Signals the user has changed the clock style
    ///
    /// @param[in]  style       Clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void styleChanged (clockStyle style);

private:
    QFormLayout*            m_layout;       ///< Form layout
    QDateEdit*              m_date;         ///< Date field
    QTimeEdit*              m_time;         ///< Time field
    QSpinBox*               m_ratio;        ///< Time ratio field
    QLayout*                m_ratioLayout;  ///< Layout containing ratio field
    common::TieredDropdown* m_style;        ///< Clock style selection
    common::ToggleSwitch*   m_running;      ///< Running toggle

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the form layouts
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (QWidget, setLabels ())

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the running state toggled
    ///
    /// @param[in]  running     true if switch has change to runnning
    ///                         false if switch has changed to stopped
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void runningToggled (bool running);

    ///////////////////////////////////////////////////////////////////////////////
    /// Called when the fast clock time changes
    ///
    /// @param[in]  time        New time
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void updateTime (const control::FastClock::time_point& time);

    ///////////////////////////////////////////////////////////////////////////////
    /// Called when the user changes the date or time value
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void dateTimeChanged ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Handles a change to the clock style combo box
    ///
    /// @param[in]  idx     Combo box index
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void styleIndexChanged (int idx);
    };

} // namespace ui::clock
