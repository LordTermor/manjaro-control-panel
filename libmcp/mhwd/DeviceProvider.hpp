/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DeviceProvider - hardware detection using libhd.
 * Simple wrapper that queries PCI and USB buses.
 */

#pragma once

#include "Device.hpp"

#include <vector>

namespace mcp::mhwd {

/**
 * Hardware device provider using libhd (hwinfo library).
 * 
 * Scans PCI and USB buses to detect installed hardware.
 * Results are cached - call scan() to refresh.
 * 
 * Usage:
 *   DeviceProvider provider;
 *   auto devices = provider.get_pci_devices();
 *   for (const auto& dev : devices) {
 *       fmt::print("{}: {}\n", dev.info().vendor_name, dev.info().device_name);
 *   }
 */
class DeviceProvider {
public:
    DeviceProvider() = default;

    /**
     * Get PCI devices (cached, call scan() to refresh).
     */
    [[nodiscard]] const std::vector<Device>& get_pci_devices() const { return pci_devices_; }

    /**
     * Get USB devices (cached, call scan() to refresh).
     */
    [[nodiscard]] const std::vector<Device>& get_usb_devices() const { return usb_devices_; }

    /**
     * Get all devices (PCI + USB).
     */
    [[nodiscard]] std::vector<Device> get_all_devices() const;

    /**
     * Scan hardware and update cache.
     */
    void scan();

private:
    std::vector<Device> pci_devices_;
    std::vector<Device> usb_devices_;
};

} // namespace mcp::mhwd
