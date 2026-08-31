/**
 * @file        clock/weekly.hpp
 * @brief       Weekly alarm condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/clock/condition.hpp>
#include <ui/common/daysofweek.hpp>

#include <QDateTimeEdit>
#include <QFormLayout>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Weekly alarm condition form
///
///////////////////////////////////////////////////////////////////////////////
class WeeklyAlarmConditionForm : public ConditionForm
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit WeeklyAlarmConditionForm (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if all the form fields have been filled correctly
    ///
    /// @return     True if all fields have acceptable input
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override { return m_days->getDays ().any (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a condition from the values in the form field
    ///
    /// @return     Weekly alarm condition
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<control::AutomationCondition> getCondition () const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the fields from an existing condition
    ///
    /// @param[in]  condition       Automation condition
    ///
    /// @remarks    condition must be an AutomationWeeklyAlarm
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setCondition (const control::AutomationCondition& condition) override;

private:
    QFormLayout*                m_layout;   ///< Widget form layout
    QTimeEdit*                  m_time;     ///< Alarm time field
    common::DaysOfWeekSelector* m_days;     ///< Which days the alarm should run on

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the form labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (ConditionForm, setLabels ())
    };

} // namespace ui::clock
