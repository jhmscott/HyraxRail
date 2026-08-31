/**
 * @file        utils/traits.hpp
 * @brief       C++ template type traits
 * @author      Justin Scott
 * @date        2026-08-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

/// Template type traits
namespace utils::traits
{

/// Unspecialized version
template<class T>
struct memberFuncTraits
    {

    };


///////////////////////////////////////////////////////////////////////////////
/// Type trait to get information about a member functiojn
///
/// @tparam     Class   Class the function is a member of
/// @tparam     RetT    Function return type
/// @tparam     Args    Function arguments
///
///////////////////////////////////////////////////////////////////////////////
template<class Class, class RetT, class... Args>
struct memberFuncTraits<RetT (Class::*) (Args...)>
    {
    using class_t   = Class;                ///< Class type
    using ret_t     = RetT;                 ///< Return type
    using args_t    = std::tuple<Args...>;  ///< Tuple of argument types
    };

/// Const member specialization
template<class Class, class RetT, class... Args>
struct memberFuncTraits<RetT (Class::*) (Args...) const >
    {
    using class_t   = Class;                ///< Class type
    using ret_t     = RetT;                 ///< Return type
    using args_t    = std::tuple<Args...>;  ///< Tuple of argument types
    };
}