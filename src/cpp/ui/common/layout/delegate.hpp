/**
 * @file        layout/delegate.hpp
 * @brief       Delegate for component specific actions
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>

#include <ui/common/layout/button.hpp>

namespace ui::common
{
//////////////////////////////////////////////////////////////////////////////
/// Delegate for component specific actions
///
//////////////////////////////////////////////////////////////////////////////
class LayoutDelegate
    {
public:
    /////////////////////////////////////////////////////////////////////////////
    /// Open a dialog to create a new component
    ///
    /// @param[in]  controller      Controller to create component for
    /// @param[in]  parent          Parent widget
    ///
    /// @return     Widget for newly created component, or NULL if none created
    ///
    /////////////////////////////////////////////////////////////////////////////
    virtual LayoutButton* openCreate (control::ControllerBase& controller, QWidget* parent) = 0;

    /////////////////////////////////////////////////////////////////////////////
    /// Fill the LayoutGroup with component buttons
    ///
    /// @param[in]  controller      Controller to create buttons for
    /// @param[in]  addToLayout     Callback to add the button to the layout
    ///
    /////////////////////////////////////////////////////////////////////////////
    virtual void createButtons (control::ControllerBase&            controller,
                                std::function<void (LayoutButton*)> addToLayout) = 0;

    };
} // namespace ui::common
