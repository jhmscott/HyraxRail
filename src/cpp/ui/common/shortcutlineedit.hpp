/**
 * @file        common/shortcutlineedit.hpp
 * @brief       Simple line to visually seprate sections/widgets with back and
 *              forward signals, specified by a specific character shortcut
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QLineEdit>

namespace ui::common
{

class ShortcutLineEdit : public QLineEdit
    {
    Q_OBJECT
public:
    ShortcutLineEdit (QChar shortcut, QWidget* parent) :
        QLineEdit (parent),
        m_shortcut (shortcut)
        {}

signals:
    void next ();

    void back ();

protected:
    void keyPressEvent (QKeyEvent* event);

    void keyReleaseEvent (QKeyEvent* event);
private:
    QChar   m_shortcut;
    bool    m_empty = false;
    };
}