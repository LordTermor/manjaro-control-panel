cmake_minimum_required(VERSION 3.25...4.2)

project(mcp
    VERSION 0.1.0
    DESCRIPTION "Manjaro Control Panel"
    LANGUAGES CXX
)

# ============================================================================
# Main CMakeLists.txt for the Manjaro Control Panel (MCP) project.
# Sets up build options, common dependencies, and includes subprojects.
# ============================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Export compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(MCP_BUILD_LIB "Build core libraries (libmcp)" OFF)
option(MCP_BUILD_CLI "Build CLI tools" OFF)
option(MCP_BUILD_QT "Build Qt/QML standalone app (mcp-qt)" OFF)
option(MCP_BUILD_QT_CLASSIC "Build Qt Widgets classic app (mcp-qt-classic)" OFF)
option(MCP_BUILD_KCM "Build KDE System Settings modules" OFF)

# Version variables for downstream targets
set(MCP_VERSION ${PROJECT_VERSION})
set(MCP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(MCP_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(MCP_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# ============================================================================
# Dependencies via FetchContent
# ============================================================================

include(FetchContent)
include(GNUInstallDirs)

# libpamac-cpp - C++ wrapper for libpamac (bundled, not installed to system)
FetchContent_Declare(
    libpamac-cpp
    GIT_REPOSITORY https://github.com/LordTermor/libpamac-cpp.git
    GIT_TAG main
    EXCLUDE_FROM_ALL
)

# Build libpamac-cpp as a static library with bundled libpamac
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)
set(PAMAC_CPP_BUNDLE_LIBPAMAC ON CACHE BOOL "Bundle libpamac with libpamac-cpp" FORCE)
set(PAMAC_CPP_BUILD_EXAMPLES OFF CACHE BOOL "Don't build libpamac-cpp examples" FORCE)

FetchContent_MakeAvailable(libpamac-cpp)

# Restore BUILD_SHARED_LIBS for our own libraries
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)

# ============================================================================
# Subprojects
# ============================================================================

# Core library (required by CLI, Qt, and KCM)
if(MCP_BUILD_LIB OR MCP_BUILD_CLI OR MCP_BUILD_QT OR MCP_BUILD_QT_CLASSIC OR MCP_BUILD_KCM)
    add_subdirectory(libmcp)
endif()

# CLI tools
if(MCP_BUILD_CLI)
    add_subdirectory(cli)
endif()

# Qt/KDE GUI components (any Qt-based component)
if(MCP_BUILD_QT OR MCP_BUILD_QT_CLASSIC OR MCP_BUILD_KCM)
    add_subdirectory(mcp-qt)
endif()
