# ============================================================================
# mcp-qt/mhwd - Hardware driver management UI components
# ============================================================================

find_package(QCoro6 REQUIRED COMPONENTS Core Qml)

# Create OBJECT library to share code between QML module, standalone, and KCM
add_library(mcp-qt-mhwd-objects OBJECT
    DeviceData.h
    DeviceListModel.cpp
    DeviceListModel.h
    DriverData.h
    MhwdViewModel.cpp
    MhwdViewModel.h
)

target_include_directories(mcp-qt-mhwd-objects
    PUBLIC
        ${CMAKE_SOURCE_DIR}/libmcp
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mcp-qt-mhwd-objects
    PUBLIC
        Qt6::Core
        Qt6::Qml
        QCoro6::Core
        QCoro6::Qml
        libmcp
        libmcp-mhwd
        mcp-qt-common

)

# QML Module
ecm_add_qml_module(mcp-qt-mhwd-qmlmodule
    URI "org.manjaro.mcp.mhwd"
    VERSION 1.0
    GENERATE_PLUGIN_SOURCE
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/org/manjaro/mcp/mhwd
)

target_link_libraries(mcp-qt-mhwd-qmlmodule
    PUBLIC
        mcp-qt-mhwd-objects
        Qt6::Core
        Qt6::Qml
        libmcp
        libmcp-mhwd
)

ecm_finalize_qml_module(mcp-qt-mhwd-qmlmodule
    DESTINATION ${KDE_INSTALL_QMLDIR}
)

# ============================================================================
# Standalone executable for testing/development
# ============================================================================

add_executable(mcp-qt-mhwd
    standalone_main.cpp
    mhwd.qrc
)

target_link_libraries(mcp-qt-mhwd
    PRIVATE
        mcp-qt-mhwd-objects
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Qml
        KF6::I18n
        KF6::CoreAddons
        libmcp
        libmcp-mhwd
)

set_property(TARGET mcp-qt-mhwd PROPERTY CXX_STANDARD 23)

install(TARGETS mcp-qt-mhwd
    RUNTIME DESTINATION ${KDE_INSTALL_BINDIR}
)

# ============================================================================
# KCM module integration
# ============================================================================

add_subdirectory(kcm)
