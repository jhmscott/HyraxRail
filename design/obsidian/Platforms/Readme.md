There are 4 main reasons for platform dependent code:
 1. There's a feature that can't be implemented wholely with Qt, and requires access to platform dependent APIs. An example of this is the Battery class.
 2. Qt has some undesirable behavior, or a bug on a given platform. An example of this is the style options in CenteredSlider.
 3. Qt provides some additional features on a certain platform. An example of this is the magic touchpad's gesture support.
 4. Some additional standard Qt code is required to achieve a UX norm on a platform. An example of this is the app menu bar on Mac OS.
In the case of 1, the code should be wrapped in some sort of interface to minimize exposure to the platform API. This should be implemented on as many platforms as possible.

This folder aims to document these changes and differences as much as possible.
