# HyraxRail

Hyrax Rail is a cross platform model train controller app, based on QT 6, using QWidgets. The main principles of it's design are as follows:
 * Cross Platform : Rely on QT as much as possible to maintain portability
 * Zero config : Pull as many settings from the controller as possible to make use plug and play
 * Extendable : It should be possible to add new models of controller without changing the UI code

## Architecture

```
├── Control : Logic for communicating with the train controllers
│   ├── controllers : Controller communication implementation
│   ├── protocols : Application layer protocols used for communication
├── Layout : Abstraction layer, provides objects to represent common layout elements
├── UI : User Interface layer. Modules within are broken down by tab within the main app
│   ├── actuators : Configuration and operation of switching devices, like turnouts
│   ├── common : Common UI elements and functions for re-use throughout UI
|   ├── config : Settings tab and associated dialogs
│   ├── routes : Configuation and control of routes. These group together switching devices into a common action
│   ├── sensors : Devices for detecting the location of trains. Allows viewing of state and associating with routes
|   ├── trains : Control of train speed and functions
├── Utils : Common utilities
```

## Controller Support

* Märklin Central Station 1 (60212) : _In progress_ 
* Märklin Central Station 2 (60215) : _Planned_
* DCC-Ex : _planned_

## Operating System Support

 * Windows : _Done_
 * Mac OS : _In progress_
 * Android : _In progress_
 * Linux : _Planned_

## Getting Started

To get started, navigate to the ![gear](./res/icons/misc/gear.svg)_settings_ tab and select ![gear](./res/icons/misc/plus.svg)_Add Controller_. 

Select the controller model from the drop-down. The default protocol and connection settings will automatically populate. If you wish to change them, do so here. Note that the COM port and IP address fields must always be manually entered. Press OK to continue.

The app will automatically populate the trains, actuators, routes and sensors configured on your controller. Any changes you make to this configuration will be pushed back to the controller.
