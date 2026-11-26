## Objective C
Apple notifications are managed using the [UNUserNotificationCenter](https://developer.apple.com/documentation/usernotifications/unusernotificationcenter?language=objc), which is part of the [[Terminology|Foundation]] framework. This requires implementation in Objective-C, with an interface exposed so the C++ code can use it. Like most of foundation, this is available on both Mac and iOS, so this will also support the eventual iOS port.

## Info.plist
In order to use the Notifications in your app, you [must have a proper bundle identifier](https://stackoverflow.com/questions/70585123/throwing-a-macos-notification-from-a-minimal-standalone-objective-c-program) in your app's [[Info.plist]]. 
