# ============================================================================
# KCM module for Kernel management
# ============================================================================

message(STATUS "Configuring Kernel KCM...")

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN="kcm_mcp_kernel")

kcmutils_add_qml_kcm(kcm_mcp_kernel SOURCES KernelKCModule.cpp KernelKCModule.h)

target_link_libraries(kcm_mcp_kernel PRIVATE
    libmcp-kernel
    mcp-qt-common
    mcp-qt-page-kernel
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::KCMUtilsQuick
    KF6::I18n
    Qt6::DBus
    pamac::pamac-cpp
)
