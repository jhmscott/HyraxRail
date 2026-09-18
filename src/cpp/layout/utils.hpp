/**
 * @file        layout/utils.hpp
 * @brief       Layout utilities
 * @author      Justin Scott
 * @date        2026-09-17
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <utils/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////////
/// Generates boilerplate for a layout component getter
///
/// @param[in]  member      State member variable
/// @param[in]  ...         (optional) default value, if state is NULL
///
///////////////////////////////////////////////////////////////////////////////
#define LAYOUT_DEFINE_GETTER(member, ...)                                       \
    {                                                                           \
    return utils::algorithm::safeGet (m_state,                                  \
                                     &decltype (m_state)::element_type::member  \
                                      __VA_OPT__(,) __VA_ARGS__);               \
    }

///////////////////////////////////////////////////////////////////////////////
/// Generates boilerplate for a layout component setter
///
/// @param[in]  member      State member variable
/// @param[in]  setter      Controller setter function
/// @param[in]  param       Setter function parameter
///
///////////////////////////////////////////////////////////////////////////////
#define LAYOUT_DEFINE_SETTER(member, setter, param) \
    {                                               \
    if (NULL != m_state && NULL != m_controller)    \
        {                                           \
        m_state->member = param;                    \
        m_controller->setter (m_id, param);         \
        }                                           \
    }
