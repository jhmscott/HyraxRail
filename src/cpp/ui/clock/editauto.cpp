/**
 * @file        clock/editauto.cpp
 * @brief       Dialog box to create or edit an automation
 * @author      Justin Scott
 * @date        2026-06-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/actuators/resources.hpp>
#include <ui/clock/alarm.hpp>
#include <ui/clock/editauto.hpp>
#include <ui/clock/monthly.hpp>
#include <ui/clock/timer.hpp>
#include <ui/clock/weekly.hpp>
#include <ui/common/duration.hpp>
#include <ui/common/utils.hpp>

#include <utils/algorithm.hpp>
#include <utils/string.hpp>

#include <QBoxLayout>
#include <QLineEdit>


namespace ui::clock
{


//////////////////////////////////////////////////////////////////////////////
/// Get the icon for an automation item
///
/// @param[in]  item        Automation
///
/// @return     Scheme dependent icon
///
//////////////////////////////////////////////////////////////////////////////
static utils::resources::Icon getIcon (const control::AutomationItem& item)
    {
    utils::resources::Icon icon;

    switch (item.getType ())
        {
        case control::AutomationItem::type::ACTUATOR:
            {
            icon = actuators::resources::getIconInfo (item.getActuator ()->getIcon ()).icon;
            break;
            }
        case control::AutomationItem::type::ROUTE:
            {
            icon = "misc/path";
            break;
            }
        }

    return icon;
    }


EditAutoDialog::EditAutoDialog (control::ControllerManager& controllers,
                                QWidget*                    parent,
                                control::AutomationTask*    task) :
    common::FormDialog (parent),
    m_edit (NULL != task)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_layout = new QFormLayout;

    m_layout->addRow (new QLabel{ this }, m_name        = new QLineEdit{ this });
    m_layout->addRow (new QLabel{ this }, m_items       = new common::SchemeComboBox{ this });
    m_layout->addRow (new QLabel{ this }, m_actions     = new common::OptionalDropdown{ this });
    m_layout->addRow (new QLabel{ this }, m_conditions  = new QComboBox{ this });
    m_layout->addRow (new QLabel{ this }, m_doOnce      = new QCheckBox{ this });
    m_layout->addRow (new QLabel{ this }, m_enabled     = new QCheckBox{ this });

    for (auto& controller : controllers)
        {
        for (auto& item : controller.getAutomationItems ())
            {
            std::string name = item.name ();
            auto        icon = getIcon (item);

            m_items->addItem (icon,
                              name.c_str (),
                              QVariant::fromValue (std::move (item)));
            }
        }

    updateActions (m_items->currentIndex ());

    for (int ii = 0; ii < control::AutomationCondition::NUM_TYPES; ++ii)
        {
        auto condition = static_cast<control::AutomationCondition::type> (ii);

        m_conditions->addItem (control::conditionText (condition),
                               QVariant::fromValue (condition));
        }

    m_conditionForms[control::AutomationCondition::TYPE_TIMER]          = new TimerConditionForm{ this };
    m_conditionForms[control::AutomationCondition::TYPE_DAILY_ALARM]    = new AlarmConditionForm{ this };
    m_conditionForms[control::AutomationCondition::TYPE_WEEKLY_ALARM]   = new WeeklyAlarmConditionForm{ this };
    m_conditionForms[control::AutomationCondition::TYPE_MONTHLY_ALARM]  = new MonthlyAlarmConditionForm{ this };

    layout->addLayout (m_layout);

    for (ConditionForm* form : m_conditionForms)
        {
        layout->addWidget (form);

        connect (form,
                &ConditionForm::inputChanged,
                 this,
                &EditAutoDialog::inputChanged);
        }

    layout->addWidget (m_buttons, 0, Qt::AlignHCenter | Qt::AlignBottom);

    if (NULL != task)
        {
        m_name->setText (task->name);
        m_doOnce->setChecked (task->doOnce);
        m_enabled->setChecked (task->enabled);
        m_actions->setIndexByUserData (task->action);

        common::setComboBoxIndexByUserData (*m_conditions, task->condition->getType ());
        common::setComboBoxIndexByUserData (*m_items, *task->getItem ());

        activeConditionForm ()->setCondition (*task->condition);
        }

    m_name->setValidator (new QRegularExpressionValidator{
                                QRegularExpression{ utils::str::NON_EMPTY_REGEX }, this });

    conditionChanged (m_conditions->currentIndex ());
    inputChanged ();

    connect (m_items,
            &QComboBox::currentIndexChanged,
             this,
            &EditAutoDialog::updateActions);

    connect (m_conditions,
            &QComboBox::currentIndexChanged,
             this,
            &EditAutoDialog::conditionChanged);

    connect (m_conditions,
            &QComboBox::currentIndexChanged,
             this,
            &EditAutoDialog::inputChanged);

    connect (m_name,
            &QLineEdit::textChanged,
             this,
            &EditAutoDialog::inputChanged);

    setWindowIcon ("misc/clock");
    setLabels ();
    setLayout (layout);
    }

void EditAutoDialog::setTaskParameters (control::AutomationTask& task) const
    {
    task.action     = m_actions->currentData ().value<control::AutomationItem::action> ();
    task.condition  = activeConditionForm ()->getCondition ();
    task.enabled    = m_enabled->isChecked ();
    task.doOnce     = m_doOnce->isChecked ();
    task.name       = m_name->text ();

    task.setItem (new control::AutomationItem{
                        m_items->currentData ().value<control::AutomationItem> ()});
    }


bool EditAutoDialog::hasAcceptableInput () const
    {
    return m_name->hasAcceptableInput () &&
           activeConditionForm ()->hasAcceptableInput ();
    }


void EditAutoDialog::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_name,         tr ("Name"));
    common::setFormRowText (*m_layout, *m_items,        tr ("Item"));
    common::setFormRowText (*m_layout, *m_actions,      tr ("Action"));
    common::setFormRowText (*m_layout, *m_conditions,   tr ("Condition"));
    common::setFormRowText (*m_layout, *m_doOnce,       tr ("Do Once"));
    common::setFormRowText (*m_layout, *m_enabled,      tr ("Enabled"));

    for (int ii = 0; ii < m_actions->count (); ++ii)
        {
        auto action = m_actions->
                      itemData (ii).
                      value<control::AutomationItem::action> ();

        m_actions->setItemText (ii, control::actionText (action));
        }

    for (int ii = 0; ii < m_conditions->count (); ++ii)
        {
        auto condition = m_conditions->
                         itemData (ii).
                         value<control::AutomationCondition::type> ();

        m_conditions->setItemText (ii, control::conditionText (condition));
        }

    if (m_edit)
        {
        setWindowTitle (tr ("Edit Automation"));
        }
    else
        {
        setWindowTitle (tr ("Add Automation"));
        }
    }


void EditAutoDialog::updateActions (int item)
    {
    const auto& currentItem = m_items->itemData (item).value<control::AutomationItem> ();
    auto        lastAction  = 0 == m_actions->count () ?
                              control::AutomationItem::NUM_ACTIONS :
                              m_actions->currentData ().value<control::AutomationItem::action> ();

    m_actions->clear ();

    for (auto ii : utils::algorithm::bitsetToSet (currentItem.getActions()))
        {
        auto action = static_cast<control::AutomationItem::action> (ii);

        m_actions->addItem (control::actionText (action),
                            QVariant::fromValue (action));
        }

    if (control::AutomationItem::NUM_ACTIONS != lastAction)
        {
        m_actions->setIndexByUserData (lastAction);
        }
    }

ConditionForm* EditAutoDialog::activeConditionForm ()
    {
    return m_conditionForms[
            m_conditions->currentData ().
                value<control::AutomationCondition::type> ()];
    }

const ConditionForm* EditAutoDialog::activeConditionForm () const
    {
    return m_conditionForms[
            m_conditions->currentData ().
                value<control::AutomationCondition::type> ()];
    }

void EditAutoDialog::hideConditionForms ()
    {
    for (ConditionForm* form : m_conditionForms)
        {
        form->hide ();
        }
    }

void EditAutoDialog::conditionChanged (int idx)
    {
    hideConditionForms ();

    activeConditionForm ()->show ();
    }

} // namespace ui::automation
