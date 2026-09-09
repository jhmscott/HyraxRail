/**
 * @file        tains/resources.hpp
 * @brief       Train UI resources
 * @author      Justin Scott
 * @date        2026-09-06
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once


#include <common.hpp>

#include <layout/locomotive.hpp>

#include <utils/resources.hpp>


/// Train UI resources
namespace ui::trains::resources
{
/// Information about a function icon
struct functionInfo
    {
    utils::resources::Icon  icon;   ///< Icon SVG to use in the UI
    QString                 name;   ///< Name of the icon
    };

///////////////////////////////////////////////////////////////////////////////
/// Get the information for a function icon
///
/// @param[in]  icon        Enumerated icon type
///
/// @return     Info about the icon
///
///////////////////////////////////////////////////////////////////////////////
functionInfo getFunctionInfo (layout::funcInfo::icon_t icon);

} // namespace ui::trains::resources