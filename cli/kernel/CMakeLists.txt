# ============================================================================
# mcp-kernel-cli - CLI tool for kernel management
# ============================================================================

include(FetchContent)

# CLI11 - Command line parser
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.4.2
)
FetchContent_MakeAvailable(cli11)

# Find fmtlib
find_package(fmt REQUIRED)

# Find sigc++ (required for pamac::Transaction)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SIGCPP REQUIRED IMPORTED_TARGET sigc++-3.0)

add_executable(mcp-kernel-cli
    main.cpp
)

target_link_libraries(mcp-kernel-cli
    PRIVATE
        libmcp-kernel
        pamac::pamac-cpp
        CLI11::CLI11
        fmt::fmt
        PkgConfig::SIGCPP
)

target_include_directories(mcp-kernel-cli
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(mcp-kernel-cli PRIVATE cxx_std_23)

set_target_properties(mcp-kernel-cli PROPERTIES
    OUTPUT_NAME "mcp-kernel"
)

install(TARGETS mcp-kernel-cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
