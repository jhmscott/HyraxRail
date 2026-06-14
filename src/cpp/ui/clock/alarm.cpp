/**
 * @file        clock/alarm.cpp
 * @brief       Daily alarm condition form
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/clock/alarm.hpp>
#include <ui/common/utils.hpp>

namespace ui::clock
{

AlarmConditionForm::AlarmConditionForm (QWidget* parent) :
    ConditionForm (parent)
    {
    m_layout = new QFormLayout{ this };

    m_layout->addRow (new QLabel{ this }, m_time = new QTimeEdit{ this });
    m_layout->setContentsMargins (0, 0, 0, 0);

    setLabels ();
    setLayout (m_layout);
    }


void AlarmConditionForm::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_time, tr ("Time"));
    }

} // namespace ui::clock
