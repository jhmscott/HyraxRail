/**
 * @file        common/icontoggle.hpp
 * @brief       Push button that toggles betweens different icons
 * @author      Justin Scott
 * @date        2026-01-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QPushButton>

#include <ui/common/pointedwidget.hpp>

namespace ui::common
{

//////////////////////////////////////////////////////////////////////////////
/// Push button that has different icons for each toggle state
///
//////////////////////////////////////////////////////////////////////////////
class IconToggle : public PointedButton
    {
public:

    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  active      Active state icon
    /// @param[in]  disabled    Disabled state icon
    /// @param[in]  frameless   If true, render this without a frame
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    IconToggle (const QIcon&    active,
                const QIcon&    disabled,
                bool            frameless,
                QWidget*        parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Set the icons
    ///
    /// @param[in]  active      Active state icon
    /// @param[in]  disabled    Disabled state icon
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setIcons (const QIcon& active, const QIcon& disabled);

    //////////////////////////////////////////////////////////////////////////////
    /// Set the tooltips
    ///
    /// @param[in]  active      Active state tootlip
    /// @param[in]  disabled    Disabled state tooltip
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setTooltips (const QString& active, const QString disabled);


private:
    QIcon   m_active;               ///< Active state icon
    QIcon   m_disabled;             ///< Disabled state icon

    QString m_activeTip;            ///< Active state tooltip
    QString m_disabledTip;          ///< Disabled state tooltip

    bool    m_hasTooltips = false;  ///< True if tooltips are enabled

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Called when the button state toggles
    ///
    //////////////////////////////////////////////////////////////////////////////
    void onToggle ();
    };

} // namespace ui::common