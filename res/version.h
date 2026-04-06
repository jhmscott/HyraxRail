/**
 * @file        version.h
 * @brief       Product version information
 * @author      Justin Scott
 * @date        2026-03-07
 *
 * @copyright   Copyright (c) 2026 Justin Scott
 */

#pragma once

#define __STRINGIFY(x)      #x
#define STRINGIFY(x)        __STRINGIFY (x)

#ifdef RC_INVOKED
#define COPYRIGHT_SYMBOL    "(C)"
#else
#define COPYRIGHT_SYMBOL    "\xC2\xA9"
#endif // RC_INVOKED

#define IDI_ICON1           101

#define VERSION_MAJOR       0
#define VERSION_MINOR       1
#define VERSION_MICRO       0
#define VERSION_PATCH       0

#define VERSION             VERSION_MAJOR,VERSION_MINOR,VERSION_MICRO,VERSION_PATCH
#define VERSION_STRING      STRINGIFY (VERSION_MAJOR) "." \
                            STRINGIFY (VERSION_MINOR) "." \
                            STRINGIFY (VERSION_MICRO) "." \
                            STRINGIFY (VERSION_PATCH)

#define PRODUCT_NAME_SHORT  "RHR"
#define PRODUCT_NAME_FULL   "Hyrax Rail"
#define PRODUCT_DESCRIPTION "Unified Model Train Control System"

#define COMPANY_DOMAIN      "justinlab.ca"
#define COMPANY_NAME        "JustinLab"
#define COPYRIGHT           "Copyright " COPYRIGHT_SYMBOL " 2026 Justin H. Scott"


