/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "PciDeviceScanner.hpp"
#include "UdevUtils.hpp"

namespace mcp::mhwd {

std::vector<Device> PciDeviceScanner::scan()
{
    using namespace udev;
    
    std::vector<Device> devices;

    UdevPtr udev_ctx(udev_new());
    if (!udev_ctx) {
        return devices;
    }

    UdevEnumeratePtr enumerate(udev_enumerate_new(udev_ctx.get()));
    if (!enumerate) {
        return devices;
    }

    udev_enumerate_add_match_subsystem(enumerate.get(), subsystem());
    udev_enumerate_scan_devices(enumerate.get());

    udev_list_entry* entry;
    udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(enumerate.get()))
    {
        const char* syspath = udev_list_entry_get_name(entry);
        UdevDevicePtr device(udev_device_new_from_syspath(udev_ctx.get(), syspath));
        
        if (!device || !is_valid(device.get())) {
            continue;
        }

        DeviceInfo info = extract_info(device.get(), syspath);
        devices.emplace_back(std::move(info), BusType::PCI);
    }

    return devices;
}

DeviceInfo PciDeviceScanner::extract_info(udev_device* device, const char* syspath)
{
    using namespace udev;
    
    unsigned long vendor = hex_to_ulong(udev_device_get_sysattr_value(device, "vendor"));
    unsigned long device_id = hex_to_ulong(udev_device_get_sysattr_value(device, "device"));
    unsigned long class_id = hex_to_ulong(udev_device_get_sysattr_value(device, "class"));

    return DeviceInfo{
        .class_id = to_hex((class_id >> 8) & 0xFFFF, 4),
        .vendor_id = to_hex(vendor, 4),
        .device_id = to_hex(device_id, 4),
        .class_name = safe_property(device, "ID_PCI_CLASS_FROM_DATABASE"),
        .vendor_name = safe_property(device, "ID_VENDOR_FROM_DATABASE"),
        .device_name = safe_property(device, "ID_MODEL_FROM_DATABASE"),
        .sysfs_bus_id = safe_string(udev_device_get_sysname(device)),
        .sysfs_id = safe_string(syspath)
    };
}

bool PciDeviceScanner::is_valid(udev_device* device)
{
    using namespace udev;
    
    unsigned long vendor = hex_to_ulong(udev_device_get_sysattr_value(device, "vendor"));
    unsigned long device_id = hex_to_ulong(udev_device_get_sysattr_value(device, "device"));
    return vendor != 0 || device_id != 0;
}

} // namespace mcp::mhwd
