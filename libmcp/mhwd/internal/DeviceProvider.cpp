/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/DeviceProvider.hpp"
#include "PciDeviceScanner.hpp"
#include "UsbDeviceScanner.hpp"

/*
 * Hardware detection coordinator.
 * Delegates actual scanning to DeviceScanner specializations.
 */

namespace mcp::mhwd {

std::vector<Device> DeviceProvider::get_all_devices() const
{
    std::vector<Device> all;
    all.reserve(pci_devices_.size() + usb_devices_.size());

    all.insert(all.end(), pci_devices_.begin(), pci_devices_.end());
    all.insert(all.end(), usb_devices_.begin(), usb_devices_.end());

    return all;
}

void DeviceProvider::scan()
{
    pci_devices_ = PciDeviceScanner::scan();
    usb_devices_ = UsbDeviceScanner::scan();
}

} // namespace mcp::mhwd
