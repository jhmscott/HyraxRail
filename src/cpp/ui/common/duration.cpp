/**
 * @file        common/duration.hpp
 * @brief       Widget to enter a duration of time
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#include <ui/common/duration.hpp>
#include <ui/common/utils.hpp>

#include <utils/algorithm.hpp>

#include <QBoxLayout>


namespace ui::common
{

DurationField::DurationField (const utils::time::units& units, QWidget* parent) :
    QWidget (parent)
    {
    QHBoxLayout* layout = new QHBoxLayout{ this };

    m_num   = new QSpinBox{ this };
    m_unit = new QComboBox{ this };

    for (size_t ii : utils::algorithm::bitsetToSet (units))
        {
        utils::time::unit unit = static_cast<utils::time::unit> (ii);

        m_unit->addItem (utils::time::unitLabel (unit),
                          QVariant::fromValue (unit));
        }

    m_unit->setSizePolicy (QSizePolicy::Maximum, QSizePolicy::Maximum);

    layout->addWidget (m_num);
    layout->addWidget (m_unit);
    layout->setContentsMargins (0, 0, 0, 0);

    connect (m_num,
            &QSpinBox::valueChanged,
             this,
            &DurationField::inputChanged);

    connect (m_unit,
            &QComboBox::currentIndexChanged,
             this,
            &DurationField::inputChanged);

    setContentsMargins (0, 0, 0, 0);
    setLayout (layout);
    }

void DurationField::setDuration (const std::chrono::nanoseconds& duration, bool updateUnits)
    {
    if (updateUnits)
        {
        auto [count, unit] = utils::time::convertToBestUnit (duration,
                                                             m_supportedUnits);

        m_num->setValue (count);
        setComboBoxIndexByUserData (*m_unit, unit);
        }
    else
        {
        auto unit       = m_unit->currentData ().value<utils::time::unit> ();
        int newCount    = duration.count () / utils::time::unitConversion (unit).count ();

        m_num->setValue (newCount);
        }
    }

std::chrono::nanoseconds DurationField::getDuration () const
    {
    return m_num->value () *
           utils::time::unitConversion (
               m_unit->currentData ().value<utils::time::unit> ());
    }


void DurationField::setUnitLabels ()
    {
    for (int ii = 0; ii < m_unit->count (); ++ii)
        {
        m_unit->setItemText (ii,
                              utils::time::unitLabel (
                                  m_unit->itemData (ii).value<utils::time::unit> ()));
        }
    }

} // namespace ui::common