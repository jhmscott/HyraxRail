/**
 * @file        trains/functionpanel.hpp
 * @brief       Panel containing a grid of function buttons for
 *              a given locomotive
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/locomotive.hpp>

#include <QBoxLayout>
#include <QPushButton>
#include <QWidget>

#include <common.hpp>

namespace ui::trains
{

class FunctionButton;


///////////////////////////////////////////////////////////////////////////////
/// Control panel for train functions
///
///////////////////////////////////////////////////////////////////////////////
class FunctionPanel : public QWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  align       Align left or right
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    FunctionPanel (vAlignment align, QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the locomotive to control the functions for
    ///
    /// @param[in]  loco    Locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setLocomotive (const layout::Locomotive& loco);

    ///////////////////////////////////////////////////////////////////////////////
    /// Clear the current locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void clear ();
private:
    layout::Locomotive              m_loco; ///< Locomotive being controlled
    std::array<FunctionButton*, 32> m_btns; ///< Function control buttons

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a function button being pressed
    ///
    /// @param[in]  id          Function ID
    /// @param[in]  enabled     Function state
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void buttonPressed (uint8_t id, bool enabled)
        { m_loco.setFunc (id, enabled); }
    };

} // namespace ui::trains