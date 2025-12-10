/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Device - hardware device with direct member access.
 */

#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

namespace mcp::mhwd {

/**
 * Hardware device detected on the system.
 */
class Device {
public:
    Device(DeviceInfo info, BusType type);

    // === Hardware identification (for driver matching) ===
    
    [[nodiscard]] const std::string& vendor_id() const { return vendor_id_; }
    [[nodiscard]] const std::string& device_id() const { return device_id_; }
    [[nodiscard]] const std::string& class_id() const { return class_id_; }
    
    // === Human-readable information ===
    
    [[nodiscard]] const std::string& vendor_name() const { return vendor_name_; }
    [[nodiscard]] const std::string& device_name() const { return device_name_; }
    [[nodiscard]] const std::string& class_name() const { return class_name_; }
    
    // === System information ===
    
    [[nodiscard]] const std::string& sysfs_path() const { return sysfs_path_; }
    [[nodiscard]] const std::string& bus_id() const { return bus_id_; }
    [[nodiscard]] BusType bus_type() const { return bus_type_; }
    [[nodiscard]] const std::string& driver() const { return driver_; }
    
    // === Device categorization ===
    
    [[nodiscard]] DeviceCategory category() const;

private:
    friend class Config;
    friend class ConfigProvider;
    [[nodiscard]] bool matches(const HardwarePattern& pattern) const;

    // Hardware IDs
    std::string vendor_id_;
    std::string device_id_;
    std::string class_id_;
    
    // Human-readable names
    std::string vendor_name_;
    std::string device_name_;
    std::string class_name_;
    
    // System paths
    std::string sysfs_path_;
    std::string bus_id_;
    BusType bus_type_;
    std::string driver_;
};

using DeviceVector = std::vector<Device>;

std::string_view to_string(DeviceCategory category);

} // namespace mcp::mhwd
