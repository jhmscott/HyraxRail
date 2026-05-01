/**
 * @file        actuators/editactuator.hpp
 * @brief       Dialog box to add or edit an actuator
 * @author      Justin Scott
 * @date        2026-04-26
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <control/controllers/base.hpp>

#include <layout/actuator.hpp>

#include <ui/common/formdialog.hpp>

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>

namespace ui::actuators
{

//////////////////////////////////////////////////////////////////////////////
/// Dialog box to add or edit an actuator
///
//////////////////////////////////////////////////////////////////////////////
class EditActuatorDialog : public common::FormDialog
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller  Controller to add or edit actuator for
    /// @param[in]  parent      Parent widget
    /// @param[in]  actuator    (optional) Actuator to edit. If NULL, this will
    ///                         be an "Add" dialog box
    ///
    //////////////////////////////////////////////////////////////////////////////
    EditActuatorDialog (control::ControllerBase&    controller,
                        QWidget*                    parent,
                        const layout::Actuator*     actuator = NULL);

    //////////////////////////////////////////////////////////////////////////////
    /// Get the name configured by the user
    ///
    /// @return     Actuator friendly name
    ///
    //////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_name->text ().toStdString (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the track protocol address configured by the user
    ///
    /// @return     Track protocol address
    ///
    //////////////////////////////////////////////////////////////////////////////
    uint getAddress () const { return m_address->value (); }


    //////////////////////////////////////////////////////////////////////////////
    /// Get the UI icon selected by the user
    ///
    /// @return     Actuator UI icon
    ///
    //////////////////////////////////////////////////////////////////////////////
    layout::actuatorIcon getIcon () const
        { return m_icon->currentData ().value<layout::actuatorIcon> (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the actuator mode selected by the user
    ///
    /// @return     Actuator mode
    ///
    //////////////////////////////////////////////////////////////////////////////
    layout::actuatorMode getMode () const
        { return m_icon->currentData ().value<layout::actuatorMode> (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the actuation duration selected by the user
    ///
    /// @return     Actuation duration
    ///
    //////////////////////////////////////////////////////////////////////////////
    uint getDuration () const
        { return m_duration->currentData ().toInt (); }
private:
    QLineEdit*  m_name;     ///< Friendly name field
    QSpinBox*   m_address;  ///< Track protocol address spin box
    QComboBox*  m_icon;     ///< Icon dropdown
    QComboBox*  m_mode;     ///< Actuator mode dropdown
    QComboBox*  m_duration; ///< Duration dropdown

    //////////////////////////////////////////////////////////////////////////////
    /// Check if all fields have valid input
    ///
    /// @return     True if all fields have been filled out with valid input
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;
    };

} // namespace ui::actuators