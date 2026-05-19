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
    static const iconInfo ICONS[] =
        {
            {
            QObject::tr ("Street Light"),
            "switches/streetlight"
            },
            {
            QObject::tr ("Mast Light"),
            "switches/mast-light",
            },
            {
            QObject::tr ("Layout Lighting"),
            "switches/lightbulb"
            },
            {
            QObject::tr ("Straight Turnout Left"),
            "switches/left-track"
            },
            {
            QObject::tr ("Straight Turnout Right"),
            "switches/right-track"
            }
        };
    ASSERT_ARRAY_LENGTH (ICONS, layout::NUM_TRUE_ICONS);

    return ICONS[icon];
    }

} // namespace ui::actuators::resources