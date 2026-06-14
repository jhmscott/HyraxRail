/**
 * @file        common/duration.hpp
 * @brief       Widget to enter a duration of time
 * @author      Justin Scott
 * @date        2026-06-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/lang.hpp>

#include <utils/time.hpp>

#include <QComboBox>
#include <QSpinBox>
#include <QWidget>

#include <chrono>


namespace ui::common
{

///////////////////////////////////////////////////////////////////////////////
/// Widget to enter a duration of time
///
///////////////////////////////////////////////////////////////////////////////
class DurationField : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  untis       List of units to include in the combobox
    /// @param[in]  parent      Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    DurationField (const utils::time::units& units, QWidget* parent);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the duration
    ///
    /// @param[in]  duration        Duration of time
    /// @param[in]  updateUnits     If true, update the units to match the duration
    ///                             If false, keep the existing units
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setDuration (const std::chrono::nanoseconds& duration, bool updateUnits);

    ///////////////////////////////////////////////////////////////////////////////
    /// Get the user entered duration
    ///
    /// @return     Time duration
    ///
    ///////////////////////////////////////////////////////////////////////////////
    std::chrono::nanoseconds getDuration () const;

signals:
    void inputChanged ();

private:
    QSpinBox*           m_num;              ///< Number of <m_units> time
    QComboBox*          m_unit;             ///< Unit combobox
    utils::time::units  m_supportedUnits;   ///< List of supported units

    ///////////////////////////////////////////////////////////////////////////////
    /// Update the unit labels
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setUnitLabels ();

    UILANG_ON_CHANGE (QWidget, setUnitLabels ());
    };

} // namespace ui::common
