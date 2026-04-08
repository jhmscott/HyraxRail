/**
 * @file        common/formdialog.hpp
 * @brief       Dialog box base class for creating forms
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QDialog>
#include <QDialogButtonBox>

namespace ui::common
{

class FormDialog : public QDialog
    {
public:
    FormDialog (QWidget* parent);

public slots:
    void inputChanged ();

protected:
    QDialogButtonBox* m_buttons;

private:
    virtual bool hasAcceptableInput () const = 0;

    };
}