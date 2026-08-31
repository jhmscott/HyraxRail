/**
 * @file        clock/editauto.hpp
 * @brief       Dialog box to create or edit an automation
 * @author      Justin Scott
 * @date        2026-06-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/automation/task.hpp>
#include <control/controllers/manager.hpp>

#include <ui/clock/condition.hpp>
#include <ui/common/formdialog.hpp>
#include <ui/common/optionaldropdown.hpp>
#include <ui/lang.hpp>

#include <QCheckBox>
#include <QFormLayout>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Dialog box to create or edit an automation
///
///////////////////////////////////////////////////////////////////////////////
class EditAutoDialog : public common::FormDialog
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers with items to automate
    /// @param[in]  parent          Parent widget
    /// @param[in]  task            Automation to edit. If NULL, this wil create
    ///                             an automation, not edit one
    ///
    ///////////////////////////////////////////////////////////////////////////////
    EditAutoDialog (control::ControllerManager& controllers,
                    QWidget*                    parent,
                    control::AutomationTask*    task = NULL);

    ///////////////////////////////////////////////////////////////////////////////
    /// Populate an automation task's parameters
    ///
    /// @param[out] task    Task to populate
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTaskParameters (control::AutomationTask& task) const;

private:
    /// Set of condition forms
    using forms = std::array<ConditionForm*, control::AutomationCondition::NUM_TYPES>;

    QFormLayout*                m_layout;           ///< Main form layout
    QLineEdit*                  m_name;             ///< Name field
    common::SchemeComboBox*     m_items;            ///< List of items available to automate
    common::OptionalDropdown*   m_actions;          ///< List of actions compatible with the selected item
    QComboBox*                  m_conditions;       ///< List of automation conditons
    QCheckBox*                  m_doOnce;           ///< Do the automation only once
    QCheckBox*                  m_enabled;          ///< Enable the automation
    forms                       m_conditionForms;   ///< Condition forms
    bool                        m_edit;             ///< True if editing an exisitng task

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if the dialog has acceptable input
    ///
    /// @return     True if all fields are filled correctly
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the dialog box labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the form for the selected condition
    ///
    /// @return     Condition form
    ///
    ///////////////////////////////////////////////////////////////////////////////
    ConditionForm* activeConditionForm ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the form for the selected condition. const version
    ///
    /// @return     Condition form
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const ConditionForm* activeConditionForm () const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Hide all the confition forms
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void hideConditionForms ();

    UILANG_ON_CHANGE (common::FormDialog, setLabels ())

private slots:

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the actions for a given item
    ///
    /// @param[in]  item        Item to populate actions for
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void updateActions (int item);

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the condition selection changing
    ///
    /// @param[in]  idx     Condition index
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void conditionChanged (int idx);
    };

} // namespace ui::common