/**
 * @file        common/shortcutlineedit.hpp
 * @brief       Line edit with shortcut and signals for moving between adjacent widgets
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
/// Line edit with shortcut and signals for moving between adjacent widgets
///
//////////////////////////////////////////////////////////////////////////////
class ShortcutLineEdit : public QLineEdit
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  shortcut    Shortcut character to move to next widget
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ShortcutLineEdit (QChar shortcut, QWidget* parent) :
        QLineEdit (parent),
        m_shortcut (shortcut)
        {}

    //////////////////////////////////////////////////////////////////////////////
    /// Set a custom paste handler
    ///
    /// @tparam     Func        Paste handler function type
    ///
    /// @param[in]  receiver    Object to receive the paste event
    /// @param[in]  slot        Paste handler slot
    ///
    //////////////////////////////////////////////////////////////////////////////
    template<class Func>
    void setPasteHandler (QObject* receiver, Func slot)
        {
        connect (this,
                &ShortcutLineEdit::pasteSpecial,
                 receiver,
                 slot);
        m_overridePaste = true;
        }

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

    //////////////////////////////////////////////////////////////////////////////
    /// Custom paste signal
    ///
    //////////////////////////////////////////////////////////////////////////////
    void pasteSpecial ();

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
    QChar   m_shortcut;                 ///< Next widget shortcut character
    bool    m_empty         = false;    ///< True if line edit is empty
    bool    m_overridePaste = false;    ///< True if the paste signal has been overriden
    };
}