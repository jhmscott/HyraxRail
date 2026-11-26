## Overview
The `NotificationCenter` class is a singleton that manages notifications for the application. This interface has the following goals:
 *  **Granular Control** : A path format is defined for each notification type, allowing whole categories of notifications to be enabled/disabled
 * **Persistence of Data** : Integrates with the settings framework to store user's notification preferences
 * **Platform Independent Interface** : Single interface for the app to send notification agnostic of the platform

## Path
Notification keys will use a path format. This allows notification types to be organized into a tree. Clients can disable an entire category of notifications by disabling any component of that path.

## Platform Specific Implementation
Qt provides the [`QSystemTrayIcon`](https://doc.qt.io/qt-6/qsystemtrayicon.html#show) which, on most desktop operating systems, has an interface for notifications. This covers the following operating systems:
 * Windows
 * MacOS
 * Most Linux platforms
Mobile operating systems will need a custom implementation:
 * [[Notifications|iOS]]
 * Android
