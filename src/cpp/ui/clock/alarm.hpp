/**
 * @file        clock/alarm.hpp
 * @brief       Daily alarm condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/clock/condition.hpp>

#include <ui/lang.hpp>

#include <QDateTimeEdit>
#include <QFormLayout>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Daily alarm condition form
///
///////////////////////////////////////////////////////////////////////////////
class AlarmConditionForm : public ConditionForm
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit AlarmConditionForm (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if all fields have been filled correctly
    ///
    /// @return     True if all fields have been filled correctly
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override { return true; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a condition from the current fields
    ///
    /// @return     Automation alarm condition
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<control::AutomationCondition> getCondition () const override
        {  return std::make_unique<control::AutomationAlarm> (m_time->time ()); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the forms from an existing condition
    ///
    /// @param[in]  condition       Condition to set the fields from
    ///
    /// @remarks    condition must be an AutomationAlarm
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setCondition (const control::AutomationCondition& condition) override
        { m_time->setTime (static_cast<const control::AutomationAlarm*> (&condition)->getTime ()); }

private:
    QFormLayout*    m_layout;   ///< Widget form layout
    QTimeEdit*      m_time;     ///< Alarm time field

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the form labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (ConditionForm, setLabels ())
    };

} // namespace ui::clock
