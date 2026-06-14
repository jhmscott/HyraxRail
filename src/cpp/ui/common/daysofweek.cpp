/**
 * @file        common/daysofweek.cpp
 * @brief       Set of checkboxes to select a set of days in a week
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/daysofweek.hpp>

#include <QBoxLayout>


namespace ui::common
{


DaysOfWeekSelector::DaysOfWeekSelector (QWidget* parent) :
    QWidget (parent)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

    for (size_t ii = 0; ii < std::size (m_days); ++ii)
        {
        QVBoxLayout* checkLayout = new QVBoxLayout;

        m_days[ii]  = new QCheckBox{ this };
        m_labels[ii]= new QLabel{ this };

        checkLayout->addWidget (m_days[ii]);
        checkLayout->addWidget (m_labels[ii]);

        connect (m_days[ii],
                &QCheckBox::checkStateChanged,
                 this,
                &DaysOfWeekSelector::selectionChanged);

        layout->addLayout (checkLayout);
        }

    setLabels ();
    setLayout (layout);
    }


void DaysOfWeekSelector::setDays (const utils::time::days& days)
    {
    for (size_t ii = 0; ii < std::size (m_days); ++ii)
        {
        m_days[ii]->setChecked (days[ii]);
        }
    }


utils::time::days DaysOfWeekSelector::getDays () const
    {
    utils::time::days days;

    for (size_t ii = 0; ii < std::size (m_days); ++ii)
        {
        days[ii] = m_days[ii]->isChecked ();
        }

    return days;
    }


void DaysOfWeekSelector::setLabels ()
    {
    for (size_t ii = 0; ii < std::size (m_labels); ++ii)
        {
        m_labels[ii]->setText (
            utils::time::dayOfWeekAbreviation (
                static_cast<utils::time::dayOfTheWeek> (ii)));
        }
    }


} // namespace ui::common