cmake_minimum_required(VERSION 3.23)
project(filestorage CXX)

set(CMAKE_CXX_STANDARD 20)

if (DEFINED PACKAGE_VERSION)
    add_compile_definitions(VERSION="${PACKAGE_VERSION}")
endif ()

if (DEFINED APPNAME)
    add_compile_definitions(APPNAME="${APPNAME}")
endif ()

add_executable(filestorage main.cpp server.h server.cpp constants.hpp)
