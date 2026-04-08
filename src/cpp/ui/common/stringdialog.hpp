/**
 * @file        common/stringdialog.hpp
 * @brief       Simple string entry dialog
 * @author      Justin Scott
 * @date        2026-03-24
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/formdialog.hpp>

#include <QDialog>
#include <QLineEdit>

namespace ui::common
{

class StringDialog : public FormDialog
    {
public:
    StringDialog (QWidget* parent);

    QString getText () const { return m_edit->text (); }

    void setText (const QString& text) { m_edit->setText (text); }

    void setPlaceholder (const QString& text) { m_edit->setPlaceholderText (text); }

    void setValidator (QValidator* validator);
private:
    QLineEdit*  m_edit;
    QValidator* m_validator = NULL;

    virtual bool hasAcceptableInput () const override { return m_edit->hasAcceptableInput (); }
    };
}
