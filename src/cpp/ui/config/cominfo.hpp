/**
 * @file        config/cominfo.hpp
 * @brief       Form to enter serial port connection info
 * @author      Justin Scott
 * @date        2026-05-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <ui/config/deviceinfo.hpp>

#include <QComboBox>
#include <QFormLayout>

namespace ui::config
{

////////////////////////////////////////////////////////////////////////////
/// COM/Serial port entry widget
///
////////////////////////////////////////////////////////////////////////////
class ComPortInfoWidget : public DeviceInfoWidget
    {
    Q_OBJECT
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