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

#include "DeviceScanner.hpp"

struct udev_device;

namespace mcp::mhwd {

/**
 * Scans PCI bus for devices using udev.
 */
class PciDeviceScanner : public DeviceScanner<PciDeviceScanner> {
public:
    using DeviceScanner<PciDeviceScanner>::scan;

private:
    friend class DeviceScanner<PciDeviceScanner>;
    
    static constexpr const char* subsystem() { return "pci"; }
    static constexpr BusType bus_type() { return BusType::PCI; }
    
    static DeviceInfo extract_info(udev_device* device, const char* syspath);
    static bool is_valid(udev_device* device);
};

} // namespace mcp::mhwd
