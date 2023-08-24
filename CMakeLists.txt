cmake_minimum_required(VERSION 3.20)

project(libqxt)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED On)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(QXT_STATIC "Build QXT as a static library" Off)
set(QXT_QT_MAJOR_VERSION "5" CACHE STRING "Major Qt version to use (5/6)")

if (QXT_STATIC)
    set(QXT_LIB_TYPE STATIC)
else()
    set(QXT_LIB_TYPE SHARED)
endif()

find_package(Qt${QXT_QT_MAJOR_VERSION} REQUIRED 
    COMPONENTS 
        Core
    OPTIONAL_COMPONENTS
        Widgets
        Network
        WebSockets
        Designer
        Sql
)
find_package(DNSSD)

add_subdirectory(src)
