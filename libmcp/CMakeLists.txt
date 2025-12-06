# ============================================================================
# libmcp - Core library for Manjaro Control Panel
# Provides kernel management and package transaction utilities.
# ============================================================================
cmake_minimum_required(VERSION 3.25...4.2)

include(FetchContent)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0 gobject-2.0 gio-2.0)
pkg_check_modules(SIGCXX REQUIRED IMPORTED_TARGET sigc++-3.0)

# libcoro - C++20 coroutine library
FetchContent_Declare(
    libcoro
    GIT_REPOSITORY https://github.com/jbaldwin/libcoro.git
    GIT_TAG f671edb4cb66276fa1c2f61d70c4d5a8b18c4076
)

set(LIBCORO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LIBCORO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libcoro)

# libmcp - main library with ProgressFlattener and kernel utilities
add_library(libmcp SHARED
    ProgressFlattener.cpp
    ProgressFlattener.hpp
)

set_target_properties(libmcp PROPERTIES
    OUTPUT_NAME mcp
)

target_link_libraries(libmcp
    PUBLIC
        PkgConfig::SIGCXX
        PkgConfig::GLIB
        pamac::pamac-cpp
)

target_include_directories(libmcp
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

install(TARGETS libmcp)

add_subdirectory(kernel)
