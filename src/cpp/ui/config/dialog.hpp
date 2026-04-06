/**
 * @file        config/dialog.hpp
 * @brief       Dialog box for the configuration of controllers
 * @author      Justin Scott
 * @date        2026-02-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>

#include <control/controllers/base.hpp>

#include <ui/common/optionaldropdown.hpp>
#include <ui/common/ipfield.hpp>

namespace ui::config
{

class DeviceInfoWidget;

class Dialog : public QDialog
    {
public:
    Dialog (QWidget* parent, control::ControllerBase* controller = NULL);

    control::createControllerInfo createController () const;
private:
    common::OptionalDropdown*   m_controller;
    common::OptionalDropdown*   m_protocol;
    common::OptionalDropdown*   m_transport;
    DeviceInfoWidget*           m_network;
    DeviceInfoWidget*           m_com;
    DeviceInfoWidget*           m_active;
    QLineEdit*                  m_name;
    QFormLayout*                m_layout;

    void setTransportProto (int idx);

    void setNetworkMode ();

    void setComMode ();
    };

}