/**
 * @file        common/formdialog.hpp
 * @brief       Dialog box base class for creating forms
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/schemeicon.hpp>

#include <QDialog>
#include <QDialogButtonBox>

namespace ui::common
{

//////////////////////////////////////////////////////////////////////////////
/// Base class for creating a dialog box containing a form. This has an OK
/// and Cancel button box, and OK is automatically disabeld until all fields
/// have acceptable input
///
//////////////////////////////////////////////////////////////////////////////
class FormDialog : public SchemeDialog
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    FormDialog (QWidget* parent);

public slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Handle a change in a widget field. Connect the input change signals
    /// (e.g. textChanged) from your fields to this slot
    ///
    //////////////////////////////////////////////////////////////////////////////
    void inputChanged ();

protected:
    QDialogButtonBox* m_buttons;    ///< OK/Cancel Button box
                                    ///  Derived classes are responsible for
                                    ///  adding this to their central layout

    //////////////////////////////////////////////////////////////////////////////
    /// Handle the mouse enter event
    ///
    /// @param[in]  event       Mouse enter event
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void enterEvent (QEnterEvent* event) override;
private:
    //////////////////////////////////////////////////////////////////////////////
    /// Virtual function to check if all fields in the derived class have
    /// acceptable input. If it returns true, the "OK" button will be enabled
    ///
    /// @return     True if all fields in the form have acceptable input
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const = 0;

    //////////////////////////////////////////////////////////////////////////////
    /// Get the message to display to the user to explain why the input is invalid
    ///
    /// @return     Error string
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual QString getErrorString () const { return ""; }

    };
} // namespace ui::common