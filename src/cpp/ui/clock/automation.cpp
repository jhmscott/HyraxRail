/**
 * @file        clock/automation.cpp
 * @brief       List of configured automations
 * @author      Justin Scott
 * @date        2026-06-03
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/clock/automation.hpp>
#include <ui/clock/editauto.hpp>
#include <ui/clock/task.hpp>

#include <ui/common/pointedwidget.hpp>
#include <ui/common/seperator.hpp>

#include <QBoxLayout>

namespace ui::clock
{
AutomationGroup::AutomationGroup (control::ControllerManager&   controllers,
                                  control::AutomationManager&   automations,
                                  QWidget*                      parent) :
    QGroupBox (parent),
    m_manager (automations),
    m_controllers (controllers)
    {
    m_layout = new QVBoxLayout{ this };

    m_layout->addWidget (m_addBtn = new common::AddButton{ this });

    for (auto& task : m_manager)
        {
        addTask (task);
        }

    connect (m_addBtn,
            &common::AddButton::addPressed,
             this,
            &AutomationGroup::addAutomation);

    connect (&m_manager,
             &control::AutomationManager::taskDestroyed,
              this,
             &AutomationGroup::removeTask);

    setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Maximum);
    setTitles ();

    setLayout (m_layout);
    }

void AutomationGroup::addTask (control::AutomationTask& task)
    {
    m_layout->insertWidget (m_layout->count () - 1, new TaskWidget{ task, this });
    m_layout->insertWidget (m_layout->count () - 1, new common::Separator{ this });
    }

void AutomationGroup::setTitles ()
    {
    setTitle (tr ("Automations"));

    m_addBtn->setLabelText (tr ("Add Automation"));
    }

void AutomationGroup::removeTask (const control::AutomationTask& task)
    {
    // count() - 1  : Exclude the last widget, which is the add button
    // ii += 2      : Exclude every other widget (separators)
    //
    for (int ii = 0; ii < m_layout->count () - 1; ii += 2)
        {
        TaskWidget* taskWidget = static_cast<TaskWidget*> (m_layout->itemAt (ii)->widget ());

        if (&task == &taskWidget->getTask ())
            {
            QWidget* separator = m_layout->itemAt (ii + 1)->widget ();

            // Remove separator
            m_layout->removeWidget (separator);
            // Remove task widget
            m_layout->removeWidget (taskWidget);

            delete separator;
            delete taskWidget;

            break;
            }
        }
    }

void AutomationGroup::addAutomation ()
    {
    EditAutoDialog dlg{ m_controllers, this };

    if (QDialog::Accepted == dlg.exec ())
        {
        auto& task = m_manager.emplace ();

        dlg.setTaskParameters (task);

        addTask (task);
        }
    }
}