/**
 * @file        sensors/editsensor.cpp
 * @brief       Dialog to create/edit sensors
 * @author      Justin Scott
 * @date        2026-02-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/utils.hpp>
#include <ui/sensors/editsensor.hpp>


namespace ui::sensors
{
EditSensorDialog::EditSensorDialog (const control::ControllerBase&  controller,
                                    QWidget*                        parent,
                                    const layout::SensorModule*     module) :
    common::FormDialog (parent),
    m_edit (NULL != module)
    {
    const auto& [portCounts, _] = controller
                                    .getCapabilityController<layout::SensorController> ()
                                        ->getSensorMetaClass ();

    std::vector<uint> positions = controller.availableSensorPositions ();

    if (NULL != module)
        {
        positions.push_back (module->getPosition ());
        std::sort (positions.begin (), positions.end ());
        }

    QVBoxLayout* layout = new QVBoxLayout{ this };

    m_form          = new QFormLayout{ this };
    m_portCount     = new QComboBox{ this };
    m_modulePosition= new QComboBox{ this };

    m_form->addRow (new QLabel{ this }, m_portCount);
    m_form->addRow (new QLabel{ this }, m_modulePosition);

    layout->addLayout (m_form);
    layout->addWidget (m_buttons);

    common::fillCombobox (*m_portCount,     portCounts);
    common::fillCombobox (*m_modulePosition,positions);

    if (NULL != module)
        {
        common::setComboBoxIndexByUserData (*m_portCount, static_cast<uint> (module->size ()));
        common::setComboBoxIndexByUserData (*m_modulePosition, module->getPosition ());
        }

    setLabels ();
    inputChanged ();

    setLayout (layout);
    }

void EditSensorDialog::setLabels ()
    {
    setWindowTitle (m_edit ? tr ("Edit Sensor Module") : tr ("Add Sensor Module"));

    common::setFormRowText (*m_form, *m_portCount, tr ("Number of Ports"));
    common::setFormRowText (*m_form, *m_modulePosition, tr ("Module Position"));
    }
}

