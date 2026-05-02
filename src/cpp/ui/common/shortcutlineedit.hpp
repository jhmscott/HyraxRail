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

//////////////////////////////////////////////////////////////////////////////
/// Simple line to visually seprate sections/widgets with back and forward
/// signals, specified by a specific character shortcut
///
//////////////////////////////////////////////////////////////////////////////
class ShortcutLineEdit : public QLineEdit
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  shorcut     Shortcut character to move to next widget
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ShortcutLineEdit (QChar shortcut, QWidget* parent) :
        QLineEdit (parent),
        m_shortcut (shortcut)
        {}

signals:
    //////////////////////////////////////////////////////////////////////////////
    /// Signals the user has requested to move to the next widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    void next ();

    //////////////////////////////////////////////////////////////////////////////
    /// Signals the user has requested to move to the previous widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    void back ();

protected:
    //////////////////////////////////////////////////////////////////////////////
    /// Handle a key press event
    ///
    /// @param[in]  event       Key press event
    ///
    //////////////////////////////////////////////////////////////////////////////
    void keyPressEvent (QKeyEvent* event);

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a key release event
    ///
    /// @param[in]  event       Key release event
    ///
    //////////////////////////////////////////////////////////////////////////////
    void keyReleaseEvent (QKeyEvent* event);

private:
    QChar   m_shortcut;         ///< Next widget shortcut character
    bool    m_empty = false;    ///< True if line edit is empty
    };
}