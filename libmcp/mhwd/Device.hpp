/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Device - hardware device with structured information access.
 * Provides hardware IDs, human-readable descriptions, and system paths.
 */

#pragma once

#include "Types.hpp"

#include <string>

namespace mcp::mhwd {

struct DeviceInfo;  // Forward declare internal type
struct HardwarePattern;  // Forward declare internal type

/**
 * Hardware device detected on the system.
 * 
 * Provides structured access to device information through nested types.
 * 
 * Usage:
 *   const auto& ids = device.hardware_ids();
 *   const auto& desc = device.description();
 *   auto category = device.category();
 */
class Device {
public:
    /**
     * Hardware identification codes used for driver matching.
     */
    struct HardwareIds {
        std::string vendor;    // "10de"
        std::string device;    // "1c03"
        std::string class_id;  // "0300"
    };
    
    /**
     * Human-readable device information.
     */
    struct Description {
        std::string vendor;        // "NVIDIA Corporation"
        std::string model;         // "GeForce GTX 1050"
        std::string device_class;  // "VGA compatible controller"
    };
    
    /**
     * System paths and identifiers.
     */
    struct SystemInfo {
        std::string sysfs_path;  // "/sys/devices/pci0000:00/..."
        std::string bus_id;      // "0000:01:00.0"
    };

    Device(DeviceInfo info, BusType type);

    // === Hardware identification ===
    
    [[nodiscard]] const HardwareIds& hardware_ids() const { return hardware_ids_; }
    
    // === Human-readable information ===
    
    [[nodiscard]] const Description& description() const { return description_; }
    
    // === System information ===
    
    [[nodiscard]] const SystemInfo& system_info() const { return system_info_; }
    [[nodiscard]] BusType bus_type() const { return bus_type_; }
    
    // === Device categorization ===
    
    [[nodiscard]] DeviceCategory category() const;

private:
    friend class Config;
    friend class ConfigProvider;
    [[nodiscard]] bool matches(const HardwarePattern& pattern) const;

    HardwareIds hardware_ids_;
    Description description_;
    SystemInfo system_info_;
    BusType bus_type_;
};

std::string_view to_string(DeviceCategory category);

} // namespace mcp::mhwd
