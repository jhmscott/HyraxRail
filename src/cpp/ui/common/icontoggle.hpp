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

namespace ui::common
{

class IconToggle : public QPushButton
    {
public:
    IconToggle (const QIcon&    active,
                const QIcon&    disabled,
                bool            frameless,
                QWidget*        parent);

    void setIcons (const QIcon& active, const QIcon& disabled);

    void setTooltips (const QString& active, const QString disabled);

protected:

    void enterEvent (QEnterEvent* event) override;

private:
    QIcon   m_active;
    QIcon   m_disabled;

    QString m_activeTip;
    QString m_disabledTip;

    bool    m_hasTooltips;

    void onToggle ();
    };
}