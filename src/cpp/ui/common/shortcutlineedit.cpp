/**
 * @file        common/shortcutlineedit.cpp
 * @brief       Simple line to visually seprate sections/widgets with back and
 *              forward signals, specified by a specific character shortcut
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/common/shortcutlineedit.hpp>

#include <qevent.h>

namespace ui::common
{


void ShortcutLineEdit::keyPressEvent (QKeyEvent* event)
    {
    if (event->key () == m_shortcut.unicode ())
        {
        event->accept ();
        }
    else
        {
        if (Qt::Key_Backspace == event->key () && text ().isEmpty ())
            {
            m_empty = true;
            }

        QLineEdit::keyPressEvent (event);
        }
    }

void ShortcutLineEdit::keyReleaseEvent (QKeyEvent* event)
    {
    if (event->key () == m_shortcut.unicode ())
        {
        event->accept ();
        emit next ();
        }
    else if (m_empty)
        {
        m_empty = false;

        event->accept ();
        emit back ();
        }
    }
}