/**
 * @file        clock/task.cpp
 * @brief       Task information widget
 * @author      Justin Scott
 * @date        2026-06-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/clock/task.hpp>

#include <QBoxLayout>

namespace ui::clock
{
TaskWidget::TaskWidget (control::AutomationTask& task, QWidget* parent) :
    m_task (task)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

    m_label = new QLabel{ this };

    m_label->setText (task.name);

    layout->addWidget (m_label);

    updateTooltip ();

    setLayout (layout);
    }


void TaskWidget::updateTooltip ()
    {
    setToolTip (m_task.description ());
    }

} // namespace ui::clock
