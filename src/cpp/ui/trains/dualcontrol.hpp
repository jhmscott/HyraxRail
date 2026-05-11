/**
 * @file        trains/dualcontrol.hpp
 * @brief       Panel to control up to two locos at once, if enough space
 *              is available. If not, behaves like a single loco controller
 * @author      Justin Scott
 * @date        2026-01-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/manager.hpp>

#include <ui/trains/lococontrolpanel.hpp>

#include <QWidget>

namespace ui::trains
{
///////////////////////////////////////////////////////////////////////////////
/// Widget with up to two control panels. If there is only enough space for one,
/// only one will be shown
///
///////////////////////////////////////////////////////////////////////////////
class DualControlWidget : public QWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  parent          Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DualControlWidget (control::ControllerManager* controllers, QWidget* parent);

private:
    LocoControlPanel* m_left;   ///< Left control panel. Default if not enough room for two
    LocoControlPanel* m_right;  ///< Right control panel. Hidden if not enough room for two

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the widget being resized
    ///
    /// @param[in]  event   Resize event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void resizeEvent (QResizeEvent* event) override;
    };

} // namespace ui::trains