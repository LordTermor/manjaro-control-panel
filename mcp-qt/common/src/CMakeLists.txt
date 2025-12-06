target_sources(mcp-qt-common PRIVATE
    TransactionProgress.h
    ModuleDescription.h
    ProgressNotifier.h
    ProgressNotifier.cpp
    TransactionAgentLauncher.h
    TransactionAgentLauncher.cpp
    VersionInfo.h
    VersionInfo.cpp
)

target_include_directories(mcp-qt-common PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})
