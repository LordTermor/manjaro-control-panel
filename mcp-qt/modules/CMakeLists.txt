# ============================================================================
# MCP Qt Modules - KCM and standalone modules
# ============================================================================

# Kernel module is at mcp-qt/kernel, not under modules/
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../kernel ${CMAKE_CURRENT_BINARY_DIR}/kernel)