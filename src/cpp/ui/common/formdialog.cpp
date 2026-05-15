/**
 * @file        common/formdialog.cpp
 * @brief       Dialog box base class for creating forms
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/formdialog.hpp>

#include <QPushButton>


namespace ui::common
{

FormDialog::FormDialog (QWidget* parent) :
    common::SchemeDialog (parent)
    {
    m_buttons = new QDialogButtonBox{ QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this };

    connect (m_buttons,
            &QDialogButtonBox::accepted,
             this,
            &QDialog::accept);

    connect (m_buttons,
            &QDialogButtonBox::rejected,
             this,
            &QDialog::reject);
    }

void FormDialog::enterEvent (QEnterEvent* event)
    {
    QPushButton* btn = m_buttons->button (QDialogButtonBox::Ok);

    if (hasAcceptableInput ())
        {
        btn->setToolTip ("");
        }
    else
        {
        btn->setToolTip (getErrorString ());
        }
    }

void FormDialog::inputChanged ()
    {
    m_buttons->button (QDialogButtonBox::Ok)->setEnabled (hasAcceptableInput ());
    }

} // namespace ui::common