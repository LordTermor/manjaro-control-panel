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

enum class BusType { 
    PCI, 
    USB 
};

enum class DeviceCategory {
    Graphics,
    Network,
    Audio,
    Storage,
    Input,
    Unknown
};

enum class Error {
    // Parsing and lookup
    ParseError,
    NotFound,
    InvalidPath,
    
    // Transaction validation
    AlreadyInstalled,
    NotInstalled,
    HasConflicts,
    RequiredByOthers,
    InvalidOperation,
    
    // Backend errors
    DatabaseError
};

struct DeviceInfo {
    std::string class_id;
    std::string vendor_id;
    std::string device_id;
    std::string class_name;
    std::string vendor_name;
    std::string device_name;
    std::string sysfs_bus_id;
    std::string sysfs_id;
    std::string driver;
};

struct HardwarePattern {
    std::vector<std::string> class_ids;
    std::vector<std::string> vendor_ids;
    std::vector<std::string> device_ids;
    std::vector<std::string> blacklisted_class_ids;
    std::vector<std::string> blacklisted_vendor_ids;
    std::vector<std::string> blacklisted_device_ids;
};

} // namespace mcp::mhwd
