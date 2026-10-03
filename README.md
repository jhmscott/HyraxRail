<img src="./res/icons/app/conductor-hyrax-mini.svg" align=left height="64" width="64" style="float:left;" alt="Hyrax Conductor">

# HyraxRail

Hyrax Rail is a cross platform model train controller app, based on QT 6, using QWidgets. The main principles of it's design are as follows:

* Cross Platform : Rely on QT as much as possible to maintain portability
* Zero config : Pull as many settings from the controller as possible to make use plug and play
* Extendable : It should be possible to add new models of controller without changing the UI code

[![Qt](https://img.shields.io/badge/Qt-%23217346.svg?style=for-the-badge&logo=Qt&logoColor=white)](https://www.qt.io)
[![Apache 2.0 License](https://img.shields.io/badge/license-Apache%20License%202.0-blue)](https://www.apache.org/licenses/LICENSE-2.0)

## Architecture

```mermaid
%%{ init : { "flowchart" : { "curve" : "stepAfter" }, 'block': { 'padding': 12 } }}%%
block
    columns 1
    block:UI
        columns 6

        UILabel["UI"]:6
        style UILabel fill:transparent,stroke:none;

        Actuators
        Clock
        Config
        Routes
        Sensors
        Trains

        space:6

        space:1
        Common:4

        Common --> Actuators
        Common --> Clock
        Common --> Config
        Common --> Routes
        Common --> Sensors
        Common --> Trains
    end

    block:Control
        columns 5

        space:2
        ControlLabel["Control"]
        style ControlLabel fill:transparent,stroke:none;
        space:2

        Automation
        space
        Controllers
        space
        Protocols

        Automation --> Controllers
        Protocols --> Controllers
    end

    block:Layout
        columns 3

        space:1
        LayoutLabel["Layout"]
        style LayoutLabel fill:transparent,stroke:none;
        space:1

        Components
        space
        Virtual

        Components --> Virtual
    end

    Components --> Controllers
    Components --> Automation
    Virtual --> Controllers

    Utils
```

## Controller Support

* Märklin Central Station 1 (60212) : _In progress_
* Märklin Central Station 2 (60215) : _Planned_
* DCC-Ex : _planned_

## Operating System Support

* Windows : _Done_
* Mac OS : _Done_
* Android : _Done_
* Linux : _Planned_

## Getting Started

To get started, navigate to the <img src="./res/icons/light/misc/gear.svg" alt="gear" width="16" height="16">_settings_ tab and select <img src="./res/icons/light/misc/plus.svg" alt="plus" width="16" height="16">_Add Controller_.

Select the controller model from the drop-down. The default protocol and connection settings will automatically populate. If you wish to change them, do so here. Note that the COM port and IP address fields must always be manually entered. Press OK to continue.

The app will automatically populate the trains, actuators, routes and sensors configured on your controller. Any changes you make to this configuration will be pushed back to the controller.

## Contributors

### Localization Team

* [Long Dương](https://github.com/longd1999) : Vietnamese

### Other Credits

App icon by [Rose Spencer-Spreeuw](https://www.linkedin.com/in/rose-spencer-spreeuw-82278a1a1/).
