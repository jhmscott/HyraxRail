/**
 * @file        clock/clockpanel.hpp
 * @brief       Panel for the fast clock/automation configuration tab
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/manager.hpp>

#include <ui/clock/clock.hpp>
#include <ui/lang.hpp>

#include <QBoxLayout>
#include <QGroupBox>
#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Main widget for the fast clock/automation configuration tab
///
///////////////////////////////////////////////////////////////////////////////
class ClockPanel : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers List of controllers
    /// @param[in]  automations List of automations
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ClockPanel (control::ControllerManager&    controllers,
                         control::AutomationManager&    automations,
                         QWidget*                       parent);

private:
    QGroupBox*   m_clockBox;    ///< Group box containing the fast clock and it's settings
    QHBoxLayout* m_clockLayout; ///< Layout containing fast clock

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the group box titles
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTitles ();

    UILANG_ON_CHANGE (QWidget, setTitles ());

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handles the user changing the clock style
    ///
    /// @param[in]  style       Clock style
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void styleChanged (clockStyle style);
    };

} // namespace ui::clock
