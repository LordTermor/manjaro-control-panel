# ============================================================================
# mcp-kernel-cli - CLI tool for kernel management
# ============================================================================

find_package(fmt REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SIGCPP REQUIRED IMPORTED_TARGET sigc++-3.0)

add_executable(mcp-kernel-cli
    main.cpp
)

target_link_libraries(mcp-kernel-cli
    PRIVATE
        libmcp-kernel
        mcp-cli-common
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

set_target_properties(mcp-kernel-cli PROPERTIES
    OUTPUT_NAME "mcp-kernel"
)

install(TARGETS mcp-kernel-cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
