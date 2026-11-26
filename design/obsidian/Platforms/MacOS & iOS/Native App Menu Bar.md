## Overview
It is normal and expected on Mac that applications have a native app menu bar. As the app's UX was designed to be mobile first, most of this functionality was concentrating in the settings tab. However, it is quite easy to open the same dialog boxes from menu items. Additionally, the [`QMenuBar`](https://doc.qt.io/qt-6/qmenubar.html) class supports acting as a native menu bar by setting the [`nativeMenuBar`](https://doc.qt.io/qt-6/qmenubar.html#nativeMenuBar-prop) property. Therefore, no native code is needed to support this feature. Finally, this also works on Linux desktop environments like Qt that also implement a native menu bar, like KDE plasma.

## Detecting a Native Menu Bar
Since all this functionality already exists in the settings tab, I only want a menu bar if it's a platform norm. Originally I planned on using `ifdef` to detect this. Mac always has it. Windows and android never do. Linux does sometimes, which requires the use of [sd-bus](https://www.freedesktop.org/software/systemd/man/latest/sd-bus.html) to detect. However, Qt has an easier solution. According to their own documentation, `QMenuBar::isNativeMenuBar()` will always return false on platforms that don't support native menus, even if `QMenuBar::setNativeMenuBar(true)` is called. Thus, a simple function can be written to detect this.


