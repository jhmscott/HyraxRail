/**
 * @file        clock/weekly.cpp
 * @brief       Weekly alarm condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/clock/weekly.hpp>
#include <ui/common/utils.hpp>

namespace ui::clock
{

WeeklyAlarmConditionForm::WeeklyAlarmConditionForm (QWidget* parent) :
    ConditionForm (parent)
    {
    m_layout = new QFormLayout{ this };

    m_layout->addRow (new QLabel{ this }, m_time = new QTimeEdit{ this });
    m_layout->addRow (new QLabel{ this }, m_days = new common::DaysOfWeekSelector{ this });
    m_layout->setContentsMargins (0, 0, 0, 0);

    connect (m_days,
            &common::DaysOfWeekSelector::selectionChanged,
             this,
            &WeeklyAlarmConditionForm::inputChanged);

    connect (m_time,
            &QTimeEdit::timeChanged,
             this,
            &WeeklyAlarmConditionForm::inputChanged);

    setLabels ();
    setLayout (m_layout);
    }


std::unique_ptr<control::AutomationCondition> WeeklyAlarmConditionForm::getCondition () const
    {
    return std::make_unique<control::AutomationWeeklyAlarm> (m_time->time (),
                                                             m_days->getDays ());
    }


void WeeklyAlarmConditionForm::setCondition (const control::AutomationCondition& condition)
    {
    auto time = static_cast<const control::AutomationWeeklyAlarm*> (&condition);

    m_time->setTime (time->getTime ());
    m_days->setDays (time->getDays ());
    }


void WeeklyAlarmConditionForm::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_time, tr ("Time"));
    common::setFormRowText (*m_layout, *m_days, tr ("Days"));
    }

} // namespace ui::clock
