# Top level configuration and variables
cmake_minimum_required(VERSION 3.19)
project(HyraxRail LANGUAGES C CXX OBJCXX)

set(CMAKE_MACOSX_DEPLOYMENT_TARGET 10.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_OBJCXX_STANDARD 17)


# External libraries
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(COCOA_FRAMEWORK Cocoa REQUIRED)

# QT init

find_package(Qt6 6.11
    REQUIRED COMPONENTS
    Core
    Widgets
    Gui
    Network
    SerialPort
    LinguistTools)

qt_standard_project_setup()

# Scan src/cpp for headers and source

file(GLOB_RECURSE CPP_SOURCES "src/cpp/*.cpp")
file(GLOB_RECURSE C_SOURCES "src/cpp/*.c")

file(GLOB_RECURSE CPP_HEADERS "src/cpp/*.hpp")
file(GLOB_RECURSE C_HEADERS "src/cpp/*.h")

file(GLOB_RECURSE O_CPP_SOURCES "src/objective-c/*.mm")
file(GLOB_RECURSE O_CPP_HEADERS "src/objective-c/*.hpp")

set(SOURCES ${CPP_HEADERS} ${C_HEADERS} ${CPP_SOURCES} ${C_SOURCES} ${O_CPP_SOURCES} ${O_CPP_HEADERS})

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

# create target
qt_add_executable(HyraxRail
    WIN32 MACOSX_BUNDLE
    ${SOURCES}
)

target_include_directories(HyraxRail
    PRIVATE src/cpp ${CMAKE_SOURCE_DIR})
target_precompile_headers(HyraxRail PRIVATE src/cpp/stdafx.h)

target_link_libraries(HyraxRail
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Gui
        Qt6::Network
        Qt6::SerialPort
)

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
