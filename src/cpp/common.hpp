/**
 * @file        common.hpp
 * @brief       Common defintions and types
 * @author      Justin Scott
 * @date        2026-01-04
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#include <QObject>

#include <bitset>
#include <array>
#include <algorithm>
#include <type_traits>


#ifdef _MSVC_LANG
#define CPP_VERSION _MSVC_LANG
#else
#define CPP_VERSION __cplusplus
#endif // _MSVC_LANG


#define CPP_VERSION_98  199711L
#define CPP_VERSION_11  201103L
#define CPP_VERSION_14  201402L
#define CPP_VERSION_17  201703L
#define CPP_VERSION_20  202002L
#define CPP_VERSION_23 	202302L

#ifdef NULL
#undef NULL
#endif // NULL

#define NULL nullptr

enum class vAlignment
    {
    LEFT,
    RIGHT
    };


#ifdef __cpp_lib_type_identity
template<class T>
using identityType = std::type_identity_t<T>;
#else
template<class T>
struct _identityType
    {
    using type = T;
    };

template<class T>
using identityType = typename _identityType<T>::type;
#endif // __cpp_lib_type_identity


template<class T, size_t N>
constexpr bool contains (const T (&arr)[N], const identityType<T>& val)
    {
    const T*    end     = arr + N;
    const T*    it;

#ifdef STL_HAS_CONSTEXPR20
    it = std::find (arr,
                    end,
                    val);
#else
    for (it = arr; arr < end; ++it)
        {
        if (val == *it) { break; }
        }
#endif // STL_HAS_CONSTEXPR20


    return end != it;
    }


template<class T, class U, size_t N>
size_t findArrayIdx (const T (&arr)[N], U& val)
    {
    const T* end = arr + N;
    const T* it;

    it = std::find (arr,
                    end,
                    val);

    return it - arr;
    }

namespace control
{

struct version
    {
    uint8_t major = 0;
    uint8_t minor = 0;
    uint8_t micro = 0;

    constexpr bool operator== (const version& other) const
#ifdef __cpp_impl_three_way_comparison
        = default;
#else
        { return major == other.major && minor == other.minor && micro == other.micro;}
#endif // __cpp_impl_three_way_comparison
    };


}