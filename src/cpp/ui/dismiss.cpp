/**
 * @file        dismiss.cpp
 * @brief       Modal dialog dismiss animation
 * @author      Justin Scott
 * @date        2026-07-31
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/dismiss.hpp>


namespace ui
{

DismissAnimation::DismissAnimation (QDialog& modal) :
    m_dialog (&modal)
    {
    m_effect = new QGraphicsOpacityEffect{ this };
    m_effect->setOpacity (1.0);
    m_dialog->setGraphicsEffect (m_effect);

    connect (m_dialog,
            &QObject::destroyed,
             this,
            &DismissAnimation::dialogDestroyed);
    }

DismissAnimation::~DismissAnimation ()
    {
    if (NULL != m_dialog)
        {
        m_dialog->setGraphicsEffect (NULL);
        }
    }

} // namespace ui
