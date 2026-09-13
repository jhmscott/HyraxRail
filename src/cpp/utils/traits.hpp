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
    static constexpr bool is_const = false; ///< Is this const member func (no)

    using class_t   = Class;                ///< Class type
    using ret_t     = RetT;                 ///< Return type
    using args_t    = std::tuple<Args...>;  ///< Tuple of argument types
    };

/// Const member specialization
template<class Class, class RetT, class... Args>
struct memberFuncTraits<RetT (Class::*) (Args...) const >
    {
    static constexpr bool is_const = true;  ///< Is this const member func (yes)

    using class_t   = Class;                ///< Class type
    using ret_t     = RetT;                 ///< Return type
    using args_t    = std::tuple<Args...>;  ///< Tuple of argument types
    };


///////////////////////////////////////////////////////////////////////////////
/// Type envelope, to allow you to pass non-trivial type information to a function
///
/// @tparam     T       Type to wrap
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
struct envelope
    {
    using type = T;
    };

///////////////////////////////////////////////////////////////////////////////
/// Type trait for always false. Type dependent to confuse the compiler enough
/// to let you do static_assert(false, ...)
///
/// @tparam     T       Any type
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
struct always_false : std::false_type {};


///////////////////////////////////////////////////////////////////////////////
/// Replacement for false when you need to confuse the compiler a bit. Use it
/// like:
///     static_assert(always_false_v, ...)
///
/// @tparam     T       Any type
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
inline constexpr bool always_false_v = always_false<T>::value;

}