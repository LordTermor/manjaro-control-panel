# ============================================================================
# mcp-qt-page-kernel - Kernel management Qt/QML components
# ============================================================================

find_package(QCoro6 REQUIRED COMPONENTS Core)

add_library(mcp-qt-page-kernel SHARED
    KernelData.h
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
    QCoro6::Core
)

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

    target_compile_definitions(mcp-qt PRIVATE
        MCP_VERSION="${MCP_VERSION}"
    )

    install(TARGETS mcp-qt)
endif()

# Classic Qt Widgets standalone executable
if(MCP_BUILD_QT_CLASSIC)
    add_executable(mcp-qt-classic
        classic/main.cpp
        classic/KernelPage.cpp
        classic/KernelPage.h
        classic/KernelItemWidget.cpp
        classic/KernelItemWidget.h
        classic/BadgeWidget.cpp
        classic/BadgeWidget.h
    )

    target_link_libraries(mcp-qt-classic
        PRIVATE
        mcp-qt-page-kernel
        libmcp-kernel
        mcp-qt-common
        Qt6::Widgets
    )

    target_compile_definitions(mcp-qt-classic PRIVATE
        MCP_VERSION="${MCP_VERSION}"
    )

    install(TARGETS mcp-qt-classic)
endif()

# KCM module (in subfolder)
if(MCP_BUILD_KCM)
    add_subdirectory(kcm)
endif()
