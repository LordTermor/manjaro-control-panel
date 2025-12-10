# ============================================================================
# libmcp kernel library - Kernel package discovery and management
# ============================================================================

add_library(libmcp-kernel SHARED
    Kernel.hpp
    KernelProvider.hpp
    KernelProvider.cpp
    Transaction.hpp
    Transaction.cpp
)

set_target_properties(libmcp-kernel PROPERTIES
    OUTPUT_NAME mcp-kernel
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
