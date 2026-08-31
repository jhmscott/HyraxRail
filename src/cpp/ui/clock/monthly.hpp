/**
 * @file        clock/monthly.hpp
 * @brief       Monthly alarm condition form
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
#include <QSpinBox>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// MOnthly alarm condition form
///
///////////////////////////////////////////////////////////////////////////////
class MonthlyAlarmConditionForm : public ConditionForm
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit MonthlyAlarmConditionForm (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the form has been filled correctly
    ///
    /// @return     True if all fields have acceptable input
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override { return m_month->hasAcceptableInput (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a monthly alarm condition from the form fields
    ///
    /// @return     Monthly alarm condition
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<control::AutomationCondition> getCondition () const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the fields from a condition
    ///
    /// @param[in]  condition       Condition to populate fields from
    ///
    /// @remarks    condition should be an AutomationMonthlyAlarm
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setCondition (const control::AutomationCondition& condition) override;

private:
    QFormLayout*    m_layout;   ///< Widget form layout
    QTimeEdit*      m_time;     ///< Alarm time widget
    QSpinBox*       m_month;    ///< Day of the month field

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the form labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (ConditionForm, setLabels ())
    };
}