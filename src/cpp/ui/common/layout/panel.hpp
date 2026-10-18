/**
 * @file        layout/panel.hpp
 * @brief       Panel class for layout components
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/controllers/base.hpp>
#include <control/controllers/manager.hpp>

#include <ui/common/autogrid.hpp>

#include <ui/common/layout/delegate.hpp>
#include <ui/common/layout/group.hpp>

#include <QPushButton>
#include <QScrollArea>
#include <QWidget>


namespace ui::common
{

//////////////////////////////////////////////////////////////////////////////
/// Panel class for layout components
///
/// @ingroup    APP_PANEL
///
//////////////////////////////////////////////////////////////////////////////
class LayoutPanel : public QWidget
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controllers     List of controllers
    /// @param[in]  delegate        Delegate for component specific actions
    /// @param[in]  type            Component type
    /// @param[in]  groupLayout     Layout to use within group boxes
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    LayoutPanel (control::ControllerManager&    controllers,
                 LayoutDelegate&                delegate,
                 control::controllerCapability  type,
                 LayoutGroup::layoutType        groupLayout,
                 QWidget*                       parent);

private:
    control::ControllerManager&     m_controllers;  ///< List of controllers
    LayoutDelegate&                 m_delegate;     ///< Delegate to handle component specific actions
    control::controllerCapability   m_type;         ///< Component type
    LayoutGroup::layoutType         m_groupLayout;  ///< Layout to use within group boxes

    //////////////////////////////////////////////////////////////////////////////
    /// Find the component group for a given controller
    ///
    /// @param[in]  controller      Controller to get component for
    ///
    /// @return     Group that manages components for that controller
    ///
    //////////////////////////////////////////////////////////////////////////////
    LayoutGroup* getGroupForController (const control::ControllerBase& controller);

private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Remove a controller from this panel
    ///
    /// @param[in]  controller      Controller to remove
    ///
    //////////////////////////////////////////////////////////////////////////////
    void remove (const control::ControllerBase& controller);

    //////////////////////////////////////////////////////////////////////////////
    /// Add a controller to this panel
    ///
    /// @param[in]  controller      Controller to add
    ///
    //////////////////////////////////////////////////////////////////////////////
    void addController (control::ControllerBase& controller);

    //////////////////////////////////////////////////////////////////////////////
    /// Handle a controller getting disconnected
    ///
    /// @param[in]  controller      Controller that has been disconnected
    ///
    //////////////////////////////////////////////////////////////////////////////
    void onDisconnect (const control::ControllerBase& controller);

    //////////////////////////////////////////////////////////////////////////////
    /// Called when a refresh has been completed for a controller's cache
    ///
    /// @param[in]  controller      Controller that has been refreshed
    ///
    //////////////////////////////////////////////////////////////////////////////
    void onRefresh (const control::ControllerBase& controller);
    };

} // namespace ui::common