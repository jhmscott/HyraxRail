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

#include <ostream>
#include <type_traits>

// Only include this for intellisense tools and doxygen generation
#if defined (__INTELLISENSE__) || defined (DOXYGEN)
#include <docs.hpp>
#endif // defined (__INTELLISENSE__) || defined (DOXYGEN)

#ifdef _MSVC_LANG
#define CPP_VERSION _MSVC_LANG  ///< C++ compiler version (MSVC)
#else
#define CPP_VERSION __cplusplus ///< C++ compiler version
#endif // _MSVC_LANG


#define CPP_VERSION_98  199711L     ///< C++ 98
#define CPP_VERSION_11  201103L     ///< C++ 11
#define CPP_VERSION_14  201402L     ///< C++ 14
#define CPP_VERSION_17  201703L     ///< C++ 17
#define CPP_VERSION_20  202002L     ///< C++ 20
#define CPP_VERSION_23 	202302L     ///< C++ 23

#ifdef NULL
#undef NULL
#endif // NULL

#define NULL nullptr    ///< Re-define to C++ nullptr

#ifdef __cpp_conditional_explicit
#define implicit explicit (false)   ///< Implicit constructor
#else
#define implicit
#endif


#if defined (Q_OS_LINUX) && !defined (Q_OS_ANDROID)
/// Defined for versions of linux that aren't android
#define NON_DROID_LINUX
#endif


//////////////////////////////////////////////////////////////////////////////
// Static assert that an array is an expected length
// Useful when array is tied to enum values
///
/// @param[in]  array       Array to check size of
/// @param[in]  len         Expected length of array
///
//////////////////////////////////////////////////////////////////////////////
#define ASSERT_ARRAY_LENGTH(array, len) static_assert (std::size (array) == len,\
                                                       "Array size mismatch")

/// Horizontal alignment
enum class hAlignment
    {
    LEFT,   ///< Align left
    RIGHT   ///< Align right
    };


#ifdef __cpp_lib_type_identity
template<class T>
using identityType = std::type_identity_t<T>; ///< Alias for std::type_identity
#else
/// Pre c++20 implementation of std::type_identity
template<class T>
struct _identityType
    {
    using type = T;
    };

/// Pre C++20 implementation of std::type_identity_t
template<class T>
using identityType = typename _identityType<T>::type;
#endif // __cpp_lib_type_identity


/// Hyrax rail's use of the User data
enum userDataRole
    {
    genericData = Qt::UserRole, ///< Generic user data. Always leave this available to clients
    schemeIcon,                 ///< Icon without the color scheme applied
    dropDownTier                ///< Which level of dropdown this is in a tiered dropdown
    };

/// Software version information
struct version
    {
    uint major; ///< Major version
    uint minor; ///< Minor version
    uint micro; ///< Micro version

    //////////////////////////////////////////////////////////////////////////////
    /// Stream formatting operator
    ///
    /// @param[in,out]  os      Stream to format to
    /// @param[in]      ver     Version to format
    ///
    /// @return         Output stream
    ///
    //////////////////////////////////////////////////////////////////////////////
    friend std::ostream& operator<< (std::ostream& os, const version& ver)
        {
        return (os << ver.major << "." << ver.minor << "." << ver.micro);
        }

    //////////////////////////////////////////////////////////////////////////////
    /// Comparison operator
    ///
    /// @param[in]  other       Version to compare to
    ///
    /// @return     True if this equals other
    ///
    //////////////////////////////////////////////////////////////////////////////
    constexpr bool operator== (const version& other) const
        {
        return other.major == major &&
               other.minor == minor &&
               other.micro == micro;
        }

    //////////////////////////////////////////////////////////////////////////////
    /// Comparison operator
    ///
    /// @param[in]  other       Version to compare to
    ///
    /// @return     True if this does not equals other
    ///
    //////////////////////////////////////////////////////////////////////////////
    constexpr bool operator!= (const version& other) const { return !(*this == other); }
    };