/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Config - driver configuration with parsing and matching logic.
 * Loads from MHWDCONFIG files and determines device applicability.
 */

#pragma once

#include "Device.hpp"
#include "internal/Types.hpp"

#include <expected>
#include <filesystem>
#include <vector>

namespace mcp::mhwd {

/**
 * Parse error details.
 */
struct ParseError {
    std::string message;
    std::filesystem::path file_path;
};

/**
 * Driver configuration loaded from MHWDCONFIG file.
 * 
 * Contains metadata, hardware patterns, and dependency information.
 * Provides matching logic to determine if config applies to devices.
 * 
 * Example config file format:
 *   Name=video-nvidia
 *   Version=1.0
 *   ClassIDs=0300
 *   VendorIDs=10de
 *   MHWD_Depends=video-modesetting
 *   MHWD_Conflicts=video-nouveau
 */
class Config {
    ConfigMetadata metadata_;
    BusType bus_type_;
    std::filesystem::path base_path_;
    std::filesystem::path config_path_;

    std::vector<HardwarePattern> patterns_;  // ALL must match
    std::vector<std::string> dependencies_;
    std::vector<std::string> conflicts_;

public:
    /**
     * Parse config from MHWDCONFIG file.
     * Returns error if file doesn't exist or has invalid format.
     */
    [[nodiscard]] static std::expected<Config, ParseError>
    from_file(const std::filesystem::path& path, BusType type);

    /**
     * Check if config matches ALL patterns (each pattern must match at least one device).
     * This is AND logic - all pattern groups must be satisfied.
     */
    [[nodiscard]] bool matches_devices(const std::vector<Device>& devices) const;

    /**
     * Check dependency/conflict relationships.
     */
    [[nodiscard]] bool depends_on(const std::string& config_name) const;
    [[nodiscard]] bool conflicts_with(const std::string& config_name) const;

    [[nodiscard]] const std::string& name() const { return metadata_.name; }
    [[nodiscard]] const std::string& version() const { return metadata_.version; }
    [[nodiscard]] const std::string& info() const { return metadata_.info; }
    [[nodiscard]] int priority() const { return metadata_.priority; }
    [[nodiscard]] bool free_driver() const { return metadata_.free_driver; }
    [[nodiscard]] BusType bus_type() const { return bus_type_; }
    [[nodiscard]] const std::filesystem::path& base_path() const { return base_path_; }
    [[nodiscard]] const std::filesystem::path& config_path() const { return config_path_; }
    [[nodiscard]] const std::vector<HardwarePattern>& patterns() const { return patterns_; }
    [[nodiscard]] const std::vector<std::string>& dependencies() const { return dependencies_; }
    [[nodiscard]] const std::vector<std::string>& conflicts() const { return conflicts_; }
};

} // namespace mcp::mhwd
