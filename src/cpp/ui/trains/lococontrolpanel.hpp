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
    explicit LocoControlPanel (control::ControllerManager&  controllers,
                               vAlignment                   align,
                               QWidget*                     parent);

private:
    control::ControllerManager& m_controllers;      ///< List of controllers being managed
    layout::Locomotive          m_currentLoco;      ///< Current loco under control
    SpeedControlWidget*         m_speed;            ///< Speed control widget
    FunctionPanel*              m_functions;        ///< Function control widget
    common::SchemeComboBox*     m_locos;            ///< Locomotive selector
    ControllerInfo*             m_controllerInfo;   ///< Controller info for selected locomotive
    common::SchemeIconButton*   m_proto;            ///< Track protocol for selected locomotive


    ///////////////////////////////////////////////////////////////////////////////
    /// Add a locomotive to the combobox
    ///
    /// @param[in]  loco        Locomotive to add
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addLocoToCb (const layout::Locomotive& loco);

    ///////////////////////////////////////////////////////////////////////////////
    /// Set the protocol icon
    ///
    /// @param[in]  proto       Protocol to display
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void setProtocol (layout::trackProtocol proto);

private slots:
    ///////////////////////////////////////////////////////////////////////////////
    /// Add a new controller
    ///
    /// @param[in]  controller      Controller being added
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addController (control::ControllerBase& controller);

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

    ///////////////////////////////////////////////////////////////////////////////
    /// Open a dialog to open a new locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void addLoco ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Open a dialog to edit the current locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void editLoco ();

    ///////////////////////////////////////////////////////////////////////////////
    /// Prompt the user to delete the current locomotive
    ///
    ///////////////////////////////////////////////////////////////////////////////
    void deleteLoco ();
    };

} // namespace ui::trains