/**
 * @file        clock/monthly.cpp
 * @brief       Monthly alarm condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/clock/monthly.hpp>
#include <ui/common/utils.hpp>

namespace ui::clock
{
MonthlyAlarmConditionForm::MonthlyAlarmConditionForm (QWidget* parent) :
    ConditionForm (parent)
    {

    m_layout = new QFormLayout{ this };

    m_layout->addRow (new QLabel{ this }, m_time = new QTimeEdit{ this });
    m_layout->addRow (new QLabel{ this }, m_month = new QSpinBox{ this });
    m_layout->setContentsMargins (0, 0, 0, 0);

    m_month->setMinimum (1);
    m_month->setMinimum (31);

    connect (m_month,
            &QSpinBox::valueChanged,
             this,
            &MonthlyAlarmConditionForm::inputChanged);

    connect (m_time,
            &QTimeEdit::timeChanged,
             this,
            &MonthlyAlarmConditionForm::inputChanged);

    setLabels ();
    setLayout (m_layout);
    }


std::unique_ptr<control::AutomationCondition> MonthlyAlarmConditionForm::getCondition () const
    {
    return std::make_unique<control::AutomationMonthlyAlarm> (m_time->time (),
                                                              static_cast<uint8_t> (
                                                                  m_month->value ()));
    }


void MonthlyAlarmConditionForm::setCondition (const control::AutomationCondition& condition)
    {
    auto time = static_cast<const control::AutomationMonthlyAlarm*> (&condition);

    m_time->setTime (time->getTime ());
    m_month->setValue (time->getDayOftheMonth ());
    }


void MonthlyAlarmConditionForm::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_time, tr ("Time"));
    common::setFormRowText (*m_layout, *m_month, tr ("Day"));
    }

} // namespace ui::clock