/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * PCI bus device scanner.
 * Extracts PCI device information from udev.
 */

#pragma once

#include "Types.hpp"
#include "../Device.hpp"

#include <vector>

struct udev_device;

namespace mcp::mhwd {

/**
 * Scans PCI bus for devices using udev.
 */
class PciDeviceScanner {
public:
    /**
     * Scan PCI bus and return all valid devices.
     */
    static std::vector<Device> scan();

private:
    static constexpr const char* subsystem() { return "pci"; }
    
    static DeviceInfo extract_info(udev_device* device, const char* syspath);
    static bool is_valid(udev_device* device);
};

} // namespace mcp::mhwd
