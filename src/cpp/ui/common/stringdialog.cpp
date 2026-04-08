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
    FormDialog (parent),
    m_edit (new QLineEdit{ this })
    {
    QVBoxLayout*        layout  = new QVBoxLayout{ this };

    layout->addWidget (m_edit);
    layout->addWidget (m_buttons);

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
                   &FormDialog::inputChanged);
        }
    else
        {
        m_validator->setParent (this);

        connect (m_edit,
                &QLineEdit::textChanged,
                 this,
                &FormDialog::inputChanged);
        }

    inputChanged ();
    }

}
