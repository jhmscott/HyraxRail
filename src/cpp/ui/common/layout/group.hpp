/**
 * @file        layout/group.hpp
 * @brief       Group box triggering components. One per controller
 * @author      Justin Scott
 * @date        2026-10-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <control/automation/manager.hpp>
#include <control/controllers/base.hpp>

#include <ui/common/addbtn.hpp>

#include <ui/common/layout/delegate.hpp>

#include <ui/lang.hpp>

#include <QGroupBox>

namespace ui::common
{
//////////////////////////////////////////////////////////////////////////////
/// Group box triggering components. One per controller
///
//////////////////////////////////////////////////////////////////////////////
class LayoutGroup : public QGroupBox
    {
    Q_OBJECT
public:
    /// Type of layout to use in the group box
    enum class layoutType
        {
        GRID,   ///< Grid style layout, @see ui::common::AutoGridLayout
        VBOX    ///< Vertical box layout
        };

    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  controller      Controller to display routes for
    /// @param[in]  delegate        Delegate for component specific actions
    /// @param[in]  type            Component type
    /// @param[in]  layoutType      Type of layout to use within the group box
    /// @param[in]  parent          Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    LayoutGroup (control::ControllerBase&       controller,
                 LayoutDelegate&                delegate,
                 control::controllerCapability  type,
                 layoutType                     layoutType,
                 QWidget*                       parent);

    //////////////////////////////////////////////////////////////////////////////
    /// Get the controller this group is managing components for
    ///
    /// @return     Controller instance
    ///
    //////////////////////////////////////////////////////////////////////////////
    control::ControllerBase& getController () { return m_controller; }

    //////////////////////////////////////////////////////////////////////////////
    /// Refresh the list of components
    ///
    //////////////////////////////////////////////////////////////////////////////
    void refresh ();
private:
    control::ControllerBase&        m_controller;   ///< Controller
    LayoutDelegate&                 m_delegate;     ///< Delegate for component specific actions
    QLayout*                        m_gridLayout;   ///< Layout containing route buttons
    common::AddButton*              m_addLabel;     ///< Button to add a new route
    control::controllerCapability   m_type;         ///< Component type

    //////////////////////////////////////////////////////////////////////////////
    /// Set this widget's labels' text
    ///
    //////////////////////////////////////////////////////////////////////////////
    void setLabels ();

    UILANG_ON_CHANGE (QGroupBox, setLabels ())
private slots:
    //////////////////////////////////////////////////////////////////////////////
    /// Prompt the user to create a new route
    ///
    //////////////////////////////////////////////////////////////////////////////
    void add ();

    //////////////////////////////////////////////////////////////////////////////
    /// Remove route from the controller and the panel
    ///
    //////////////////////////////////////////////////////////////////////////////
    void remove ();
    };
} // namespace ui::common