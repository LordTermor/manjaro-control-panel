/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/DeviceProvider.hpp"
#include "udev/PciDeviceScanner.hpp"
#include "udev/UsbDeviceScanner.hpp"

/*
 * Hardware detection coordinator.
 * Delegates actual scanning to DeviceScanner specializations.
 */

namespace mcp::mhwd {

DeviceVector DeviceProvider::all_devices() const
{
    DeviceVector all;
    all.reserve(pci_devices_.size() + usb_devices_.size());

    all.insert(all.end(), pci_devices_.begin(), pci_devices_.end());
    all.insert(all.end(), usb_devices_.begin(), usb_devices_.end());

    return all;
}

Task<void> DeviceProvider::scan()
{
    // udev scanning is fast local I/O, wrapped in coroutine for API consistency
    pci_devices_ = PciDeviceScanner::scan();
    usb_devices_ = UsbDeviceScanner::scan();
    co_return;
}

} // namespace mcp::mhwd
