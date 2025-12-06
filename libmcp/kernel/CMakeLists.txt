# ============================================================================
# libmcp kernel library - Kernel package discovery and management
# ============================================================================

add_library(libmcp-kernel SHARED
    Kernel.hpp
    KernelProvider.hpp
    KernelProvider.cpp
    KernelManager.hpp
    KernelManager.cpp
)

set_target_properties(libmcp-kernel PROPERTIES
    OUTPUT_NAME mcp-kernel
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(libmcp-kernel 
    PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/..
)

target_link_libraries(libmcp-kernel
    PUBLIC
        libcoro
        pamac::pamac-cpp
)

install(TARGETS libmcp-kernel)
