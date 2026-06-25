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

#include <qmath.h>

#include <limits>

namespace utils::math
{

inline constexpr double DEGRAD = (M_PI / 180.0);
inline constexpr double RADDEG = (180.0 / M_PI);

///////////////////////////////////////////////////////////////////////////////
/// Perform a division operation and round upt the result
///
/// @tparam     T           Numeric type
///
/// @param[in]  dividend    Dividend/numerator
/// @param[in]  divider     Divider/denomenator
///
/// @return     Division result
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
T divCeil (const T& dividend, const identityType<T>& divider)
    {
    static_assert (std::is_arithmetic_v<T>, "Must be numeric type");

    if constexpr (std::is_integral_v<T>)
        {
        return (dividend + divider - 1) / divider;
        }
    else // std::is_floating_point_v<T>
        {
        return std::ceil (dividend / divider);
        }
    }

///////////////////////////////////////////////////////////////////////////////
/// Round a floating point value to the nearest integer
///
/// @tparam     T       (optiona) Integer type, defaults to int
///
/// @param[in]  dbl     Floating point double to round
///
/// @return     dbl rounded to nearest integer value
///
///////////////////////////////////////////////////////////////////////////////
template<class T = int>
T roundToInt (double dbl)
    {
    static_assert (std::numeric_limits<T>::is_integer, "Must be an integer type");
    return static_cast<T> (std::round (dbl));
    }



///////////////////////////////////////////////////////////////////////////////
/// Take the square of a value
///
/// @tparam     T       Numeric type
///
/// @param[in]  val     Value to square
///
/// @return     val squared
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
T sqr (T val) { return val * val; }

} // namespace utils::math
