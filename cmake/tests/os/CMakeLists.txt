cmake_minimum_required(VERSION 3.16)

project(OsTest LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5
    REQUIRED COMPONENTS
    Test
    Core
    Widgets
    Gui
    Network
    SerialPort)

find_package(Qt${QT_VERSION_MAJOR}
    REQUIRED COMPONENTS
    Test
    Core
    Widgets
    Gui
    Network
    SerialPort)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(OsTest ../../../tests/cpp/utils/ostest.cpp)
add_test(NAME OsTest COMMAND OsTest)


target_include_directories(OsTest
    PRIVATE ../../../src/cpp ../../../tests/cpp)


set(DEPENDENCIES
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt6::Network
    Qt6::SerialPort
    Qt6::Test
    HyraxRailTestCore)

if (ANDROID)
    list(APPEND DEPENDENCIES android log)
endif ()

target_link_libraries(OsTest PRIVATE ${DEPENDENCIES})

include(GNUInstallDirs)
install(TARGETS OsTest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
