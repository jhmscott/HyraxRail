/**
 * @file        clock/editauto.cpp
 * @brief       Dialog box to create or edit an automation
 * @author      Justin Scott
 * @date        2026-06-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/actuators/resources.hpp>
#include <ui/trains/resources.hpp>

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

static const int ACTUATOR_PARENT_IDX    = 0;
static const int ROUTE_PARENT_IDX       = 1;

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
        case control::AutomationItem::type::LOCO_FUNC:
            {
            auto info = item.getFunctionInfo ();

            icon = trains::resources::getFunctionInfo (info->icon).icon;
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
    m_layout->addRow (new QLabel{ this }, m_items       = new common::TieredDropdown{ this });
    m_layout->addRow (new QLabel{ this }, m_actions     = new common::OptionalDropdown{ this });
    m_layout->addRow (new QLabel{ this }, m_conditions  = new QComboBox{ this });
    m_layout->addRow (new QLabel{ this }, m_doOnce      = new QCheckBox{ this });
    m_layout->addRow (new QLabel{ this }, m_enabled     = new QCheckBox{ this });

    common::refreshStyleSheetOnColorSchemeChange (*m_conditions);

    m_items->addParentItem ("", "misc/split");
    m_items->addParentItem ("", "misc/path");

    std::map<layout::Locomotive, int> locoToParent;

    int locoParentCounter = 2;

    for (auto& controller : controllers)
        {
        for (auto& loco : controller.getLocomotives ())
            {
            m_items->addParentItem (loco.getName ().c_str (), "misc/train");

            locoToParent[loco] = locoParentCounter;
            ++locoParentCounter;
            }
        }

    for (auto& controller : controllers)
        {
        for (auto& item : controller.getAutomationItems ())
            {
            std::string name    = item.name ();
            auto        icon    = getIcon (item);
            int         parent  = -1;

            switch (item.getType ())
                {
                case control::AutomationItem::type::ACTUATOR:
                    {
                    parent = ACTUATOR_PARENT_IDX;
                    break;
                    }
                case control::AutomationItem::type::ROUTE:
                    {
                    parent = ROUTE_PARENT_IDX;
                    break;
                    }
                case control::AutomationItem::type::LOCO_FUNC:
                    {
                    auto [loco, _] = *item.getFunction ();

                    parent = locoToParent[loco];
                    break;
                    }
                }

            m_items->addChildItem (name.c_str (),
                                   QVariant::fromValue (std::move (item)),
                                   icon,
                                   parent);
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
       not m_items->isParentItem (m_items->currentIndex ()) &&
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

    m_items->setParentItemText (ACTUATOR_PARENT_IDX,    tr ("Actuators"));
    m_items->setParentItemText (ROUTE_PARENT_IDX,       tr ("Routes"));

    for (int ii = 0; ii < m_items->count (); ++ii)
        {
        if (not m_items->isParentItem (ii))
            {
            const auto& item = m_items->itemData (ii).value<control::AutomationItem> ();

            if (control::AutomationItem::type::LOCO_FUNC == item.getType ())
                {
                m_items->setItemText (ii, item.name ().c_str ());
                }
            }
        }
    }


void EditAutoDialog::updateActions (int item)
    {
    const auto& currentItem = m_items->itemData (item).value<control::AutomationItem> ();

    m_actions->refreshComboboxItems (
        [&] () -> void
        {
        for (auto ii : utils::algorithm::bitsetToSet (currentItem.getActions ()))
            {
            auto action = static_cast<control::AutomationItem::action> (ii);

            m_actions->addItem (control::actionText (action),
                                QVariant::fromValue (action));
            }
        });

    emit inputChanged ();
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

    emit inputChanged ();
    }

} // namespace ui::automation
