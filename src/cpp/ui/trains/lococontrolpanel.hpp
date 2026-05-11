/**
 * @file        trains/lococontrolpanel.hpp
 * @brief       Control panel for a single locomotive
 * @author      Justin Scott
 * @date        2026-01-25
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/manager.hpp>

#include <ui/trains/speedcontrol.hpp>
#include <ui/trains/functionpanel.hpp>
#include <ui/controllerinfo.hpp>

#include <QComboBox>
#include <QPushButton>

namespace ui::trains
{

///////////////////////////////////////////////////////////////////////////////
/// Panel to control a train
///
///////////////////////////////////////////////////////////////////////////////
class LocoControlPanel : public QWidget
    {
    Q_OBJECT
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  align           Align left or right
    /// @param[in]  parent          Parent widget
    ///
    ///////////////////////////////////////////////////////////////////////////////
    explicit LocoControlPanel (control::ControllerManager*  controllers,
                               vAlignment                   align,
                               QWidget*                     parent);

private:
    layout::Locomotive          m_currentLoco;      ///< Current loco under control
    SpeedControlWidget*         m_speed;            ///< Speed control widget
    FunctionPanel*              m_functions;        ///< Function control widget
    common::SchemeComboBox*     m_locos;            ///< Locomotive selector
    ControllerInfo*             m_controllerInfo;   ///< Controller info for selected locomotive
    common::SchemeIconButton*   m_proto;            ///< Track protocol for selected locomotive

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Add a new controller
    ///
    /// @param[in]  controller      Controller being added
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void add (control::ControllerBase& controller);

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle the user selecting a new locomotive
    ///
    /// @param[in]  idx     Index of new locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void onLocoChange (int idx);

    ///////////////////////////////////////////////////////////////////////////////
    /// Handle a locomotive being deleted from the system
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void locoDeleted ();

    };

} // namespace ui::trains