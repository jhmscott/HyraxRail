/**
 * @file        controller/utils.hpp
 * @brief       Common utilities and defintions for the controller system
 * @author      Justin Scott
 * @date        2026-09-30
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */


#pragma once

#include <layout/actuator.hpp>
#include <layout/estop.hpp>
#include <layout/locomotive.hpp>
#include <layout/route.hpp>

#include <layout/virtual/vestop.hpp>
#include <layout/virtual/vroute.hpp>

namespace control
{


/// Capability a controller can have
enum controllerCapability
    {
    CAPABILITY_LOCOMOTIVE,  ///< @see layout::LocomotiveController
    CAPABILITY_ACTUATOR,    ///< @see layout::ActuatorController
    CAPABILITY_ROUTE,       ///< @see layout::RouteController
    CAPABILITY_ESTOP,       ///< @see layout::EmergencyStopController

    NUM_CAPABILITIES        ///< Delimiter only
    };

/// Set of capabilities supported by a controller
using controllerCapabilitySet = std::bitset<NUM_CAPABILITIES>;

/// List of types defining the interface of the capability controllers
using capabilityControllerTypes = std::tuple<layout::ActuatorController,
                                             layout::LocomotiveController,
                                             layout::RouteController,
                                             layout::EmergencyStopController>;
ASSERT_TUPLE_SIZE (capabilityControllerTypes, NUM_CAPABILITIES);

/// List of type implementing software defined/virtual controller types
using softwareControllerTypes = std::tuple<layout::VirtualEmergencyStopController,
                                           layout::VirtualRouteController>;

/// Controller type traits
namespace traits
{
///////////////////////////////////////////////////////////////////////////////
/// Type trait to get the enumerated capability type for a given layout controller
///
/// @tparam     Controller      Layout controller type
///
///////////////////////////////////////////////////////////////////////////////
template<class Controller>
struct capability
    {};

template<>
struct capability<layout::ActuatorController>
    {
    static constexpr controllerCapability value = CAPABILITY_ACTUATOR;
    };

template<>
struct capability<layout::LocomotiveController>
    {
    static constexpr controllerCapability value = CAPABILITY_LOCOMOTIVE;
    };

template<>
struct capability<layout::RouteController>
    {
    static constexpr controllerCapability value = CAPABILITY_ROUTE;
    };

template<>
struct capability<layout::EmergencyStopController>
    {
    static constexpr controllerCapability value = CAPABILITY_ESTOP;
    };


//////////////////////////////////////////////////////////////////////////////
/// Gets the dependencies and base type for a given virtual controller type
///
/// @tparam     T       Virtual controller type (derived0
///
/// @remarks    Shakes fist at C++ standard. Why can't I make a member function
///             pointer type for a constructor? Then I could use memberFuncTraits
///             instead of defining this for each type
///
/// @see        https://stackoverflow.com/questions/41623899/c-obtaining-the-type-of-a-constructor
/// @see        https://www.reddit.com/r/cpp_questions/comments/nti1bt/how_to_deduce_parameter_types_of_a_constructor/
///
//////////////////////////////////////////////////////////////////////////////
template<class T>
struct virtualController {};

template<>
struct virtualController<layout::VirtualEmergencyStopController>
    {
    using base          = layout::EmergencyStopController;
    using dependencies  = std::tuple<layout::LocomotiveController>;
    };

template<>
struct virtualController<layout::VirtualRouteController>
    {
    using base          = layout::RouteController;
    using dependencies  = std::tuple<layout::ActuatorController>;
    };

} // namespace traits

} // namespace control