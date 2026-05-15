/**
 * @file        config/dialog.hpp
 * @brief       Dialog box for the configuration of controllers
 * @author      Justin Scott
 * @date        2026-02-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>

#include <control/controllers/base.hpp>

#include <ui/config/deviceinfo.hpp>
#include <ui/common/formdialog.hpp>
#include <ui/common/optionaldropdown.hpp>
#include <ui/common/ipfield.hpp>

namespace ui::config
{


//////////////////////////////////////////////////////////////////////////////
/// Controller configuration dialog box
///
//////////////////////////////////////////////////////////////////////////////
class Dialog : public common::FormDialog
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widegt
    /// @param[in]  controller  (optional) controller to edit. If NULL, this an
    ///                         "Add Controller" dialog
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit Dialog (QWidget* parent, control::ControllerBase* controller = NULL);

    //////////////////////////////////////////////////////////////////////////////
    /// Return the settings to create a controller from the entered settings
    ///
    /// @return     Control creation settings
    ///
    //////////////////////////////////////////////////////////////////////////////
    control::createControllerInfo createController () const;
private:
    common::OptionalDropdown*   m_controller;   ///< Controller drop down
    common::OptionalDropdown*   m_protocol;     ///< Application layer protocol drop down
    common::OptionalDropdown*   m_transport;    ///< Transport layer (UDP, TCP, COM) derop down
    DeviceInfoWidget*           m_network;      ///< Network settings entry (for UDP and TCP)
    DeviceInfoWidget*           m_com;          ///< COM port settings entry
    DeviceInfoWidget*           m_active;       ///< Active device settings (m_network or m_com)
    QLineEdit*                  m_name;         ///< Controller name field
    QFormLayout*                m_layout;       ///< Main form layout


    //////////////////////////////////////////////////////////////////////////////
    /// Makes the network device widget active and hides the COM entry widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setNetworkMode ();

    //////////////////////////////////////////////////////////////////////////////
    /// Makes the COM device widget active and hides the network entry widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setComMode ();

    //////////////////////////////////////////////////////////////////////////////
    /// Check if all the fields have been filled out correctly
    ///
    /// @return     true if dialog has acceptable fields
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;

    //////////////////////////////////////////////////////////////////////////////
    /// Get the error message to help the user if the fields aren't filled out
    /// correctly
    ///
    /// @return     Error message
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual QString getErrorString () const override;

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Handles the transport combobox index change by setting the active transport
    /// widget
    ///
    /// @param[in]  idx     Combobox index
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setTransportProto (int idx);

    };

} // namespace ui::config