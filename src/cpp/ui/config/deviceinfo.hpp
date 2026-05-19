/**
 * @file        config/deviceinfo.hpp
 * @brief       Defines widgets for entering transport layer device info
 * @author      Justin Scott
 * @date        2026-04-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/common/hostentry.hpp>
#include <ui/common/ipfield.hpp>

#include <utils/device.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QWidget>



namespace ui::config
{
//////////////////////////////////////////////////////////////////////////////
/// Base class for tansport layer device setting entry. Defines interface,
/// not widgets
///
//////////////////////////////////////////////////////////////////////////////
class DeviceInfoWidget : public QWidget
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit DeviceInfoWidget (QWidget* parent) :
        QWidget (parent)
        { setContentsMargins (0, 0, 0, 0); }

    //////////////////////////////////////////////////////////////////////////////
    /// Get the entered device settings
    ///
    /// @return     Device settings
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual utils::device::deviceInfo::info_t getInfo () const = 0;

    //////////////////////////////////////////////////////////////////////////////
    /// Populate the fields with settings from a device info record
    ///
    /// @param[in]  info        Device info
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void setInfo (const utils::device::deviceInfo::info_t& info) = 0;

    ////////////////////////////////////////////////////////////////////////////
    /// Check if the fields have acceptable input
    ///
    /// @return     True if all fields have been filled out correctly
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const = 0;

    ////////////////////////////////////////////////////////////////////////////
    /// If the fields are not filled out correctly, returns a string with information
    /// for the user to correct it
    ///
    /// @return     Error message
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual QString getErrorString () const { return ""; }

signals:
    ////////////////////////////////////////////////////////////////////////////
    /// Signals that the input to one of the fields has changed
    ///
    ////////////////////////////////////////////////////////////////////////////
    void inputChanged ();
    };


//////////////////////////////////////////////////////////////////////////////
/// Entry widget for TCP/IP and UDP/IP device settings
///
//////////////////////////////////////////////////////////////////////////////
class NetworkDeviceInfoWidget : public DeviceInfoWidget
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    /// @param[in]  port        Default TCP/UDP port to use
    ///
    //////////////////////////////////////////////////////////////////////////////
    NetworkDeviceInfoWidget (QWidget* parent, utils::device::portNumber_t port);

    //////////////////////////////////////////////////////////////////////////////
    /// Set the network info
    ///
    /// @param[in]  info        Device network connection info
    ///
    //////////////////////////////////////////////////////////////////////////////
    virtual void setInfo (const utils::device::deviceInfo::info_t& info) override;

    ////////////////////////////////////////////////////////////////////////////
    /// Get the entered network device settings
    ///
    /// @return     Device network connection info
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual utils::device::deviceInfo::info_t getInfo () const override;

    ////////////////////////////////////////////////////////////////////////////
    /// Check if the fields have acceptable input
    ///
    /// @return     True if all fields have been filled out correctly
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override;

    ////////////////////////////////////////////////////////////////////////////
    /// If the fields are not filled out correctly, returns a string with information
    /// for the user to correct it
    ///
    /// @return     Error message
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual QString getErrorString () const override;

private:
    common::IpV4Field*          m_addressV4;        ///< IPv4 entry field
    common::IpV6Field*          m_addressV6;        ///< IPv6 entry field
    common::AbstractHostField*  m_activeIp = NULL;  ///< Active host entry field (IPv4, IPv6, or hostname)
    common::HostEntryField*     m_hostname;         ///< Host name entry field (alternative to IP address)

    QLineEdit*                  m_port;             ///< TCP/UDP port number
    QComboBox*                  m_network;          ///< Network host type slection
                                                    ///  Either IP version selector (IPv4 or IPv6)
                                                    ///  or DNS hostname/FQDN
    QFormLayout*                m_layout;           ///< Main form layout

    ////////////////////////////////////////////////////////////////////////////
    /// Handle a change to the layer 3 (IP) protocol
    ///
    /// @param[in]  idx     Network protocol comobox index
    ///
    ////////////////////////////////////////////////////////////////////////////
    void networkProtoChanged (int idx);

    };


////////////////////////////////////////////////////////////////////////////
/// COM/Serial port entry widget
///
////////////////////////////////////////////////////////////////////////////
class ComPortInfoWidget : public DeviceInfoWidget
    {
public:
    ////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    ////////////////////////////////////////////////////////////////////////////
    explicit ComPortInfoWidget (QWidget* parent);

    ////////////////////////////////////////////////////////////////////////////
    /// Apply COM port settings to the field
    ///
    /// @param[in]  info        Serial device connection info
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual void setInfo (const utils::device::deviceInfo::info_t& info) override;

    ////////////////////////////////////////////////////////////////////////////
    /// Get the entered COM device settings
    ///
    /// @return     Serial device connection info
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual utils::device::deviceInfo::info_t getInfo () const override;

    ////////////////////////////////////////////////////////////////////////////
    /// Check if the fields have acceptable input
    ///
    /// @return     True if all fields have been filled out correctly
    ///
    ////////////////////////////////////////////////////////////////////////////
    virtual bool hasAcceptableInput () const override { return true; }

private:
    QComboBox*      m_comport;  ///< Com port selection, populated with available ports
    QComboBox*      m_baud;     ///< Baud rate selection in bits/second
    QFormLayout*    m_layout;   ///< Main form layout

    };

} // namespace ui::config
