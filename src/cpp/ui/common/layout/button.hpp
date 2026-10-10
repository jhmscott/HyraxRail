/**
 * @file        layout/button.hpp
 * @brief       Button widget base class for triggering a layout component
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/gesture.hpp>

namespace ui::common
{
//////////////////////////////////////////////////////////////////////////////
/// Button widget base class for triggering a layout component
///
//////////////////////////////////////////////////////////////////////////////
class LayoutButton : public HoldToOpenMenu<QWidget>
    {
    Q_OBJECT
public:
    using HoldToOpenMenu<QWidget>::HoldToOpenMenu;

signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the user has requested to remove this component
    ///
    //////////////////////////////////////////////////////////////////////////////
    void removeRequested ();
    };
} // namespace ui::common
