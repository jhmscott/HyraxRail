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


///////////////////////////////////////////////////////////////////////////////
/// Perform a division operation and round upt the result
///
/// @tparam     T                   Numeric type
///
/// @param[in]  dividend    Dividend/numerator
/// @param[in]  divider      Divider/denomenator
///
/// @return     Division result
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
T divCeil (const T& dividend, const identityType<T>& divider)
    {
    if constexpr (std::is_integral_v<T>)
        {
        return (dividend + divider - 1) / divider;
        }
    else // std::is_floating_point_v<T>
        {
        return std::ceil (dividend / divider);
        }
    }

    } // namespace utils::math
