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
#include <ui/routes/includebutton.hpp>

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>


namespace ui::routes
{
//////////////////////////////////////////////////////////////////////////////
/// Dialog box to edit or add a route
///
//////////////////////////////////////////////////////////////////////////////
class EditRouteDialog : public common::FormDialog
    {
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
    std::vector<ActuatorIncludeButton*> m_buttonList;   ///< Actuator buttons
    QLineEdit*                          m_name;         ///< Name field

    //////////////////////////////////////////////////////////////////////////////
    /// Check if this dialog has valid input
    ///
    /// @return     True if the entered values are valid
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;
    };

}