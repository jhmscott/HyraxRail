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
class SensorBar : public common::LayoutButton
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  module      Module to create a bar for
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    SensorBar (const layout::SensorModule& module, QWidget* parent);

private:
    std::vector<SensorManageState*> m_states;   ///< Sensor state indicators
    layout::SensorModule            m_module;   ///< Module this is displaying the state of
    };

} // namespace ui::sensors