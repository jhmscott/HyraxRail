/**
 * @file        tains/editloco.hpp
 * @brief       Dialog box to create or edit a locomotive
 * @author      Justin Scott
 * @date        2026-08-22
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/formdialog.hpp>
#include <ui/common/pointedwidget.hpp>
#include <ui/common/schemeicon.hpp>
#include <ui/common/tiereddropdown.hpp>
#include <ui/lang.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>

namespace ui::trains
{

///////////////////////////////////////////////////////////////////////////////
/// Dialog box to create or edit a locomotive
///
/// @ingroup    COMPONENT_DLG
///
///////////////////////////////////////////////////////////////////////////////
class EditLocoDialog : public common::FormDialog
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor, add loco dialog variant
    ///
    /// @param[in]  manager     Controller manager
    /// @param[in]  parent      Parent widget
    /// @param[in]  controller  Controller to select from the drop down
    ///                         If NULL, the first is selected
    ///
    ///////////////////////////////////////////////////////////////////////////////
    EditLocoDialog (const control::ControllerManager&   manager,
                    QWidget*                            parent,
                    const control::ControllerBase*      controller) :
        EditLocoDialog (manager, parent, controller, NULL)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor, edit loco dialog variant
    ///
    /// @param[in]  manager     Controller manager
    /// @param[in]  parent      Parent widget
    /// @param[in]  loco        Locomotive to edit
    ///
    ///////////////////////////////////////////////////////////////////////////////
    EditLocoDialog (const control::ControllerManager&   manager,
                    QWidget*                            parent,
                    const layout::Locomotive&           loco) :
        EditLocoDialog (manager,
                        parent,
                        static_cast<const control::ControllerBase*> (loco.getController ()),
                       &loco)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the selected controller
    ///
    /// @return     Controller instance
    ///
    ///////////////////////////////////////////////////////////////////////////////
    const control::ControllerBase* getController () const
        { return m_controllerCb->currentData ().value<const control::ControllerBase*> (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the locomotive name
    ///
    /// @return     Locomotive name
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_name->text ().toStdString (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the locomotive track protocol address
    ///
    /// @return     Locomotive track protocol address
    ///
    ///////////////////////////////////////////////////////////////////////////////
    uint getAddress () const { return m_address->value (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the track protocol
    ///
    /// @return     Locomotive track protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    layout::trackProtocol getProtocol () const
        { return m_protocol->currentData ().value<layout::trackProtocol> (); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the locomotive functions
    ///
    /// @return     List of functions
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<layout::funcInfo> getFunctions () const;

private:
    const control::ControllerManager&   m_manager;          ///< List of controllers
    const control::ControllerBase*      m_controller;       ///< Controller being used

    // Forms
    QFormLayout*                        m_form;             ///< Locomotive form
    QFormLayout*                        m_funcForm;         ///< Function form

    // Locomotive fields
    QWidget*                            m_controllerWdgt;   ///< Controller widget (static label or dropdown)
    QComboBox*                          m_controllerCb;     ///< Controller dropdown
    QLineEdit*                          m_name;             ///< Locomotive name
    QSpinBox*                           m_address;          ///< Locomotive track address
    common::SchemeComboBox*             m_protocol;         ///< Locomotive track protocol

    // Function bar field
    QWidget*                            m_funcBar;          ///< Function bar widget
                                                            ///  Contains dropdown, add and delete
    common::SchemeComboBox*             m_functions;        ///< List of available functions
    common::PointedIconButton*          m_plus;             ///< Add function
    common::PointedIconButton*          m_trash;            ///< Delete function

    // Function fields
    QGroupBox*                          m_functionGroup;
    common::TieredDropdown*             m_funcIcons;        ///< List of function icon
    QComboBox*                          m_funcNumbers;      ///< List of function numbers

    bool                                m_edit;             ///< True if we are editing a loco
                                                            ///  False if we are creating one

    UILANG_ON_CHANGE (common::FormDialog, setLabels ())

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  manager     Controller manager
    /// @param[in]  parent      Parent widget
    /// @param[in]  controller  Controller to select from the drop down
    ///                         If NULL, the first is selected
    /// @param[in]  loco        Locomotive to edit
    ///                         If NULL, this is an add dialog
    ///
    ///////////////////////////////////////////////////////////////////////////////
    EditLocoDialog (const control::ControllerManager&   manager,
                    QWidget*                            parent,
                    const control::ControllerBase*      controller,
                    const layout::Locomotive*           loco);

    ///////////////////////////////////////////////////////////////////////////////
    /// Check that the form has acceptable input
    ///
    /// @return     True if all form fields have been set correctly
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the text labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Populate the protocol dropdown with the protocols supported by a given
    /// controller
    ///
    /// @param[in]  controller      Controller to populate the protocols for
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void populateProtocols (const control::ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Sets the min and max of the address field to match the current track protocol
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void limitAddress ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Populate the function icon dropdown
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void populateFunctionIcons ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Populate the function number dropdown
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void populateFunctionNumbers ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Refresh the function number dropdown
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void refreshFunctionNumbers ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the function fields to match a given function in the dropdown
    ///
    /// @param[in]  idx     Function dropdown index
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setFunctionFields (int idx);

    ///////////////////////////////////////////////////////////////////////////////
    /// Read the values from the function field and update the currently selected
    /// function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void readFunctionFields ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the enabled/disabled state of the add/delete function buttons
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void updateButtonStates ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the maximum number of functions available with the current combination
    /// of controller and track protocol
    ///
    /// @return     Max number of locomotive function
    ///
    ///////////////////////////////////////////////////////////////////////////////
    uint getMaxFunctions () const;

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Called when the user changes the controller
    ///
    /// @param[in]  idx     Index of the new controller
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onControllerChange (int idx);

    ///////////////////////////////////////////////////////////////////////////////
    /// Called when the user changes the track protocol
    ///
    /// @param[in]  idx     Index of the track protocol selected
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onProtocolChange (int idx);

    /////////////////////////////////////////////////////////////////////////////
    /// Called when the user selects a function from the dropdown
    ///
    /// @param[in]  idx     Index of selected function
    ///
    /////////////////////////////////////////////////////////////////////////////
    void onFunctionChange (int idx);

    /////////////////////////////////////////////////////////////////////////////
    /// Prompt the user to delete a function
    ///
    /////////////////////////////////////////////////////////////////////////////
    void deleteFunction ();

    /////////////////////////////////////////////////////////////////////////////
    /// Add a new function to the combobox
    ///
    /////////////////////////////////////////////////////////////////////////////
    void addFunction ();

    };

} // namespace ui::trains