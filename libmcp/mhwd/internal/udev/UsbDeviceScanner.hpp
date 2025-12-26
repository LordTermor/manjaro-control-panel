/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * USB bus device scanner.
 * Extracts USB device information from udev.
 */

#pragma once

#include "DeviceScanner.hpp"

struct udev_device;

namespace mcp::mhwd {

/**
 * Scans USB bus for devices using udev.
 */
class UsbDeviceScanner : public DeviceScanner<UsbDeviceScanner> {
public:
    using DeviceScanner<UsbDeviceScanner>::scan;

private:
    friend class DeviceScanner<UsbDeviceScanner>;
    
    static constexpr const char* subsystem() { return "usb"; }
    static constexpr BusType bus_type() { return BusType::USB; }
    
    static DeviceInfo extract_info(udev_device* device, const char* syspath);
    static bool is_valid(udev_device* device);
};

} // namespace mcp::mhwd
