/**
 * @file        config/networkinfo.hpp
 * @brief       Form to enter network connection info
 * @author      Justin Scott
 * @date        2026-05-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/config/deviceinfo.hpp>

#include <ui/common/hostentry.hpp>
#include <ui/common/ipfield.hpp>
#include <ui/lang.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>

namespace ui::config
{

//////////////////////////////////////////////////////////////////////////////
/// Entry widget for TCP/IP and UDP/IP device settings
///
//////////////////////////////////////////////////////////////////////////////
class NetworkDeviceInfoWidget : public DeviceInfoWidget
    {
    Q_OBJECT
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

    ////////////////////////////////////////////////////////////////////////////
    /// Set the form labels for the widget
    ///
    ////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (DeviceInfoWidget, setLabels ());
    };
} // namespace ui::config