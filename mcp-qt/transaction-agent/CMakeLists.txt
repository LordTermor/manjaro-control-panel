# ============================================================================
# MCP Transaction Agent - Standalone transaction executor
# ============================================================================

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(QCoro6 REQUIRED COMPONENTS Core)

add_executable(mcp-transaction-agent
    main.cpp
    AgentUi.cpp
    AgentUi.h
)

# Enable Qt MOC for the target
set_target_properties(mcp-transaction-agent PROPERTIES
    AUTOMOC ON
)

target_compile_definitions(mcp-transaction-agent PRIVATE QT_NO_KEYWORDS)

# MCP version - propagated from root CMakeLists.txt
target_compile_definitions(mcp-transaction-agent PRIVATE
    MCP_VERSION="${MCP_VERSION}"
)

target_link_libraries(mcp-transaction-agent
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        QCoro6::Core
        libmcp
)

target_include_directories(mcp-transaction-agent
    PRIVATE
        ${CMAKE_SOURCE_DIR}/libmcp
)

# Install to bin directory
install(TARGETS mcp-transaction-agent
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
