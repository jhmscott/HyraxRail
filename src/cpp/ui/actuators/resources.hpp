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

#include <QIcon>

namespace ui::actuators::resources
{

// Display information for an actuator icon
struct iconInfo
    {
    QString title;  ///< Title of this icon
    QIcon   on;     ///< Actuator on state icon
    QIcon   off;    ///< Actuator off state icon
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
            QIcon{ ":/icons/switches/streetlight-on.svg"},
            QIcon{ ":/icons/switches/streetlight-off.svg"},
            },
            {
            "Mast Light",
            QIcon{ ":/icons/switches/mast-light-on.svg"},
            QIcon{ ":/icons/switches/mast-light-off.svg"},
            },
            {
            "Layout Lighting",
            QIcon{ ":/icons/switches/lightbulb.svg"},
            QIcon{ ":/icons/switches/lightbulb-off.svg"},
            },
            {
            "Straight Turnout Left",
            QIcon{ ":/icons/switches/left-track-left.svg"       },
            QIcon{ ":/icons/switches/left-track-straight.svg"   }
            },
            {
            "Straight Turnout Right",
            QIcon{ ":/icons/switches/right-track-right.svg"     },
            QIcon{ ":/icons/switches/right-track-straight.svg"  }
            }
        };

    return icons[icon];
    }

} // namespace ui::actuators::resources