/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Core types for driver management - value objects and enums.
 * Simple data structures without behavior.
 */

#pragma once

#include <string>
#include <vector>

namespace mcp::mhwd {

/**
 * Hardware bus type - PCI or USB.
 */
enum class BusType { PCI, USB };

/**
 * Error codes for driver operations.
 */
enum class Error {
    ParseError,
    NotFound,
    AlreadyInstalled,
    NotInstalled,
    HasConflicts,
    RequiredByOthers,
    InvalidPath,
    DatabaseError
};

/**
 * Driver configuration metadata.
 * Immutable value object describing config identity.
 */
struct ConfigMetadata {
    std::string name;
    std::string version;
    std::string info;
    int priority = 0;
    bool free_driver = true;

    auto operator<=>(const ConfigMetadata&) const = default;
};

/**
 * Hardware matching pattern with support for wildcards and blacklists.
 * Defines which devices a driver config applies to.
 */
struct HardwarePattern {
    std::vector<std::string> class_ids;
    std::vector<std::string> vendor_ids;
    std::vector<std::string> device_ids;
    std::vector<std::string> blacklisted_class_ids;
    std::vector<std::string> blacklisted_vendor_ids;
    std::vector<std::string> blacklisted_device_ids;
};

/**
 * Hardware device identity.
 * Immutable snapshot of device information from hardware detection.
 */
struct DeviceInfo {
    std::string class_id;
    std::string vendor_id;
    std::string device_id;
    std::string class_name;
    std::string vendor_name;
    std::string device_name;
    std::string sysfs_bus_id;
    std::string sysfs_id;

    auto operator<=>(const DeviceInfo&) const = default;
};

} // namespace mcp::mhwd
