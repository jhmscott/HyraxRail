/**
 * @file        sensors/editsensor.hpp
 * @brief       Dialog to create/edit sensors
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <control/controllers/base.hpp>

#include <layout/sensor.hpp>

#include <ui/common/formdialog.hpp>
#include <ui/lang.hpp>

#include <QFormLayout>

namespace ui::sensors
{

//////////////////////////////////////////////////////////////////////////////
/// Dialog box to create/edit dialog
///
//////////////////////////////////////////////////////////////////////////////
class EditSensorDialog : public common::FormDialog
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller  Controller to edit/create sensor for
    /// @param[in]  parent      Parent widget
    /// @param[in]  module      (optional) Sensor module to edit
    ///                         Pass NULL for a create dialog
    ///
    //////////////////////////////////////////////////////////////////////////////
    EditSensorDialog (const control::ControllerBase&    controller,
                      QWidget*                          parent,
                      const layout::SensorModule*       module = NULL);

    //////////////////////////////////////////////////////////////////////////////
    /// Get the number of ports configured for the module
    ///
    /// @return     Number of ports
    ///
    //////////////////////////////////////////////////////////////////////////////
    uint getPortCount () const { return m_portCount->currentData ().toInt (); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the position of the module
    ///
    /// @return     Position of the module
    ///
    //////////////////////////////////////////////////////////////////////////////
    uint getModulePosition () const { return m_modulePosition->currentData ().toInt (); }

private:
    QFormLayout*    m_form;             ///< Form layout
    QComboBox*      m_portCount;        ///< Number of ports for the module
    QComboBox*      m_modulePosition;   ///< Position of the module
    bool            m_edit;             ///< True if this is ann edit dialog, false for create

    UILANG_ON_CHANGE (common::FormDialog, setLabels ())

    //////////////////////////////////////////////////////////////////////////////
    /// Check if the form has been filled correctly
    ///
    /// @return     true if filled correctly
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override { return true; }


    //////////////////////////////////////////////////////////////////////////////
    /// Set the dialog's labels
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setLabels ();
    };

} // namespace ui::sensor
