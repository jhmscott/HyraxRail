/**
 * @file        utils/math.hpp
 * @brief       Math functions
 * @author      Justin Scott
 * @date        2026-02-01
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <common.hpp>

namespace utils::math
{

template<class T>
T divCeil (const T& dividend, const identityType<T>& divider)
    {
    if constexpr (std::is_integral_v<T>)
        {
        return (dividend + divider - 1) / divider;
        }
    else
        {
        return std::ceil (dividend / divider);
        }
    }

}