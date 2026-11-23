/**
 * @file        sensors/sensorbar.hpp
 * @brief       Bar representing the state of sensors for a given sensor module
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/sensor.hpp>

#include <ui/common/layout/button.hpp>

#include <ui/sensors/managestate.hpp>

#include <QHBoxLayout>
#include <QWidget>

namespace ui::sensors
{

//////////////////////////////////////////////////////////////////////////////
/// Bar containing indicators for each sensor in a module
///
//////////////////////////////////////////////////////////////////////////////
class SensorBar : public common::ComponentButton<layout::SensorModule>
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  automations List of automations
    /// @param[in]  controller  Controller for this sensor module
    /// @param[in]  module      Module to create a bar for
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    SensorBar (control::AutomationManager&  automations,
               control::ControllerBase&     controller,
               const layout::SensorModule&  module,
               QWidget*                     parent);

private:
    std::vector<SensorManageState*> m_states;       ///< Sensor state indicators
    layout::SensorModule            m_module;       ///< Module this is displaying the state of
    control::AutomationManager&     m_automations;  ///< List of automations
    control::ControllerBase&        m_controller;   ///< Controller for this sensor
    QLabel*                         m_name;         ///< Module name label

    //////////////////////////////////////////////////////////////////////////////
    /// Get the title for the delete sensor message box
    ///
    /// @return     Translated title
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual QString getDeleteText () const override { return tr ("Delete Sensor Module"); }

    //////////////////////////////////////////////////////////////////////////////
    /// Edit this sensor module
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void editComponent () override;

    };

} // namespace ui::sensors
