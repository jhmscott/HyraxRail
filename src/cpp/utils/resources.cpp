/**
 * @file        utils/resources.cpp
 * @brief       Helper functions for resolving resouce paths
 * @author      Justin Scott
 * @date        2026-05-10
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#include <utils/resources.hpp>

namespace utils::resources
{
QIcon Icon::toIcon (Qt::ColorScheme scheme) const
    {
    QIcon   icon;

    QString schemeName  = Qt::ColorScheme::Dark == scheme ? "dark" : "light";
    QString basePath    = makePath (RESOURCE_ICON, schemeName + "/" + m_subPath);
    QString defaultPath = basePath + ".svg";
    QString onPath      = basePath + "-on.svg";
    QString offPath     = basePath + "-off.svg";

    if (not QFile::exists (defaultPath) &&
        not QFile::exists (onPath))
        {
        basePath    = makePath (RESOURCE_ICON, "common/" + m_subPath);
        defaultPath = basePath + ".svg";
        onPath      = basePath + "-on.svg";
        offPath     = basePath + "-off.svg";
        }

    if (QFile::exists (defaultPath))
        {
        icon.addFile (defaultPath);
        }

    if (QFile::exists (onPath))
        {
        icon.addFile (onPath, QSize{}, QIcon::Normal, QIcon::On);
        }

    if (QFile::exists (offPath))
        {
        icon.addFile (offPath, QSize{}, QIcon::Normal, QIcon::Off);
        }

    return icon;
    }

} // namespace utils::resources