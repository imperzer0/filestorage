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

set(resource_files
        "config_lib.lua"
        "error.html"
        "explorer.html"
        "favicon.ico"
        "filestorageconf.lua"
        "index.html"
        "invalid_credentials.html"
        "uploader.html"
        )

# Creates C resources file from files in given directory
function(create_resources output)
    # Create empty output file
    file(WRITE ${output}
            "#ifndef FILESTORAGE_RESOURCES_HPP\n"
            "#define FILESTORAGE_RESOURCES_HPP\n"
            "\n"
            " #define LEN(name) name##_len\n"
            "\n")
    # Iterate through input files
    foreach (bin ${resource_files})
        message("[RC] Generating ${bin} resource...")
        file(REAL_PATH ${bin} freal)
        file(SIZE ${freal} fsize)
        # Get short filename
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(REGEX REPLACE "\\.| |-" "_" filename ${filename})
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        if (NOT "${fsize}" STREQUAL "0")
            # Convert hex data for C compatibility
            string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        endif ()
        # Append data to output file
        file(APPEND ${output}
                "\n"
                "#ifndef RESOURCE_${filename}\n"
                "#define RESOURCE_${filename}\n"
                " static constexpr const unsigned char ${filename}[] = {${filedata} 0};\n"
                " static constexpr const unsigned ${filename}_len = ${fsize};\n"
                "#endif\n"
                "\n")
    endforeach ()
    # Create empty output file
    file(APPEND ${output} "\n#endif //FILESTORAGE_RESOURCES_HPP")
endfunction()

create_resources("resources.hpp")


message(" [ compiler ]  ${CMAKE_CXX_COMPILER}_${CMAKE_CXX_STANDARD} ${CMAKE_CXX_FLAGS}")

set(CMAKE_VERBOSE_MAKEFILE on)

add_executable(filestorage main.cpp server.cpp config_script.cpp lua_config_libfunctions.cpp Gen_QR.cpp zip_dir.cpp layer.cpp matrix.cpp perceptron.cpp)

target_link_libraries(filestorage mariadb mariadbcpp lua pthread zip)
