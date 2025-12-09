# ============================================================================
# mcp-mhwd-cli - CLI tool for hardware driver management
# ============================================================================

find_package(fmt REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SIGCPP REQUIRED IMPORTED_TARGET sigc++-3.0)

add_executable(mcp-mhwd-cli
    main.cpp
    commands/list_command.cpp
    commands/install_command.cpp
    commands/remove_command.cpp
)

target_link_libraries(mcp-mhwd-cli
    PRIVATE
        libmcp
        libmcp-mhwd
        mcp-cli-common
        pamac::pamac-cpp
        CLI11::CLI11
        fmt::fmt
        PkgConfig::SIGCPP
)

target_include_directories(mcp-mhwd-cli
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(mcp-mhwd-cli PRIVATE cxx_std_23)

set_target_properties(mcp-mhwd-cli PROPERTIES
    OUTPUT_NAME "mcp-mhwd"
)

install(TARGETS mcp-mhwd-cli
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
