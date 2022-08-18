cmake_minimum_required(VERSION 3.23)
project(filestorage CXX)

set(CMAKE_CXX_STANDARD 20)

if (DEFINED PACKAGE_VERSION)
    add_compile_definitions(VERSION="${PACKAGE_VERSION}")
endif ()

if (DEFINED APPNAME)
    add_compile_definitions(APPNAME="${APPNAME}")
endif ()

if (NOT ("${CMAKE_BUILD_TYPE}" STREQUAL "Release"))
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
    message("Disabled optimization flags for use in profiler.")
else ()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
    message("Enabled optimization compiler flags.")
endif ()

add_executable(filestorage main.cpp server.h server.cpp constants.hpp resources.hpp)

target_link_libraries(filestorage mariadb mariadbcpp)