/**
 * @file        utils/resources.hpp
 * @brief       Helper functions for resolving resouce paths
 * @author      Justin Scott
 * @date        2026-05-05
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */
#pragma once

#include <common.hpp>

#include <QFile>
#include <QIcon>
#include <QString>

namespace utils::resources
{

// Resource type
enum type
    {
    RESOURCE_FONT,      ///< Font file (.ttf, .otf, etc)
    RESOURCE_ICON,      ///< Icon file (.svg, .ico, etc)
    RESOURCE_TEXT,      ///< Plain or rich text file (.txt, .md, etc)

    NUM_RESOURCE_TYPES  ///< Delimiter only
    };


///////////////////////////////////////////////////////////////////////////////
/// Create a resource path
///
/// @param[in]  type    Resource type
/// @param[in]  path    Path relative to the reource type folder
///
/// @return     Full resource path
///
///////////////////////////////////////////////////////////////////////////////
inline QString makePath (type type, const QString& path)
    {
    static const QString resourceSuperPaths[] =
        {
        "fonts",
        "icons",
        "texts"
        };

    return ":/" + resourceSuperPaths[type] + "/" + path;
    }


///////////////////////////////////////////////////////////////////////////////
/// Icon stored a resource file. Includes the on/off state of icon, and color
/// scheme variants
///
/// @remarks    Icons should be stored in the res folder using the following path:<BR>
///             icons/<scheme>/<path>.svg<BR><BR>
///
///             Where <scheme> is light or dark. If there is no difference between
///             the light and dark icons, <scheme> can be replaced with common.<BR>
///             The <path> is the path parameter in the constructor.<BR><BR>
///
///             If there is different icons for the on and off states, they should
///             be named as follows:<BR>
///             On state    : <path>-on.svg<BR>
///             Off state   : <path>-off.svg
///
///
///////////////////////////////////////////////////////////////////////////////
class Icon
    {
public:
    ///////////////////////////////////////////////////////////////////////////////
    /// Default constructor
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Icon () = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an icon
    ///
    /// @param[in]  path    Path relative to scheme folder within icon folder
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Icon (const QString& path) :
        m_subPath (path)
        {}

    ///////////////////////////////////////////////////////////////////////////////
    /// Create an icon
    ///
    /// @param[in]  path    Path relative to scheme folder within icon folder
    ///
    ///////////////////////////////////////////////////////////////////////////////
    Icon (const char* path) :
        m_subPath (path)
        {}

    // Default copy behaviour
    Icon (const Icon&) = default;
    Icon& operator= (const Icon&) = default;

    ///////////////////////////////////////////////////////////////////////////////
    /// Create a QIcon for a given colour scheme
    ///
    /// @param[in]  scheme      Colour scheme
    ///
    /// @return     Icon
    ///
    ///////////////////////////////////////////////////////////////////////////////
    QIcon toIcon (Qt::ColorScheme scheme) const;

    ///////////////////////////////////////////////////////////////////////////////
    /// Check if this icon has a path
    ///
    /// @return     True if icon has a path
    ///
    ///////////////////////////////////////////////////////////////////////////////
    operator bool () const { return not m_subPath.isEmpty (); }
private:
    QString m_subPath;
    };

} // namespace utils::resources