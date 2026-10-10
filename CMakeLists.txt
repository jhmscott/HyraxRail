# Top level configuration and variables
cmake_minimum_required(VERSION 3.19)
project(HyraxRail
    VERSION 0.1
    LANGUAGES C CXX)

set(CMAKE_MACOSX_DEPLOYMENT_TARGET 10.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if(APPLE)
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_STANDARD 17)

    # External libraries
    find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
    find_library(COCOA_FRAMEWORK Cocoa REQUIRED)
endif()


# QT init

find_package(Qt6 6.11
    REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    Network
    SerialPort
    LinguistTools
    Concurrent
)

qt_standard_project_setup()

if (ANDROID)
    set(QT_ANDROID_GENERATE_JAVA_QTQUICKVIEW_CONTENTS FALSE)
endif()

# Scan src/cpp for headers and source

file(GLOB_RECURSE CPP_SOURCES "src/cpp/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/cpp/*.c")

file(GLOB_RECURSE CPP_HEADERS "src/cpp/*.hpp")
file(GLOB_RECURSE C_HEADERS "src/cpp/*.h")

set(SOURCES ${CPP_HEADERS} ${C_HEADERS} ${CPP_SOURCES} ${C_SOURCES})

if(APPLE)
    file(GLOB_RECURSE O_CPP_SOURCES "src/objective-c/*.mm")
    file(GLOB_RECURSE O_CPP_HEADERS "src/objective-c/*.hpp")

    list (APPEND SOURCES ${O_CPP_SOURCES} ${O_CPP_HEADERS})
endif()


# Setup translation files

file(GLOB TRANSLATIONS "res/translations/*.ts")

qt_add_translations(HyraxRail
    TS_FILES ${TRANSLATIONS}
    SOURCES ${SOURCES}
    QM_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/res/translations)

# Runs lupdate and lrelease with each build
add_custom_target(ALL DEPENDS
# TODO: For now, leave lupdate to windows
#    HyraxRail_lupdate
    HyraxRail_lrelease)

# Build resource files

qt_add_resources(SOURCES res/HyraxRail.qrc)

if(APPLE)
    # Add Mac App icon
    set(MACOSX_BUNDLE_ICON_FILE conductor-hyrax.icns)
    set(APP_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/res/icons/app/${MACOSX_BUNDLE_ICON_FILE}")
    set_source_files_properties(${APP_ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

set(DEPENDENCIES
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt6::Network
    Qt6::SerialPort
    Qt6::Concurrent)

if (ANDROID)
    list(APPEND DEPENDENCIES android log)
endif ()

# Static library target for unit testing

add_library(HyraxRailTestCore STATIC ${SOURCES})
target_link_libraries(HyraxRailTestCore PUBLIC ${DEPENDENCIES})


target_include_directories(HyraxRailTestCore
    PRIVATE src/cpp ${CMAKE_SOURCE_DIR})
target_precompile_headers(HyraxRailTestCore PRIVATE src/cpp/stdafx.h)

# Actual executable target

qt_add_executable(HyraxRail
    WIN32 MACOSX_BUNDLE
    ${SOURCES}
    ${APP_ICON_PATH}
)

target_include_directories(HyraxRail
    PRIVATE src/cpp ${CMAKE_SOURCE_DIR})
target_precompile_headers(HyraxRail PRIVATE src/cpp/stdafx.h)

target_link_libraries(HyraxRail
    PRIVATE ${DEPENDENCIES})


install(TARGETS HyraxRail
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET HyraxRail
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# Unit Test Sub-projects

# Common tests
add_subdirectory (cmake/tests/common/)

# Utilities tests
add_subdirectory (cmake/tests/algorithm/)
add_subdirectory (cmake/tests/device/)
add_subdirectory (cmake/tests/log/)
add_subdirectory (cmake/tests/math/)
add_subdirectory (cmake/tests/os/)
add_subdirectory (cmake/tests/pp/)
add_subdirectory (cmake/tests/qobj/)
add_subdirectory (cmake/tests/string/)
add_subdirectory (cmake/tests/traits/)

# Test utilities tests
add_subdirectory (cmake/tests/assert/)
add_subdirectory (cmake/tests/logspy/)
add_subdirectory (cmake/tests/testlang/)
add_subdirectory (cmake/tests/ui/)

# Layout tests
add_subdirectory (cmake/tests/actuator/)
add_subdirectory (cmake/tests/layoutbase/)
add_subdirectory (cmake/tests/locomotive/)
add_subdirectory (cmake/tests/vestop/)
add_subdirectory (cmake/tests/vroute/)

# Control controller tests
add_subdirectory (cmake/tests/controllerbase/)
add_subdirectory (cmake/tests/controllermeta/)
add_subdirectory (cmake/tests/marklincs1/)


# Top level UI tests
add_subdirectory (cmake/tests/lang/)
add_subdirectory (cmake/tests/mainwindow/)

# Common UI Tests
add_subdirectory (cmake/tests/gesture/)
add_subdirectory (cmake/tests/optionaldropdown/)
add_subdirectory (cmake/tests/schemeicon/)
add_subdirectory (cmake/tests/tiereddropdown/)
add_subdirectory (cmake/tests/uiutils/)

