/**
 * @file        common/icontoggle.cpp
 * @brief       Push button that toggles betweens different icons
 * @author      Justin Scott
 * @date        2026-03-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/icontoggle.hpp>

#include <ui/common/utils.hpp>

#include <QApplication>
#include <QStyleHints>

namespace ui::common
{


IconToggle::IconToggle (const utils::resources::Icon&  icon,
                        bool            frameless,
                        QWidget*        parent) :
    PointedIconButton (icon, parent)
    {
    setCheckable (true);

    if (frameless)
        {
        makeFrameless (*this);
        }

    connect (this,
             &QPushButton::toggled,
             this,
             &IconToggle::onToggle);
    }


void IconToggle::setTooltips (const QString& active, const QString disabled)
    {
    m_activeTip     = active;
    m_disabledTip   = disabled;

    m_hasTooltips   = true;

    setToolTip (isChecked () ? active : disabled);
    }


void IconToggle::onToggle ()
    {
    if (m_hasTooltips)
        {
        setToolTip (isChecked () ? m_activeTip : m_disabledTip);
        }
    }

}
