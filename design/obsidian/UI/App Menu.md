## Overview
This outlines the structure of the Main App Menu. This is only implemented on platforms that have a native menu bar: Mac OS and some Linux desktop experiences. Implementation details can be found here in [[Native App Menu Bar]].

## Main Menu Structure
```mermaid
flowchart TD
	MainMenu["Main Menu"]
	Controllers
	Clock
	Help
	
	ControllerMenu["`_Controller Menu_`"]
	NewController["New Controller"]
	RefreshAll["Refresh All"]
	
	FastClock["Fast Clock"]
	AddAutomation["Add Automation"]
	
	Help2["Help"]
	About
	AboutQt["About Qt"]
	License["License Info"]
	ThirdParty["Third Party License & Credits"]
	
	MainMenu --> Controllers
	MainMenu --> Clock
	MainMenu --> Help
	
	Controllers --> ControllerMenu
	Controllers --> NewController
	Controllers --> RefreshAll
	
	Clock --> FastClock
	Clock --> AddAutomation
	
	Help --> Help2
	Help --> About
	Help --> AboutQt
	Help --> License
	Help --> ThirdParty
```

## Controller Menu
One controller sub-menu is added to the controller menu for each controller configured. An "Add Component" menu item is added for each component capability the controller has
```mermaid
flowchart TD
	ControllerName["Controller Name"]
	
	AddComponent["`_Add Component_`"]
	
	RefreshComponents["Refresh Components"]
	EStop["Emergency Stop"]
	
	Edit["Edit Controller"]
	Delete["Delete Controller"]
	
	ControllerName --> AddComponent
	ControllerName --> RefreshComponents
	ControllerName --> EStop
	ControllerName --> Edit
	ControllerName --> Delete
```

## Dock Menu
```mermaid
flowchart TD
	Dock
	
	FastClock["Fast Clock"]
	RefreshAll["Refresh All"]
	
	Dock --> FastClock
	Dock --> RefreshAll
```
