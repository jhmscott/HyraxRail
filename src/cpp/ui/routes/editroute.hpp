/**
 * @file        routes/editroute.hpp
 * @brief       Dialog box to edit/add routes
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>

#include <ui/common/formdialog.hpp>
#include <ui/routes/includebutton.hpp>

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>


namespace ui::routes
{
class EditRouteDialog : public common::FormDialog
    {
public:
    EditRouteDialog (control::ControllerBase& controller, QWidget* parent);

    std::vector<control::routeMember> getActuators () const;

    std::string getName () const { return m_name->text ().toStdString (); }
private:
    std::vector<ActuatorIncludeButton*> m_buttonList;
    QLineEdit*                          m_name;

    virtual bool hasAcceptableInput () const override;
    };

}