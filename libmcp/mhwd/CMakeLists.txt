# ============================================================================
# libmcp-mhwd - Hardware device detection and driver configuration management
# ============================================================================

find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED IMPORTED_TARGET libudev)

add_library(libmcp-mhwd SHARED
    internal/Device.cpp
    internal/DeviceProvider.cpp
    internal/Config.cpp
    internal/ConfigProvider.cpp
    internal/Transaction.cpp
    internal/udev/PciDeviceScanner.cpp
    internal/udev/UsbDeviceScanner.cpp
)

set_target_properties(libmcp-mhwd PROPERTIES
    OUTPUT_NAME mcp-mhwd
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(libmcp-mhwd
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/internal
)

target_link_libraries(libmcp-mhwd
    PUBLIC
        libcoro
        pamac::pamac-cpp
    PRIVATE
        PkgConfig::UDEV
)

install(TARGETS libmcp-mhwd COMPONENT Runtime)
