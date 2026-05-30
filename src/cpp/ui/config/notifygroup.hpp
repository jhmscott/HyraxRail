/**
 * @file        config/notifygroup.hpp
 * @brief       Group box for the configuration of notifications
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <ui/lang.hpp>

#include <QGroupBox>
#include <QLabel>

namespace ui::config
{

//////////////////////////////////////////////////////////////////////////////
/// Group box for notification settings
///
//////////////////////////////////////////////////////////////////////////////
class NotifyGroup : public QGroupBox
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit NotifyGroup (QWidget* parent);

private:
    QLabel* m_stopLabel;    ///< Stop toggle label
    QLabel* m_sensorLabel;  ///< Sensor toggle label

    //////////////////////////////////////////////////////////////////////////////
    /// Set all the widget label text
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (QGroupBox, setLabels ());
    };

} // ui::config