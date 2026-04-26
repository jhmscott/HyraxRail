/**
 * @file        common/hovericon.hpp
 * @brief       Frameless pushbutton with an icon, that animates with a
 *              shadow effect on mouse hovers
 * @author      Justin Scott
 * @date        2026-04-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/common/pointedwidget.hpp>

#include <QPropertyAnimation>
#include <QPushButton>

namespace ui::common
{

//////////////////////////////////////////////////////////////////////////////
/// Icon button with a shadow effect that is added on mouse over
///
//////////////////////////////////////////////////////////////////////////////
class HoverIcon : public PointedButton
    {
    Q_OBJECT

    Q_PROPERTY (uint opacity READ getOpacity WRITE setOpacity);
public:
    // Shadow style
    enum class style
        {
        CIRCLE,         ///< Circle
        ROUNDED_RECT    ///< Rect with 2 pixel rounded corners
        };

    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  icon        Icon to display
    /// @param[in]  hoverColor  Color of the shadow added on hover
    /// @param[in]  style       Shadow style
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    HoverIcon (const QIcon& icon, const QColor& hoverColor, style style, QWidget* parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Get the opacity of the shadow
    ///
    /// @return     shadow opacity
    ///
    //////////////////////////////////////////////////////////////////////////////
    uint getOpacity () const { return m_opacity; }

    //////////////////////////////////////////////////////////////////////////////
    /// Set the shadow opacity
    ///
    /// @param[in]  opacity     New opacity value
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setOpacity (uint opacity) { m_opacity = opacity; update (); }

protected:
    //////////////////////////////////////////////////////////////////////////////
    /// Handles widget painting
    ///
    /// @param[in,out]  event       Paint event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void paintEvent (QPaintEvent* event) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Handles the mouse enter evenet
    ///
    /// @param[in,out]  event       Mouse enter event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void enterEvent (QEnterEvent* event) override;

    //////////////////////////////////////////////////////////////////////////////
    /// Handles the mouse leave evenet
    ///
    /// @param[in,out]  event       Mouse leave event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void leaveEvent (QEvent* event) override;
private:
    QPropertyAnimation* m_anim;         ///< Mouse over animation
    QColor              m_hoverColor;   ///< Mouse over shadow colour
    style               m_style;        ///< Mouse over shadow style
    uint                m_opacity = 0;  ///< Mouse over shadow opacity
    };

}