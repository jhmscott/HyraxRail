/**
 * @file        sensors/sensorbar.spp
 * @brief       Bar representing the state of sensors for a given sensor module
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/sensors/editsensor.hpp>
#include <ui/sensors/sensorbar.hpp>

#include <QLabel>

namespace ui::sensors
{
SensorBar::SensorBar (control::AutomationManager&   automations,
                      control::ControllerBase&      controller,
                      const layout::SensorModule&   module,
                      QWidget*                      parent) :
    common::ComponentButton<layout::SensorModule> (&automations, module, true, parent),
    m_module (module),
    m_automations (automations),
    m_controller (controller)
    {
    QVBoxLayout* layout = new QVBoxLayout{ this };
    QHBoxLayout* states = new QHBoxLayout{ this };

    m_states.reserve (m_module.size ());

    for (size_t ii = 0; ii < m_module.size (); ++ii)
        {
        states->addWidget (
            m_states.emplace_back (
                new SensorManageState{ m_module, ii, 5, this}));
        }

    setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addLayout (states);
    layout->addWidget (m_name = new QLabel{ module.getName ().c_str (), this },
                       0,
                       Qt::AlignHCenter);

    setLayout (layout);
    }

void SensorBar::editComponent ()
    {
    EditSensorDialog dlg{ m_controller, this, &m_module };

    if (QDialog::Accepted == dlg.exec ())
        {
        // This gets a bit messy, since we don't want to remove our automations
        // But changing the position changes the ID
        // So as a work around we need to save and rebuild our automations that use this
        //

        auto automations = m_automations.findAutomatedComponents (m_module);
        std::vector<std::pair<control::AutomationSensor::condition, uint8_t>> conditionParams;

        conditionParams.reserve (automations.size ());

        std::transform (automations.begin (),
                        automations.end (),
                        std::back_inserter (conditionParams),
            [] (std::reference_wrapper<const control::AutomationTask> taskRef) ->
            std::pair<control::AutomationSensor::condition, uint8_t>
            {
            control::AutomationTask& task           =
                const_cast<control::AutomationTask&> (taskRef.get ());
            control::AutomationCondition* condition = task.getCondition ();

            Q_CHECK_PTR (condition);
            Q_ASSERT (condition->getType () == control::AutomationCondition::TYPE_SENSOR);

            auto* sensor = static_cast<control::AutomationSensor*> (task.getCondition ());

            auto pair = std::make_pair (sensor->getCondition (),
                                        sensor->getPort ());

            task.setCondition (NULL);

            return pair;
            });

        m_module.remove ();

        m_module = m_controller.createSensor (dlg.getPortCount (),
                                              dlg.getModulePosition ());
        m_name->setText (m_module.getName ().c_str ());

        for (size_t ii = 0; ii < automations.size (); ++ii)
            {
            auto& [type, port] = conditionParams[ii];

            const_cast<control::AutomationTask&> (automations[ii].get ())
                .setCondition (new control::AutomationSensor{ m_module, type, port });
            }
        }
    }

} // namespace ui::sensors
