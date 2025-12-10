/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DeviceProvider - hardware detection using udev.
 * Simple wrapper that queries PCI and USB buses.
 */

#pragma once

#include "../Types.hpp"
#include "Device.hpp"

namespace mcp::mhwd {

/**
 * Hardware device provider using udev.
 * 
 * Scans PCI and USB buses to detect installed hardware.
 * Results are cached - call scan() to refresh.
 * 
 * Usage:
 *   DeviceProvider provider;
 *   co_await provider.scan();
 *   for (const auto& dev : provider.pci_devices()) {
 *       fmt::print("{}: {}\n", dev.vendor_name(), dev.device_name());
 *   }
 */
class DeviceProvider {
public:
    DeviceProvider() = default;

    /**
     * Get PCI devices (cached, call scan() to refresh).
     */
    [[nodiscard]] const DeviceVector& pci_devices() const { return pci_devices_; }

    /**
     * Get USB devices (cached, call scan() to refresh).
     */
    [[nodiscard]] const DeviceVector& usb_devices() const { return usb_devices_; }

    /**
     * Get all devices (PCI + USB).
     */
    [[nodiscard]] DeviceVector all_devices() const;

    /**
     * Scan hardware and update cache.
     */
    Task<void> scan();

private:
    DeviceVector pci_devices_;
    DeviceVector usb_devices_;
};

} // namespace mcp::mhwd
