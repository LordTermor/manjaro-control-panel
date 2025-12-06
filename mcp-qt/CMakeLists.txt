# ============================================================================
# mcp-qt - Qt/KDE GUI components for Manjaro Control Panel
# ============================================================================

# KCM plugins must be shared libraries, not static
set(BUILD_SHARED_LIBS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(KF_MIN_VERSION "6.20.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)

# Ensure QML modules install to the prefix
if(NOT KDE_INSTALL_QMLDIR)
    set(KDE_INSTALL_QMLDIR "${CMAKE_INSTALL_PREFIX}/lib/qt6/qml" CACHE PATH "Qt6 QML install directory")
endif()
include(ECMQmlModule)

find_package(OpenMP REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Quick Qml QuickWidgets QuickControls2 DBus)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Kirigami I18n CoreAddons Auth ConfigWidgets KCMUtils)

add_subdirectory(common)

add_subdirectory(transaction-agent)

add_subdirectory(modules)

# Install System Settings category
if(MCP_BUILD_KCM)
    install(FILES data/categories/settings-system-administration-mcp.desktop
            DESTINATION ${KDE_INSTALL_DATADIR}/systemsettings/categories)
endif()

# Install MCP icon for application and category
install(FILES data/assets/icon.svg
        DESTINATION ${KDE_INSTALL_ICONDIR}/hicolor/scalable/apps
        RENAME mcp.svg)