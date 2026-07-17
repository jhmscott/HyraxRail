/**
 * @file        config/cominfo.cpp
 * @brief       Form to enter serial port connection info
 * @author      Justin Scott
 * @date        2026-05-20
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/config/cominfo.hpp>
#include <ui/common/utils.hpp>


namespace ui::config
{


ComPortInfoWidget::ComPortInfoWidget (QWidget* parent) :
    DeviceInfoWidget (parent)
    {
    m_layout    = new QFormLayout{ this };

    m_comport   = new QComboBox{ this };
    m_baud      = new QComboBox{ this };

    common::refreshStyleSheetOnColorSchemeChange (*m_comport);
    common::refreshStyleSheetOnColorSchemeChange (*m_baud);

    for (auto& comPort : QSerialPortInfo::availablePorts ())
        {
        m_comport->addItem (comPort.portName (),
                            QVariant::fromValue (std::move (comPort)));
        }

    for (int baud : QSerialPortInfo::standardBaudRates ())
        {
        m_baud->addItem (QString::number (baud),
                         QVariant::fromValue (baud));
        }

    m_layout->addRow (new QLabel{ this }, m_baud);
    m_layout->addRow (new QLabel{ this }, m_comport);

    m_layout->setContentsMargins (0, 0, 0, 0);

    setLabels ();

    connect (m_baud,
            &QComboBox::currentIndexChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    connect (m_comport,
            &QComboBox::currentIndexChanged,
             this,
            &DeviceInfoWidget::inputChanged);

    setLayout (m_layout);
    }

void ComPortInfoWidget::setInfo (const utils::device::deviceInfo::info_t& info)
    {
    const utils::device::serialInfo& com = std::get<utils::device::serialInfo> (info);

    common::setComboBoxIndexByText (*m_comport, com.port.portName ());
    common::setComboBoxIndexByUserData (*m_baud, com.baud);
    }

utils::device::deviceInfo::info_t ComPortInfoWidget::getInfo () const
    {
    utils::device::serialInfo info;

    info.baud = m_baud->currentData ().toInt ();
    info.port = m_comport->currentData ().value<QSerialPortInfo> ();

    return info;
    }

void ComPortInfoWidget::setLabels ()
    {
    common::setFormRowText (*m_layout, *m_baud,     tr ("Baud Rate"));
    common::setFormRowText (*m_layout, *m_comport,  tr ("COM Port"));
    }

} // namespace ui::config