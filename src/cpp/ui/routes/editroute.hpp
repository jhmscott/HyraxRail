/**
 * @file        routes/editroute.hpp
 * @brief       Dialog box to edit/add routes
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>

#include <ui/common/formdialog.hpp>
#include <ui/lang.hpp>
#include <ui/routes/includebutton.hpp>

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>


namespace ui::routes
{
//////////////////////////////////////////////////////////////////////////////
/// Dialog box to edit or add a route
///
/// @ingroup    COMPONENT_DLG
///
//////////////////////////////////////////////////////////////////////////////
class EditRouteDialog : public common::FormDialog
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller      Controller to add/edit route for
    /// @param[in]  parent          Parent widget
    /// @param[in]  route           Route to edit. If NULL, this is an add route
    ///                             dialog
    //////////////////////////////////////////////////////////////////////////////
    EditRouteDialog (control::ControllerBase&   controller,
                     QWidget*                   parent,
                     const layout::Route*       route = NULL);

    //////////////////////////////////////////////////////////////////////////////
    /// Get the actuators the user has set
    ///
    /// @return     Actuator list
    ///
    //////////////////////////////////////////////////////////////////////////////
    layout::routeList getActuators () const;

    //////////////////////////////////////////////////////////////////////////////
    /// Get the name of the route set by the user
    ///
    /// @return     Route name
    ///
    //////////////////////////////////////////////////////////////////////////////
    std::string getName () const { return m_name->text ().toStdString (); }
private:
    std::vector<ActuatorIncludeButton*> m_buttonList;       ///< Actuator buttons
    std::string                         m_controllerName;   ///< Name of the controller this route belongs to
    QLineEdit*                          m_name;             ///< Name field
    QLabel*                             m_label;            ///< Route name label
    bool                                m_edit;             ///< True if this is in edit mode, false if this is add mode

    //////////////////////////////////////////////////////////////////////////////
    /// Set the form labels
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    //////////////////////////////////////////////////////////////////////////////
    /// Check if this dialog has valid input
    ///
    /// @return     True if the entered values are valid
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;

    UILANG_ON_CHANGE (common::FormDialog, setLabels ())
    };

}