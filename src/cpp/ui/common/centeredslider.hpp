/**
 * @file        common/centeredslider.hpp
 * @brief       Slider bar widget, with positive/negative values with the 0 position at the
 *              center, and the progress drawn from this origin point
 * @author      Justin Scott
 * @date        2026-08-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/touchslider.hpp>

namespace ui::common
{

///////////////////////////////////////////////////////////////////////////////
/// Slider bar widget, with positive/negative values with the 0 position at the
/// center, and the progress drawn from this origin point
///
///////////////////////////////////////////////////////////////////////////////
class CenteredSlider : public TouchSlider
    {
public:

    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  ornt        Orientation (vertical or horizontal)
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    CenteredSlider (Qt::Orientation ornt, QWidget* parent);

protected:
    ///////////////////////////////////////////////////////////////////////////////
    /// Draw the slider bar widget
    ///
    /// @param[in]  event       Paint event
    ///
    ///////////////////////////////////////////////////////////////////////////////
    virtual void paintEvent (QPaintEvent* event) override;

    };

} // namespace ui::common
