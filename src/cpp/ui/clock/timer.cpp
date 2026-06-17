/**
 * @file        clock/task.cpp
 * @brief       Timer condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/clock/timer.hpp>
#include <ui/common/utils.hpp>

#include <utils/algorithm.hpp>

namespace ui::clock
{

TimerConditionForm::TimerConditionForm (QWidget* parent) :
    ConditionForm (parent)
    {
    utils::time::units units;

    utils::algorithm::makeBitset (units,
                                  utils::time::SECONDS,
                                  utils::time::MINUTES,
                                  utils::time::HOURS);

    m_layout = new QFormLayout{ this };

    m_layout->addRow (new QLabel{ this }, m_duration = new common::DurationField{ units, this });
    m_layout->setContentsMargins (0, 0, 0, 0);

    connect (m_duration,
            &common::DurationField::inputChanged,
             this,
            &ConditionForm::inputChanged);

    setLabels ();
    setLayout (m_layout);
    }


std::unique_ptr<control::AutomationCondition> TimerConditionForm::getCondition () const
    {
    return std::make_unique<control::AutomationTimer> (m_duration->getDuration ());
    }


void TimerConditionForm::setCondition (const control::AutomationCondition& condition)
    {
    const control::AutomationTimer* timer = static_cast<const control::AutomationTimer*> (&condition);

    m_duration->setDuration (timer->getDuration (), true);
    }


void TimerConditionForm::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_duration, tr ("Timer"));
    }

} // namespace ui::clock