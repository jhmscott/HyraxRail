# Layout : Component Abstraction

This module provides an abstract interface for various components of the layout without providing direct access to the controller. 

## Goals

This goal of this is to provides access to each component via a single object, instead of trough the controller object. This aims for:
 *  A simplified interface: clients don't need to know about controller specific IDs and member functions that don't impact their feature.
 * Limiting access: clients are given access to only the parts of the controller they need to.
 * Tracking and signalling: clients can be notified of controller events like destruction, without needing access to the controller manager.

## <a name="id-sys"></a>The ID system

Each component is identified by a unique 64-bit ID. This need only be unique within the component type, but can be globally unqiue. The ID is specific to the controller model and is thus implementation specific. This shouldn't be relied on beyond it's utility for this purpose.

## <a name="structure"></a>Structure 

Each file should contain a controller class and a component class. They derive from ControllerBase and ComponentDerived respectively. Each controller class is a friend of the component class and vice versa. The controller class member functions are all private and virtual. This sevres the same purpose as the [Attorney-Client Idom](https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Friendship_and_the_Attorney-Client).

Each member function of the controller accepts the component ID, as described by [The ID System](#id-sys).

## Components

The following components are currently implemented:
 * Actuator : a generic switching device like a turnout
 * Locomotive : a powered train
 * Route : a group of actuators that can be triggered together

## Rules

When adding new features to a component, add the corresponding function to both the Component and Controller clases. This will neccessitate a change in the [Control](../control/README.md) module to implement. When adding a new Component class, create a new file and add the corresponding Controller class as described in [Structure](#structure). ControllerBase in the [Control](../control/README.md) must derive from this.
