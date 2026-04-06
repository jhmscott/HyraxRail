/**
 * @file        common/stringdialog.cpp
 * @brief       Simple string entry dialog
 * @author      Justin Scott
 * @date        2026-03-24
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/stringdialog.hpp>

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QValidator>

namespace ui::common
{
StringDialog::StringDialog (QWidget* parent) :
    QDialog (parent),
    m_edit (new QLineEdit{ this })
    {
    QVBoxLayout*        layout  = new QVBoxLayout{ this };
    QDialogButtonBox*   buttons = new QDialogButtonBox{ QDialogButtonBox::Ok |
                                                        QDialogButtonBox::Cancel,
                                                        this };

    layout->addWidget (m_edit);
    layout->addWidget (buttons);

    m_ok = buttons->button (QDialogButtonBox::Ok);

    connect (buttons,
            &QDialogButtonBox::accepted,
             this,
            &QDialog::accept);

    connect (buttons,
            &QDialogButtonBox::rejected,
             this,
            &QDialog::reject);

    setLayout (layout);
    }

void StringDialog::setValidator (QValidator* validator)
    {
    if (NULL != m_validator)
        {
        delete m_validator;
        }

    m_validator = validator;
    m_edit->setValidator (m_validator);

    if (NULL == m_validator)
        {
        disconnect (m_edit,
                   &QLineEdit::textChanged,
                    this,
                   &StringDialog::validityChanged);
        }
    else
        {
        m_validator->setParent (this);

        connect (m_edit,
                &QLineEdit::textChanged,
                 this,
                &StringDialog::validityChanged);
        }

    validityChanged (m_edit->text ());
    }

void StringDialog::validityChanged (const QString& text)
    {
    m_ok->setEnabled (m_edit->hasAcceptableInput ());
    }
}
