# ============================================================================
# mcp-cli-common - Shared utilities for MCP CLI tools
# ============================================================================

find_package(fmt REQUIRED)

add_library(mcp-cli-common INTERFACE)

target_include_directories(mcp-cli-common
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(mcp-cli-common
    INTERFACE
        fmt::fmt
)
