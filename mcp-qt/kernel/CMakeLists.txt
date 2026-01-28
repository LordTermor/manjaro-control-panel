# ============================================================================
# mcp-qt-kernel - Kernel management Qt/QML components
# ============================================================================

find_package(QCoro6 REQUIRED COMPONENTS Core)

# C++ ViewModels library (no QML files, used by QML module)
add_library(mcp-qt-kernel SHARED
    KernelData.h
    KernelListModel.cpp
    KernelListModel.h
    KernelViewModel.cpp
    KernelViewModel.h
)

target_link_libraries(mcp-qt-kernel
    PRIVATE
    libmcp-kernel
    mcp-qt-common
    QCoro6::Core
)

install(TARGETS mcp-qt-kernel COMPONENT Runtime)

# QML Module
mcp_add_qml_module(
    MODULE_NAME mcp-qt-kernel-qmlmodule
    URI "org.manjaro.mcp.kernel"
    DOMAIN kernel
    QML_FILES
        ui/View.qml
        ui/KernelListView.qml
        ui/KernelDelegate.qml
        ui/SelectedKernels.qml
        ui/ChangelogSheet.qml
        ui/standalone_main.qml
    CPP_BACKING_LIBS
        mcp-qt-kernel
        mcp-qt-common
)

# Standalone QML executable
if(MCP_BUILD_QT)
    add_executable(mcp-qt-kernels standalone_main.cpp standalone.qrc)

    target_link_libraries(mcp-qt-kernels
        PRIVATE
        mcp-qt-kernel
        libmcp-kernel
        mcp-qt-common
        Qt6::Quick
        Qt6::Qml
    )

    target_compile_definitions(mcp-qt-kernels PRIVATE
        MCP_VERSION="${MCP_VERSION}"
    )

    install(TARGETS mcp-qt-kernels COMPONENT Standalone)
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
        mcp-qt-kernel
        libmcp-kernel
        mcp-qt-common
        Qt6::Widgets
    )

    target_compile_definitions(mcp-qt-classic PRIVATE
        MCP_VERSION="${MCP_VERSION}"
    )

    install(TARGETS mcp-qt-classic COMPONENT Standalone)
endif()

# KCM module (in subfolder)
if(MCP_BUILD_KCM)
    add_subdirectory(kcm)
endif()
