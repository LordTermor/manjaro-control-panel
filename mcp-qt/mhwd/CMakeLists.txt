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
mcp_add_qml_module(
    MODULE_NAME mcp-qt-mhwd-qmlmodule
    URI "org.manjaro.mcp.mhwd"
    DOMAIN mhwd
    QML_FILES
        ui/UIManager.qml
        ui/ErrorDialog.qml
        ui/standalone_main.qml
        ui/View.qml
        ui/CategorySection.qml
        ui/DeviceCard.qml
        ui/DriverDetailsPanel.qml
        ui/DriverItem.qml
        ui/ViewModelManager.qml
        ui/DialogManager.qml
        ui/DriverStateManager.qml
    CPP_BACKING_LIBS
        mcp-qt-mhwd-objects
        Qt6::Core
        Qt6::Qml
        libmcp
        libmcp-mhwd
)

# ============================================================================
# Standalone executable for testing/development
# ============================================================================

add_executable(mcp-qt-mhwd
    standalone_main.cpp
    standalone.qrc
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
    COMPONENT Standalone
)

# ============================================================================
# KCM module integration
# ============================================================================

add_subdirectory(kcm)
