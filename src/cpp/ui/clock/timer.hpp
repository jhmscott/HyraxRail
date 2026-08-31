/**
 * @file        clock/task.hpp
 * @brief       Timer condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/clock/condition.hpp>
#include <ui/common/duration.hpp>
#include <ui/lang.hpp>

#include <QFormLayout>


namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Timer condition form
///
///////////////////////////////////////////////////////////////////////////////
class TimerConditionForm : public ConditionForm
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit TimerConditionForm (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the form has acceptable input
    ///
    /// @return     True if all fields have acceptable input
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override { return m_duration->getDuration ().count () > 0; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a timer automation condition from the current state of the form
    /// fields
    ///
    /// @return     Timer condition
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<control::AutomationCondition> getCondition () const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the form fields from an automation condition
    ///
    /// @param[in]  condition       Automation condition
    ///
    /// @remarks    condition must be an AutomationTimer
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setCondition (const control::AutomationCondition& condition) override;

private:
    common::DurationField*  m_duration; ///< Timer durstion field
    QFormLayout*            m_layout;   ///< Widget form layout

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the form labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (ConditionForm, setLabels ())
    };

} // namespace ui::clock