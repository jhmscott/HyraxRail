## Overview
Magic trackpad and similar products in MacBooks have both multi touch and gesture support. Apps like safari use this to navigate back and forth between pages. The `SwipeView` should be extended to support a similar UX.

## Qt Native Gestures
Qt has an event type, [`QNativeGestureEvent`](https://doc.qt.io/qt-6/qnativegestureevent.html), for trackpad gestures. The `SwipeView` action should map to the [`Qt::SwipeNativeGesture` ](https://doc.qt.io/qt-6/qt.html#NativeGestureType-enum)  gesture type. However, from my testing, this isn't generated on Mac. The other event types are, so I suspect this is a bug in the version of Qt I am using. However, even if this was working, this  provides a less responsive UX, as the widget will not respond until the gesture has been recognized. Ideally I want the `SwipeView` to move with the user's finger. Ultimately abandoned this solution

## Apple Native Swipe Events
The [[Terminology|AppKit]] framework provides an interface for tracking swipe events: [`trackSwipEvent()`](https://developer.apple.com/documentation/appkit/nsevent/trackswipeevent(options:dampenamountthresholdmin:max:usinghandler:)). While it is feasible to implement a native `NSView` in Qt via [`QWindow::fromWinId()`](https://doc.qt.io/qt-6.11/qtdoc-demos-windowembedding-example.html).  I would then need to imbed the [`QStackedWidget`](https://doc.qt.io/qt-6/qstackedwidget.html) within this native window. This adds significant complexity for a rather simple feature, so I ruled it out early on.

## Qt Wheel Events
By default, Qt translates all swipe events into a [`QWheelEvent`](https://doc.qt.io/qt-6/qwheelevent.html). Horizontal swipe events can be detected as they have a delta x value != 0. Furthermore, we can determine that this is coming from a trackpad gesture via the [deviceType](https://doc.qt.io/qt-6/qinputevent.html#deviceType) property, which should be `QInputDevice::DeviceType::TouchPad`.  This device type provides us with the scroll phase, which indicates when a scroll stops, updates and ends. It also natively calculates and provides the momentum via the `Qt::ScrollMomentum` phase. 

This met all of the requirements and thus was ultimately chosen as the solution. 

A couple considerations:
 * The momentum can be enough to throw us past the next widget. Thus the total swipe distance should be limited to \[-width, width\] 
 * If we don't process the `Qt::ScrollEnd`, we seem to stop getting the `Qt::ScrollBegin`. we need to keep processing new events even if we're reached the next widget
 * Unlike touch events, we only get a delta with each update, not the total distance. So we need to keep track in a member variable to maintain the same behaviour
 * Sometimes the delta value can be quite large, especially during the momentum phase if we have a very quick initial swipe. In these cases we should use the snap animation, rather than jumping straight to the new delta value