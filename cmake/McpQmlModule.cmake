# ============================================================================
# MCP QML Module Helpers - Declarative ECM QML module creation
# Provides helper functions for creating QML modules with consistent pattern.
# ============================================================================

#
# Creates a QML module using ECM with standard MCP conventions.
#
# Usage:
#   mcp_add_qml_module(
#       MODULE_NAME mcp-qt-kernel-qmlmodule
#       URI "org.manjaro.mcp.kernel"
#       DOMAIN kernel
#       QML_FILES ui/View.qml ui/KernelDelegate.qml
#       CPP_BACKING_LIBS mcp-qt-kernel mcp-qt-common
#   )
#
# Parameters:
#   MODULE_NAME - CMake target name for the QML module
#   URI - QML module URI (e.g., "org.manjaro.mcp.kernel")
#   DOMAIN - Domain name for output directory (e.g., "kernel" -> org/manjaro/mcp/kernel)
#   QML_FILES - List of QML files to include in the module
#   CPP_BACKING_LIBS - Optional C++ libraries to link (ViewModels, etc.)
#
function(mcp_add_qml_module)
    set(options "")
    set(oneValueArgs MODULE_NAME URI DOMAIN)
    set(multiValueArgs QML_FILES CPP_BACKING_LIBS)
    
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT ARG_MODULE_NAME)
        message(FATAL_ERROR "mcp_add_qml_module: MODULE_NAME is required")
    endif()
    
    if(NOT ARG_URI)
        message(FATAL_ERROR "mcp_add_qml_module: URI is required")
    endif()
    
    if(NOT ARG_DOMAIN)
        message(FATAL_ERROR "mcp_add_qml_module: DOMAIN is required")
    endif()
    
    if(NOT ARG_QML_FILES)
        message(FATAL_ERROR "mcp_add_qml_module: QML_FILES is required")
    endif()
    
    ecm_add_qml_module(${ARG_MODULE_NAME}
        URI "${ARG_URI}"
        VERSION 1.0
        GENERATE_PLUGIN_SOURCE
        OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/org/manjaro/mcp/${ARG_DOMAIN}
    )
    
    ecm_target_qml_sources(${ARG_MODULE_NAME} SOURCES ${ARG_QML_FILES})
    
    target_link_libraries(${ARG_MODULE_NAME}
        PRIVATE
            Qt6::Qml
            Qt6::Quick
            ${ARG_CPP_BACKING_LIBS}
    )
    
    # Note: ecm_finalize_qml_module doesn't support COMPONENT parameter
    # QML modules are installed via directory-based installation in PKGBUILD
    ecm_finalize_qml_module(${ARG_MODULE_NAME}
        DESTINATION ${KDE_INSTALL_QMLDIR}
    )
endfunction()
