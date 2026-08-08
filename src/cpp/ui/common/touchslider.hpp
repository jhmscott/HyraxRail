/**
 * @file        common/touchslider.hpp
 * @brief       Slider class that fixes the issues with QSlider on touchscreens
 * @author      Justin Scott
 * @date        2026-08-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QSlider>

namespace ui::common
{

///////////////////////////////////////////////////////////////////////////////
/// Slider class that fixes the issues with QSlider on touchscreens
///
///////////////////////////////////////////////////////////////////////////////
class TouchSlider : public QSlider
    {
public:
    using QSlider::QSlider;

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a mouse press event
    ///
    /// @param[in]  event   Mouse event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void mousePressEvent (QMouseEvent* event) override;

private:

    ///////////////////////////////////////////////////////////////////////////////
    /// helper to get the handle rect
    ///
    /// @return     Slider handle rect
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QRect getHandleRect () const;
    };

} // namespace ui::common