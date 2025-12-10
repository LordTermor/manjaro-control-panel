# ============================================================================
# KCM module for About MCP
# ============================================================================

message(STATUS "Configuring About KCM...")

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN="kcm_mcp_about")

kcmutils_add_qml_kcm(kcm_mcp_about SOURCES AboutKCModule.cpp AboutKCModule.h about.qrc)

target_link_libraries(kcm_mcp_about PRIVATE
    mcp-qt-common
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::KCMUtilsQuick
    KF6::I18n
)
