# ============================================================================
# mcp-qt-page-kernel - Kernel management Qt/QML components
# ============================================================================

add_library(mcp-qt-page-kernel SHARED
    KernelListModel.cpp
    KernelListModel.h
    KernelViewModel.cpp
    KernelViewModel.h
    kernel.qrc
)

target_link_libraries(mcp-qt-page-kernel
    PRIVATE
    libmcp-kernel
    mcp-qt-common
)
target_compile_definitions(mcp-qt-page-kernel PUBLIC QT_NO_KEYWORDS)

set_property(TARGET mcp-qt-page-kernel PROPERTY CXX_STANDARD 23)

install(TARGETS mcp-qt-page-kernel)

# Standalone QML executable
if(MCP_BUILD_QT)
    add_executable(mcp-qt standalone_main.cpp)

    target_link_libraries(mcp-qt
        PRIVATE
        mcp-qt-page-kernel
        libmcp-kernel
        mcp-qt-common
        Qt6::Quick
        Qt6::Qml
    )

    target_compile_definitions(mcp-qt PUBLIC QT_NO_KEYWORDS)

    # MCP version for standalone app
    target_compile_definitions(mcp-qt PRIVATE
        MCP_VERSION="${MCP_VERSION}"
    )

    set_property(TARGET mcp-qt PROPERTY CXX_STANDARD 23)

    install(TARGETS mcp-qt)
endif()

# Classic Qt Widgets standalone executable
if(MCP_BUILD_QT_CLASSIC)
    add_executable(mcp-qt-classic
        classic/main.cpp
        classic/KernelPage.cpp
        classic/KernelPage.h
        classic/KernelCardWidget.cpp
        classic/KernelCardWidget.h
        classic/KernelListDelegate.cpp
        classic/KernelListDelegate.h
        classic/SelectedKernelsWidget.cpp
        classic/SelectedKernelsWidget.h
    )

    target_link_libraries(mcp-qt-classic
        PRIVATE
        mcp-qt-page-kernel
        libmcp-kernel
        mcp-qt-common
        Qt6::Widgets
    )

    target_compile_definitions(mcp-qt-classic PUBLIC QT_NO_KEYWORDS)

    # MCP version for classic app
    target_compile_definitions(mcp-qt-classic PRIVATE
        MCP_VERSION="${MCP_VERSION}"
    )

    set_property(TARGET mcp-qt-classic PROPERTY CXX_STANDARD 23)

    install(TARGETS mcp-qt-classic)
endif()

# KCM module (in subfolder)
if(MCP_BUILD_KCM)
    add_subdirectory(kcm)
endif()
