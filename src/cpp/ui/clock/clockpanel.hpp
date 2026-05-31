/**
 * @file        clock/clockpanel.hpp
 * @brief       Panel for the fast clock/automation configuration tab
 * @author      Justin Scott
 * @date        2026-05-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/lang.hpp>

#include <QGroupBox>
#include <QWidget>

namespace ui::clock
{

///////////////////////////////////////////////////////////////////////////////
/// Main widget for the fast clock/automation configuration tab
///
///////////////////////////////////////////////////////////////////////////////
class ClockPanel : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit ClockPanel (QWidget* parent);

private:
    QGroupBox* m_clockBox;  ///< Group box containing the fast clock and it's settings

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the group box titles
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTitles ();

    UILANG_ON_CHANGE (QWidget, setTitles ());
    };

} // namespace ui::clock
