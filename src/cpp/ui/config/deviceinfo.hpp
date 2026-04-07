/**
 * @file        config/deviceinfo.hpp
 * @brief       Defines widgets for entering transport layer device info
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/ipfield.hpp>

#include <utils/device.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QWidget>


namespace ui::config
{

class DeviceInfoWidget : public QWidget
    {
    Q_OBJECT
public:
    explicit DeviceInfoWidget (QWidget* parent) :
        QWidget (parent)
        { setContentsMargins (0, 0, 0, 0); }

    virtual utils::device::deviceInfo::info_t getInfo () const = 0;

    virtual void setInfo (utils::device::deviceInfo::info_t&) = 0;

    virtual bool hasAcceptableInput () const = 0;

signals:
    void inputChanged ();
    };


class NetworkDeviceInfoWidget : public DeviceInfoWidget
    {
public:
    NetworkDeviceInfoWidget (QWidget* parent, utils::device::portNumber_t port);

    virtual void setInfo (utils::device::deviceInfo::info_t& info) override;

    virtual utils::device::deviceInfo::info_t getInfo () const override;

    virtual bool hasAcceptableInput () const override
        {
        return m_port->hasAcceptableInput () &&
               m_activeIp->hasAcceptableInput ();
        }

private:
    common::IpField*    m_activeIp = NULL;
    common::IpV4Field*  m_addressV4;
    common::IpV6Field*  m_addressV6;
    QLineEdit*          m_port;
    QComboBox*          m_network;
    QFormLayout*        m_layout;

    void networkProtoChanged (int idx);

    };

class ComPortInfoWidget : public DeviceInfoWidget
    {
public:
    explicit ComPortInfoWidget (QWidget* parent);

    virtual void setInfo (utils::device::deviceInfo::info_t& info) override;

    virtual utils::device::deviceInfo::info_t getInfo () const override;

    virtual bool hasAcceptableInput () const override { return true; }

private:
    QComboBox*      m_comport;
    QComboBox*      m_baud;
    QFormLayout*    m_layout;

    };


}
