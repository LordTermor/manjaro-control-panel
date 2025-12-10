/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "UsbDeviceScanner.hpp"
#include "UdevUtils.hpp"

#include <cstring>
#include <string_view>

namespace mcp::mhwd {

DeviceInfo UsbDeviceScanner::extract_info(udev_device* device, const char* syspath)
{
    using namespace udev;
    
    unsigned long vendor = hex_to_ulong(udev_device_get_sysattr_value(device, "idVendor"));
    unsigned long product = hex_to_ulong(udev_device_get_sysattr_value(device, "idProduct"));
    unsigned long dev_class = hex_to_ulong(udev_device_get_sysattr_value(device, "bDeviceClass"));
    unsigned long dev_subclass = hex_to_ulong(udev_device_get_sysattr_value(device, "bDeviceSubClass"));

    // USB composite devices report class 0 at device level - extract from first interface
    if (dev_class == 0) {
        const char* interfaces = udev_device_get_property_value(device, "ID_USB_INTERFACES");
        if (interfaces && interfaces[0] == ':' && strlen(interfaces) >= 7) {
            // Format: ":CCSSPP:..." where CC=class, SS=subclass, PP=protocol
            std::string_view iface_str(interfaces + 1, 6);
            dev_class = hex_to_ulong(std::string(iface_str.substr(0, 2)).c_str());
            dev_subclass = hex_to_ulong(std::string(iface_str.substr(2, 2)).c_str());
        }
    }

    std::string device_name = safe_property(device, "ID_MODEL_FROM_DATABASE");
    if (device_name.empty()) {
        device_name = safe_property(device, "ID_MODEL");
    }

    return DeviceInfo{
        .class_id = to_hex((dev_class << 8) | dev_subclass, 4),
        .vendor_id = to_hex(vendor, 4),
        .device_id = to_hex(product, 4),
        .class_name = safe_property(device, "ID_USB_CLASS_FROM_DATABASE"),
        .vendor_name = safe_property(device, "ID_VENDOR_FROM_DATABASE"),
        .device_name = std::move(device_name),
        .sysfs_bus_id = safe_string(udev_device_get_sysname(device)),
        .sysfs_id = safe_string(syspath),
        .driver = safe_string(udev_device_get_driver(device))
    };
}

bool UsbDeviceScanner::is_valid(udev_device* device)
{
    using namespace udev;
    
    unsigned long vendor = hex_to_ulong(udev_device_get_sysattr_value(device, "idVendor"));
    unsigned long product = hex_to_ulong(udev_device_get_sysattr_value(device, "idProduct"));
    return vendor != 0 || product != 0;
}

} // namespace mcp::mhwd
