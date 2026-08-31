/**
 * @file        clock/task.hpp
 * @brief       Task information widget
 * @author      Justin Scott
 * @date        2026-06-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/task.hpp>

#include <ui/lang.hpp>

#include <QLabel>
#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Widget to display information about a given automation task
///
///////////////////////////////////////////////////////////////////////////////
class TaskWidget : public QWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  task    Task to display information for
    /// @param[in]  parent  Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    TaskWidget (control::AutomationTask& task, QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the task this is displaying
    ///
    /// @return     Automation task
    ///
    ///////////////////////////////////////////////////////////////////////////////
    control::AutomationTask& getTask () { return m_task; }
private:
    control::AutomationTask&    m_task;     ///< Task we are displaying
    QLabel*                     m_label;    ///< Task text label

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the tooltip text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void updateTooltip ();

    UILANG_ON_CHANGE (QWidget, updateTooltip ())
    };

} // namespace ui::clock