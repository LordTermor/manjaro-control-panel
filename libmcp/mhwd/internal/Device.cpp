/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "mhwd/Device.hpp"
#include "mhwd/Types.hpp"

#include <algorithm>
#include <vector>

/*
 * Device implementation with hardware pattern matching and categorization.
 */

namespace mcp::mhwd {

namespace {

bool matches_id(const std::string& device_id, const std::string& pattern)
{
    return pattern == "*" || pattern == device_id;
}

DeviceCategory categorize_pci(unsigned int base_class)
{
    switch (base_class) {
        case 0x03: return DeviceCategory::Graphics;
        case 0x02: return DeviceCategory::Network;
        case 0x04: return DeviceCategory::Audio;
        case 0x01: return DeviceCategory::Storage;
        case 0x09: return DeviceCategory::Input;
        default:   return DeviceCategory::Unknown;
    }
}

DeviceCategory categorize_usb(unsigned int base_class)
{
    // USB class codes differ from PCI
    switch (base_class) {
        case 0x03: return DeviceCategory::Input;    // HID (Human Interface Devices)
        case 0x02: return DeviceCategory::Network;  // Communications
        case 0x01: return DeviceCategory::Audio;    // Audio
        case 0x08: return DeviceCategory::Storage;  // Mass Storage
        case 0x0E: return DeviceCategory::Graphics; // Video
        default:   return DeviceCategory::Unknown;
    }
}

DeviceCategory categorize_from_class_id(const std::string& class_id, BusType bus_type)
{
    if (class_id.length() < 2) {
        return DeviceCategory::Unknown;
    }
    
    unsigned long class_code = std::stoul(class_id, nullptr, 16);
    unsigned int base_class = (class_code >> 8) & 0xFF;
    
    return bus_type == BusType::PCI ? categorize_pci(base_class) : categorize_usb(base_class);
}

}

Device::Device(DeviceInfo info, BusType type)
    : hardware_ids_{info.vendor_id, info.device_id, info.class_id}
    , description_{info.vendor_name, info.device_name, info.class_name}
    , system_info_{info.sysfs_id, info.sysfs_bus_id}
    , bus_type_(type)
{
}

DeviceCategory Device::category() const
{
    return categorize_from_class_id(hardware_ids_.class_id, bus_type_);
}

bool Device::matches(const HardwarePattern& pattern) const
{
    auto matches_any = [](const std::string& device_id, const std::vector<std::string>& patterns) {
        return std::ranges::any_of(patterns, [&device_id](const std::string& p) { 
            return matches_id(device_id, p); 
        });
    };

    return matches_any(hardware_ids_.class_id, pattern.class_ids) &&
           !matches_any(hardware_ids_.class_id, pattern.blacklisted_class_ids) &&
           matches_any(hardware_ids_.vendor, pattern.vendor_ids) &&
           !matches_any(hardware_ids_.vendor, pattern.blacklisted_vendor_ids) &&
           matches_any(hardware_ids_.device, pattern.device_ids) &&
           !matches_any(hardware_ids_.device, pattern.blacklisted_device_ids);
}

std::string_view to_string(DeviceCategory category)
{
    using namespace std::string_view_literals;
    switch (category) {
        case DeviceCategory::Graphics: return "Graphics"sv;
        case DeviceCategory::Network:  return "Network"sv;
        case DeviceCategory::Audio:    return "Audio"sv;
        case DeviceCategory::Storage:  return "Storage"sv;
        case DeviceCategory::Input:    return "Input"sv;
        case DeviceCategory::Unknown:  return "Unknown"sv;
    }
    return "Unknown"sv;
}

} // namespace mcp::mhwd
