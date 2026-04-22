/**
 * @file        utils/algorithm.hpp
 * @brief       Generic algorithms for modifying containers.
 *              Inspired by c++20 ranges
 * @author      Justin Scott
 * @date        2026-01-27
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <memory>
#include <vector>

#include <common.hpp>

namespace utils::algorithm
{
// Internal implementation details, don't use outside this header
namespace internal
{
///////////////////////////////////////////////////////////////////////////////
/// Creates a predicate functor to compare a unique_ptr<> to a dumb pointer value
///
/// @tparam     T           Pointer type
///
/// @param[in]  ptr       Pointer to compare against in predicate
///
/// @return     Predicate functor
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
auto makePtrPred (const T* ptr)
    {
    return [ptr] (const std::unique_ptr<T>& other) -> bool
                 {  return other.get () == ptr; };
    }
} // namespace internal

///////////////////////////////////////////////////////////////////////////////
/// Erase all elements in a vector that fulfill a predicate. Basically std::erase_if() from c++20
///
/// @tparam         T           Vector element type
/// @tparam         Pred    Predicate type
///
/// @param[in,out]  vec      Vector to erase elements from
/// @param[in]      pred    Predicate
///
///////////////////////////////////////////////////////////////////////////////
template<class T, class Pred>
void eraseIf (std::vector<T>& vec, Pred pred)
    {
    auto it = std::remove_if (vec.begin (),
                              vec.end (),
                              pred);
    vec.erase (it, vec.end ());
    }

///////////////////////////////////////////////////////////////////////////////
/// Erase all elements from a vector equal to a value
///
/// @tparam         T               Vector element type
/// @tparam         U               Value to remove. T{}==U{} must be defined
///
/// @param[in,out]  vec           Vector to remove elements from
/// @param[in]      val           Value to look for
///
///////////////////////////////////////////////////////////////////////////////
template<class T, class U>
void erase (std::vector<T>& vec, const U& val)
    {
    auto it = std::remove (vec.begin (),
                           vec.end (),
                           val);
    vec.erase (it, vec.end ());
    }

///////////////////////////////////////////////////////////////////////////////
/// Removes elements from a vector of unique_ptr<> by the pointer address/value. Const version
///
/// @tparam         T               Pointer type
///
/// @param[in,out]  vec           Vector to remove elements from
/// @param[in]      ptr           Value to remove
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
void eraseByPtr (std::vector<const std::unique_ptr<T>>& vec, const T* ptr)
    {
    eraseIf (vec, internal::makePtrPred (ptr));
    }

///////////////////////////////////////////////////////////////////////////////
/// Removes elements from a vector of unique_ptr<> by the pointer address/value. Non-const version
///
/// @tparam         T               Pointer type
///
/// @param[in,out]  vec           Vector to remove elements from
/// @param[in]      ptr           Value to remove
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
void eraseByPtr (std::vector<std::unique_ptr<T>>& vec, const T* ptr)
    {
    eraseIf (vec, internal::makePtrPred (ptr));
    }


///////////////////////////////////////////////////////////////////////////////
/// Find the first element in an vector of unique_ptr<> by the pointer address/value. Const version
///
/// @tparam         T               Pointer type
///
/// @param[in,out]  vec           Vector to search
/// @param[in]      ptr           Value to search
///
/// @return         Iterator to first instance of ptr in vec. vec.end() if none exist
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
auto findByPtr (const std::vector<const std::unique_ptr<T>>& vec, const T* ptr)
    {
    return std::find_if (vec.begin (),
                         vec.end (),
                         internal::makePtrPred (ptr));
    }

///////////////////////////////////////////////////////////////////////////////
/// Find the first element in an vector of unique_ptr<> by the pointer address/value. Non-const version
///
/// @tparam         T               Pointer type
///
/// @param[in,out]  vec           Vector to search
/// @param[in]      ptr           Value to search
///
/// @return         Iterator to first instance of ptr in vec. vec.end() if none exist
///
///////////////////////////////////////////////////////////////////////////////
template<class T>
auto findByPtr (const std::vector<std::unique_ptr<T>>& vec, const T* ptr)
    {
    return std::find_if (vec.begin (),
                         vec.end (),
                         internal::makePtrPred (ptr));
    }

} // namespace utils::algorithm
