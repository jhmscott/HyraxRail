/**
 * @file        controller/manager.cpp
 * @brief       Provides container to manage controllers, with signals for
 *              when controllers are added, deleted or changed
 * @author      Justin Scott
 * @date        2026-04-20
 *
 * @copyright   Copyright (c) 2026 Justin ScottS
 */

#include <control/controllers/manager.hpp>

#include <utils/algorithm.hpp>

namespace control
{
void ControllerManager::append (const createControllerInfo& info)
    {
    auto controller = createController (info);

    if (NULL != controller)
        {
        m_controllers.emplace_back (std::move (controller));

        emit controllerAdded (*m_controllers.back ());
        }
    }

void ControllerManager::remove (const ControllerBase& controller)
    {
    auto it = utils::algorithm::findByPtr (m_controllers, &controller);

    if (m_controllers.end () != it)
        {
        erase (m_controllers.begin () + (it - m_controllers.cbegin ()));
        }
    }

void ControllerManager::erase (Iterator it)
    {
    size_t idx = std::distance (m_controllers.begin (), it.m_it);

    emit controllerDeleted (*m_controllers[idx]);

    m_controllers.erase (it.m_it);
    }

ptrdiff_t ControllerManager::indexOf (const ControllerBase& controller) const
    {
    ptrdiff_t   idx = -1;
    auto        it  = utils::algorithm::findByPtr (m_controllers, &controller);

    if (m_controllers.end () != it)
        {
        idx = std::distance (m_controllers.begin (), it);
        }

    return idx;
    }

} // namespace control