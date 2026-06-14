/**
 * @file        config/controllergroup.hpp
 * @brief       Group box for the controller settings
 * @author      Justin Scott
 * @date        2026-03-13
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/addbtn.hpp>
#include <ui/lang.hpp>

#include <QGroupBox>
#include <QWidget>

namespace ui::config
{

//////////////////////////////////////////////////////////////////////////////
/// Group box for controller setting
///
//////////////////////////////////////////////////////////////////////////////
class ControllerGroup : public QGroupBox
    {
    Q_OBJECT
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    ControllerGroup (control::ControllerManager&    controllers,
                     QWidget*                       parent);

private:
    control::ControllerManager& m_controllers;              ///< List of controllers
    common::AddButton*          m_plusLabel;                ///< Label for the plus/add button
    bool                        m_hasAddController = false; ///< Set once the controller
                                                            ///  add widget has been added

    //////////////////////////////////////////////////////////////////////////////
    /// Open dialog to add a new controller
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addController ();

    //////////////////////////////////////////////////////////////////////////////
    /// Set the text labels for the widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (QGroupBox, setLabels ());
private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Handle a controller being deleted
    ///
    /// @param[in]  controller      Controller being deleted
    ///
    //////////////////////////////////////////////////////////////////////////////
    void onControllerDeleted (const control::ControllerBase& controller);

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a new controller being added
    ///
    /// @param[in]  controller      Controller being added
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addControllerInfo (control::ControllerBase& controller);
    };

} // ui::config