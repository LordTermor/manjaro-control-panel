/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Core public types for MHWD library.
 */

#pragma once

#include <string>
#include <vector>

namespace mcp::mhwd {

/**
 * Hardware bus type.
 */
enum class BusType { 
    PCI, 
    USB 
};

/**
 * Device category based on hardware class.
 */
enum class DeviceCategory {
    Graphics,
    Network,
    Audio,
    Storage,
    Input,
    Unknown
};

/**
 * Error codes for driver configuration operations.
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
 * Raw hardware device information from detection layer.
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
};

/**
 * Hardware matching pattern with wildcards (*) and blacklists.
 */
struct HardwarePattern {
    std::vector<std::string> class_ids;
    std::vector<std::string> vendor_ids;
    std::vector<std::string> device_ids;
    std::vector<std::string> blacklisted_class_ids;
    std::vector<std::string> blacklisted_vendor_ids;
    std::vector<std::string> blacklisted_device_ids;
};

} // namespace mcp::mhwd
