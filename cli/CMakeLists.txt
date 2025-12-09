# ============================================================================
# CLI tools for Manjaro Control Panel
# ============================================================================

include(FetchContent)

# CLI11 - Command line parser (header-only, bundled, not installed to system)
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
    EXCLUDE_FROM_ALL
)

set(CLI11_BUILD_TESTS OFF CACHE BOOL "Disable CLI11 tests" FORCE)
set(CLI11_BUILD_EXAMPLES OFF CACHE BOOL "Disable CLI11 examples" FORCE)

FetchContent_MakeAvailable(cli11)

add_subdirectory(common)
add_subdirectory(kernel)
add_subdirectory(mhwd)
