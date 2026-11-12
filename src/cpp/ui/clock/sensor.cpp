/**
 * @file        automation/sensor.cpp
 * @brief       Form fields for sensor automation condition
 * @author      Justin Scott
 * @date        2026-11-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <names.hpp>

#include <ui/clock/sensor.hpp>

#include <ui/common/utils.hpp>

#include <QFormLayout>

namespace ui::clock
{

SensorConditionForm::SensorConditionForm (control::ControllerManager& controllers, QWidget* parent) :
    ConditionForm (parent)
    {
    m_layout = new QFormLayout{ this };

    m_modules       = new common::TieredDropdown{ this };
    m_ports         = new QComboBox{ this };
    m_conditions    = new QComboBox{ this };

    m_modules   ->setObjectName (OBJNAME_SENSOR_CONDFORM_MODULES);
    m_ports     ->setObjectName (OBJNAME_SENSOR_CONDFORM_PORTS);
    m_conditions->setObjectName (OBJNAME_SENSOR_CONDFORM_CONDITIONS);

    m_layout->addRow (new QLabel{ this }, m_modules);
    m_layout->addRow (new QLabel{ this }, m_ports);
    m_layout->addRow (new QLabel{ this }, m_conditions);

    for (const control::ControllerBase& controller : controllers)
        {
        if (controller.getMetaClass ().hasCapability (control::CAPABILITY_SENSOR))
            {
            m_modules->addParentItem (controller.getFriendlyName ().c_str ());

            for (const layout::SensorModule& sensor : controller.getSensors ())
                {
                m_modules->addChildItem (sensor.getName ().c_str (),
                                         QVariant::fromValue (sensor));
                }
            }
        }

    fillPorts ();
    fillConditions ();
    setFormLabels ();

    connect (m_modules,
            &QComboBox::currentIndexChanged,
             this,
            &SensorConditionForm::onModuleChange);

    setLayout (m_layout);
    }

control::AutomationCondition* SensorConditionForm::getCondition () const
    {
    auto sensor     = m_modules->currentData ().value<layout::SensorModule> ();
    auto condition  = m_conditions->currentData ().value<control::AutomationSensor::condition> ();
    int  port       = m_ports->currentData ().toInt ();

    return new control::AutomationSensor{ std::move (sensor),
                                          condition,
                                          static_cast<uint8_t> (port) };
    }

void SensorConditionForm::setCondition (const control::AutomationCondition& condition)
    {
    const control::AutomationSensor& sensor = static_cast<const control::AutomationSensor&> (condition);

    common::setComboBoxIndexByUserData (*m_modules,     sensor.getModule ());
    common::setComboBoxIndexByUserData (*m_ports,       sensor.getPort ());
    common::setComboBoxIndexByUserData (*m_conditions,  sensor.getType ());
    }

void SensorConditionForm::onModuleChange ()
    {
    common::refreshComboboxItems (*m_ports,
                                  std::bind (&SensorConditionForm::fillPorts, this));

    emit inputChanged ();
    }

void SensorConditionForm::fillPorts ()
    {
    QVariant var = m_modules->currentData ();

    if (var.isValid ())
        {
        layout::SensorModule sensor = var.value<layout::SensorModule> ();

        for (size_t ii = 0; ii < sensor.size (); ++ii)
            {
            m_ports->addItem (tr ("Port %1").arg (ii), ii);
            }
        }
    }

void SensorConditionForm::fillConditions ()
    {
    utils::algorithm::EnumRange range{ control::AutomationSensor::NUM_CONDITIONS };

    for (control::AutomationSensor::condition cond : range)
        {
        m_conditions->addItem (control::sensorConditionText (cond), cond);
        }
    }

void SensorConditionForm::updateTexts ()
    {
    common::refreshComboboxItems (*m_ports,
                                  std::bind (&SensorConditionForm::fillPorts, this));

    common::refreshComboboxItems (*m_conditions,
                                  std::bind (&SensorConditionForm::fillConditions, this));

    setFormLabels ();
    }

void SensorConditionForm::setFormLabels ()
    {
    common::setFormRowText (*m_layout, *m_modules,   tr ("Module"));
    common::setFormRowText (*m_layout, *m_ports,     tr ("Port"));
    common::setFormRowText (*m_layout, *m_conditions,tr ("Condition"));
    }

} // namespace ui::clock
