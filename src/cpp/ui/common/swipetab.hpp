/**
 * @file        common/swipetab.hpp
 * @brief       Widget that combines a tab bar and swipe view,
                to allow swiping between tabs
 * @author      Justin Scott
 * @date        2026-10-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/schemeicon.hpp>
#include <ui/common/swipeview.hpp>

#include <QBoxLayout>

namespace ui::common
{
///////////////////////////////////////////////////////////////////////////////
/// Widget that combines a tab bar and swipe view, to allow swiping between tabs
///
///////////////////////////////////////////////////////////////////////////////
class SwipeTabWidget : public QWidget
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit SwipeTabWidget (QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Add a tab with an icon
    ///
    /// @param[in]  widget      Central widget for tab
    /// @param[in]  icon        Icon for tab
    ///
    /// @return     Index of added tab
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int addTab (QWidget* widget, const utils::resources::Icon& icon);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the tab bar widget
    ///
    /// @return     Tab bar widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QTabBar* tabBar () const { return m_tabs; }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the size of the icons used in the tabs
    ///
    /// @param[in]  size    Icon size
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setIconSize (const QSize& size) { m_tabs->setIconSize (size); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Enable/disable tab scroll buttons
    ///
    /// @param[in]  use     True to enable, false to disable
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setUsesScrollButtons (bool use) { m_tabs->setUsesScrollButtons (use); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the tooltip for a given tab
    ///
    /// @param[in]  idx     Index of tab to set tooltip for
    /// @param[in]  text    Tooltip text
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTabToolTip (int idx, const QString& text) { m_tabs->setTabToolTip (idx, text); }

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the position of the tabs
    ///
    /// @param[in]  pos     Tab bar position
    ///
    /// @remarks    QTabWidget::East and QTabWidget::West not working yet
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setTabPosition (QTabWidget::TabPosition pos);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the number of tabs
    ///
    ///////////////////////////////////////////////////////////////////////////////
    int count () const { return m_tabs->count (); }

private:
    QVBoxLayout*    m_vbox; ///< Vertical box layout (for North/South tabs)
    QHBoxLayout*    m_hbox; ///< Horizontal box layout (for East/West tabs)
    SchemeTabBar*   m_tabs; ///< Tab bar
    SwipeView*      m_view; ///< Swipe view
    };

} // namespace ui::common
