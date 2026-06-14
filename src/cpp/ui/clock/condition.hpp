/**
 * @file        clock/condition.hpp
 * @brief       Base class, defines interface for a form for
 *              entering an automation condition
 * @author      Justin Scott
 * @date        2026-06-08
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/automation/condition.hpp>

#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Base class defining interface for a form widget for entering an automation
/// condition
///
///////////////////////////////////////////////////////////////////////////////
class ConditionForm : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ConditionForm (QWidget* parent) :
        QWidget (parent)
        { setContentsMargins (0, 0, 0, 0); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if all fields have been filled correctly
    ///
    /// @return     True if all fields in this form have been filled correctly
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create the condition object for what has been entered in this form
    ///
    /// @return     Condition object
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual std::unique_ptr<control::AutomationCondition> getCondition () const = 0;

    ///////////////////////////////////////////////////////////////////////////////
    /// Populate the fields of this form from an existing condition
    ///
    /// @param[in]  condition       Condition to populate from
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void setCondition (const control::AutomationCondition& condition) = 0;

signals:

    ///////////////////////////////////////////////////////////////////////////////
    /// Signals that one of the input to one of the fields has changed
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void inputChanged ();

private:
    };

} // namespace ui::clock