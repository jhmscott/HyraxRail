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
namespace internal
{
template<class T>
auto makePtrPred (const T* ptr)
    {
    return [ptr] (const std::unique_ptr<T>& other) -> bool
             {  return other.get () == ptr; };
    }
}

template<class T, class Pred>
void eraseIf (std::vector<T>& vec, Pred pred)
    {
    auto it = std::remove_if (vec.begin (),
                              vec.end (),
                              pred);
    vec.erase (it, vec.end ());
    }


template<class T, class U>
void erase (std::vector<T>& vec, const U& val)
    {
    auto it = std::remove (vec.begin (),
                           vec.end (),
                           val);
    vec.erase (it, vec.end ());
    }

template<class T>
void eraseByPtr (std::vector<const std::unique_ptr<T>>& vec, const T* ptr)
    {
    eraseIf (vec, internal::makePtrPred (ptr));
    }


template<class T>
void eraseByPtr (std::vector<std::unique_ptr<T>>& vec, const T* ptr)
    {
    eraseIf (vec, internal::makePtrPred (ptr));
    }


template<class T>
auto findByPtr (std::vector<const std::unique_ptr<T>>& vec, const T* ptr)
    {
    return std::find_if (vec.begin (),
                         vec.end (),
                         internal::makePtrPred (ptr));
    }


template<class T>
auto findByPtr (std::vector<std::unique_ptr<T>>& vec, const T* ptr)
    {
    return std::find_if (vec.begin (),
                         vec.end (),
                         internal::makePtrPred (ptr));
    }

}
