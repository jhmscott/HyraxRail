## Overview
**`Info.plist`** is the information property list, which serves a similar role to a manifest file. This hold information for the following:
 * App name, ID, and version
 * System Permissions
 * Capabilities
 * Network Security
An example of a basic **`Info.plist`**:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd"> <plist version="1.0">
	<dict> 
		<key>CFBundleDisplayName</key> 
		<string>My App</string> 
	</dict> 
</plist>
```

## Integrating with CMake
CMake will generate an **`Info.plist`** file automatically. However, it also exposes the [MACOSX_BUNDLE_INFO_PLIST](https://cmake.org/cmake/help/latest/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html) property that allows you to set a custom **`Info.plist`** file. This is used with [`set_target_properties()`](https://cmake.org/cmake/help/latest/command/set_target_properties.html) .