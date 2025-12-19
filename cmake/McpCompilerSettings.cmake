# ============================================================================
# MCP Compiler Settings - C++ standard and hardening flags
# Applies C++23 standard and hardening compiler flags globally.
# Includes strict warnings for better code quality.
# ============================================================================

set(CMAKE_CXX_STANDARD 23 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "C++ standard required")
set(CMAKE_CXX_EXTENSIONS OFF CACHE BOOL "Disable compiler extensions")

add_compile_options(
    -Wall
    -Wextra
    -Wconversion
    -Woverloaded-virtual
)
