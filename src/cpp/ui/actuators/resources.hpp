/**
 * @file        actuators/respources.hpp
 * @brief       Common UI resources for actuators
 * @author      Justin Scott
 * @date        2026-04-26
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <layout/actuator.hpp>

#include <utils/resources.hpp>

#include <QIcon>

namespace ui::actuators::resources
{

// Display information for an actuator icon
struct iconInfo
    {
    QString                 title;  ///< Title of this icon
    utils::resources::Icon  icon;   ///< Scheme aware icon
    };


//////////////////////////////////////////////////////////////////////////////
/// Get the information about a given enumerated icon value
///
/// @param[in]  icon        Icon to get info for
///
/// @return     Icon information
///
//////////////////////////////////////////////////////////////////////////////
inline iconInfo getIconInfo (layout::actuatorIcon icon)
    {
    static const iconInfo icons[] =
        {
            {
            "Street Light",
            "switches/streetlight"
            },
            {
            "Mast Light",
            "switches/mast-light",
            },
            {
            "Layout Lighting",
            "switches/lightbulb"
            },
            {
            "Straight Turnout Left",
            "switches/left-track"
            },
            {
            "Straight Turnout Right",
            "switches/right-track"
            }
        };

    return icons[icon];
    }

} // namespace ui::actuators::resources