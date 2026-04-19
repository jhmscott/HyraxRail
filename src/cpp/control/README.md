# Control : Controller Communications

This module implements the layout controller interface defined in the [Layout](../layout/README.md) module, defines a common interface for all models of controllers in the ControllerBase class and implements communication logic for each controller model.

## Base Clases

### Protocol Base Class

Protocols provide the application layer protocol abstraction per controller. By making this a sperate class from the controller allows controllers to multiple protocols.

New Protocols must inherit from ProtocolBase in order to use the WorkerThread class described in [Utilities](#utils). Otherwise you are free to design the interface of this derived class in any way that serves your controller model best. The derived class must include the PROTOCOL_DEFINE macro to register it with the meta class system.

### Controller Base Class

The ControllerBase class inherits from the various [Layout](../layout/README.md) controllers. Derived classes must implement both the virtual functions in ControllerBase and the functions in the layout controllers.

Like the protocols, controllers use a meta class system, and all derived controller class must include the CONTROLLER_DEFINE macro.

## <a name="meta"></a>Meta Classes

The control module relies on a meta class system. This provides:
 * Metadata about the controller model and protocol
 * Mechanism for creating classes by type name
 
This allows for the UI to adapt to new controller models without needing to change the UI code. It also provides a mechaism to store and retrieve controller data between session (i.e. registrty).

### Protocol Meta Class

The protocol meta class provides the following metadata about the protocol:
 * Type name (this is the same as the name of the class)
 * Friendly name (used in UI)
 * Supported transport layer protocols (UDP, TCP, COM)
 * Default port
 * Default transport protocol
 
### Controller Meta Class

The controller meta class provides the following metadata about the controller model:
 * Type name (this is the same as the name of the class)
 * Friendly name (used in UI)
 * Supported protocols

## <a name="utils"></a>Utilities

The WorkerThread class provides a thread queue for network communication tasks with the controller. Currently this is the only utility, but more may be added, necesitating a dedicated utilities sub-module.

## Rules

New derived controller and protocol classes must use the CONTROLER_DEFINE and PROTOCOL_DEFINE macros respectively to ensure proper meta-class support. If this is done correctly, no change is needed to the UI to support the new model. If new functionality is needed in the base class, carefully consider it's impact on other models.

Ensure new classes, both protocol and controller, must be named uniquely, even if in different namespaces. The type name is used as a unique key in the [meta class system](#meta)
