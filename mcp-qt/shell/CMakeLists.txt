# ============================================================================
# mcp-qt shell - Unified standalone application combining all modules
# ============================================================================

find_package(QCoro6 REQUIRED COMPONENTS Core Qml)

add_executable(mcp-qt
    main.cpp
    shell.qrc
)

target_link_libraries(mcp-qt
    PRIVATE
        mcp-qt-kernel
        mcp-qt-mhwd-objects
        mcp-qt-common
        libmcp-kernel
        libmcp-mhwd
        libmcp
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Qml
        KF6::I18n
        KF6::CoreAddons
        QCoro6::Core
        QCoro6::Qml
)

target_compile_definitions(mcp-qt PRIVATE
    MCP_VERSION="${MCP_VERSION}"
)

install(TARGETS mcp-qt
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
    COMPONENT Standalone
)
