/**
 * @file        tains/resources.cpp
 * @brief       Train UI resources
 * @author      Justin Scott
 * @date        2026-09-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <ui/trains/resources.hpp>

namespace ui::trains::resources
{

functionInfo getFunctionInfo (layout::funcInfo::icon_t icon)
    {
    const functionInfo FUNCTION_ICONS[] =
        {
        // lights
        { "functions/headlights",
          QObject::tr ("Headlight") },          ///< ICON_FUNC_LIGHT_HEADLIGHT,
        { "functions/lightbulb",
          QObject::tr ("Cabin Lighting") },     ///< ICON_FUNC_LIGHT_CAB

        // Sound
        { "functions/megaphone",
          QObject::tr ("Horn") },               ///< ICON_FUNC_SOUND_HORN
        { "functions/tire",
          QObject::tr ("Brakes") },             ///< ICON_FUNC_SOUND_BRAKES
        { "functions/plugs-connected",
           QObject::tr ("Coupling Sound") },    ///< ICON_FUNC_SOUND_COUPLING
        { "functions/speaker-high",
           QObject::tr ("Generic Sounds") },    ///< ICON_FUNC_SOUND_GENERIC
        { "functions/engine",
           QObject::tr ("Operating Sounds") },  ///< ICON_FUNC_SOUND_OPERATING

        // Misc
        { "functions/radical",
           QObject::tr ("Pantograph") },
        { "functions/chart-line-up",
           QObject::tr ("ABV") },
        { "functions/snail",
           QObject::tr ("Slow Locomotive") },
        };
    ASSERT_ARRAY_LENGTH (FUNCTION_ICONS, layout::funcInfo::NUM_TRUE_ICONS);

    functionInfo info;

    if (layout::funcInfo::ICON_FUNC_NUMBER == icon)
        {
        info = { "", "Generic Function" };
        }
    else
        {
        info = FUNCTION_ICONS[icon];
        }

    return info;
    }
}