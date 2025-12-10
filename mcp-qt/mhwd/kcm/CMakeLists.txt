# ============================================================================
# KCM module for Hardware Driver management
# ============================================================================

message(STATUS "Configuring MHWD KCM...")

# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN="kcm_mcp_mhwd")

kcmutils_add_qml_kcm(kcm_mcp_mhwd SOURCES MhwdKCModule.cpp MhwdKCModule.h)

target_link_libraries(kcm_mcp_mhwd PRIVATE
    libmcp-mhwd
    mcp-qt-common
    mcp-qt-mhwd-objects
    
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::KCMUtilsQuick
    KF6::I18n
    Qt6::DBus
)

target_include_directories(kcm_mcp_mhwd PRIVATE
    ${CMAKE_SOURCE_DIR}/libmcp
    ${CMAKE_SOURCE_DIR}/mcp-qt/mhwd
)
