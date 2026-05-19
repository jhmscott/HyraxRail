/**
 * @file        config/notifygroup.hpp
 * @brief       Group box for the configuration of notifications
 * @author      Justin Scott
 * @date        2026-04-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <QGroupBox>

namespace ui::config
{

//////////////////////////////////////////////////////////////////////////////
/// Group box for notification settings
///
//////////////////////////////////////////////////////////////////////////////
class NotifyGroup : public QGroupBox
    {
public:
    //////////////////////////////////////////////////////////////////////////////
    /// Constructor
    ///
    /// @param[in]  parent      Parent widget
    ///
    //////////////////////////////////////////////////////////////////////////////
    explicit NotifyGroup (QWidget* parent);
    };

} // ui::config