# ============================================================================
# mcp-qt-common - Shared Qt components for MCP GUI
# ============================================================================

add_library(mcp-qt-common SHARED)

set_target_properties(mcp-qt-common PROPERTIES
    AUTOMOC ON
)

find_package(fmt REQUIRED)

target_link_libraries(mcp-qt-common
    PUBLIC
    libmcp
    fmt::fmt
    Qt6::Widgets
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickWidgets
    Qt6::QuickControls2
    KF6::KirigamiPlatform
    KF6::I18n
    KF6::CoreAddons
    KF6::AuthCore
)

target_include_directories(mcp-qt-common
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(mcp-qt-common PUBLIC QT_NO_KEYWORDS)

# MCP version - propagated from root CMakeLists.txt
target_compile_definitions(mcp-qt-common PUBLIC
    MCP_VERSION="${MCP_VERSION}"
    MCP_VERSION_MAJOR=${MCP_VERSION_MAJOR}
    MCP_VERSION_MINOR=${MCP_VERSION_MINOR}
    MCP_VERSION_PATCH=${MCP_VERSION_PATCH}
)

# Define path to transaction agent executable
target_compile_definitions(mcp-qt-common PRIVATE
    MCP_TRANSACTION_AGENT_PATH="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/mcp-transaction-agent"
)

add_subdirectory(qml)

add_subdirectory(src)

install(TARGETS mcp-qt-common)
