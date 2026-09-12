cmake_minimum_required(VERSION 3.16)

project(cobalt
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(COBALT_SOURCES
    src/cobalt/cobalt.c
    src/cobalt/tony.c
    src/cobalt/dtypes.c
    src/cobalt/Clist.c
    src/cobalt/Cstring.c
    src/cobalt/io.c
    src/cobalt/csv.c
)

# Static library
add_library(cobalt_static STATIC
    ${COBALT_SOURCES}
)

# Shared library
add_library(cobalt_shared SHARED
    ${COBALT_SOURCES}
)

# Public headers
target_include_directories(cobalt_static
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_include_directories(cobalt_shared
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Compiler warnings
target_compile_options(cobalt_static PRIVATE
    -Wall
    -Wextra
)

target_compile_options(cobalt_shared PRIVATE
    -Wall
    -Wextra
)

# Put libraries in build/lib
set_target_properties(cobalt_static cobalt_shared
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# --------------------------------------------------
# Tests
# --------------------------------------------------

option(COBALT_BUILD_TESTS "Build Cobalt tests" OFF)

if(COBALT_BUILD_TESTS)

    add_executable(cobalt_test
        test/test.c
    )

    target_link_libraries(cobalt_test PRIVATE
        cobalt_static
    )

    target_include_directories(cobalt_test PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

endif()