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

namespace ui::common
{

IconToggle::IconToggle (const QIcon&    active,
                        const QIcon&    disabled,
                        bool            frameless,
                        QWidget*        parent) :
    QPushButton (disabled, "", parent),
    m_active (active),
    m_disabled (disabled)
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

void IconToggle::setIcons (const QIcon& active, const QIcon& disabled)
    {
    m_active = active;
    m_disabled = disabled;

    setIcon (isChecked () ? active : disabled);
    }

void IconToggle::setTooltips (const QString& active, const QString disabled)
    {
    m_activeTip = active;
    m_disabledTip = disabled;

    m_hasTooltips = true;

    setToolTip (isChecked () ? active : disabled);
    }

void IconToggle::enterEvent (QEnterEvent* event)
    {
    setCursor (Qt::PointingHandCursor);
    QAbstractButton::enterEvent (event);
    }

void IconToggle::onToggle ()
    {
    setIcon (isChecked () ? m_active : m_disabled);

    if (m_hasTooltips)
        {
        setToolTip (isChecked () ? m_activeTip : m_disabledTip);
        }
    }

}
