/**
 * @file        sensors/sensordelegate.hpp
 * @brief       Delegate for the sensor component
 * @author      Justin Scott
 * @date        2026-10-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/layout/delegate.hpp>

#include <ui/sensors/sensorbar.hpp>

namespace ui::sensors
{

//////////////////////////////////////////////////////////////////////////////
/// Delegate for the sensor component
///
//////////////////////////////////////////////////////////////////////////////
class SensorDelegate : public common::LayoutDelegate
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Open the dialog to create an sensor module
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual common::LayoutButton* openCreate (control::ControllerBase&  controller,
                                              QWidget*                  parent) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Create the sensor module bars
    ///
    /// @param[in]  controller      Controller to create the sensor widgets for
    /// @param[in]  addToLayout     Callback to add widget to the layout
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void createButtons (control::ControllerBase&                    controller,
                                std::function<void (common::LayoutButton*)> addToLayout) override;
    };

} // namespace ui::sensors
