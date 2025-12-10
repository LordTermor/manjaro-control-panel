/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Driver configuration with parsing and device matching logic.
 * Loads from MHWDCONFIG files and determines device applicability.
 */

#pragma once

#include "../Types.hpp"
#include "Device.hpp"
#include "Types.hpp"

#include <filesystem>
#include <vector>

namespace mcp::mhwd {

using mcp::Result;

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
 * Contains metadata, hardware compatibility info, and dependency relationships.
 * Provides matching logic to determine if config applies to devices.
 * 
 * Example usage:
 *   auto config = Config::from_file(path, BusType::PCI);
 *   if (config && config->matches_devices(my_devices)) {
 *       install(config->name());
 *   }
 */
class Config {
public:
    /**
     * Parse config from MHWDCONFIG file.
     * Returns error if file doesn't exist or has invalid format.
     */
    [[nodiscard]] static Result<Config, ParseError>
    from_file(const std::filesystem::path& path, BusType type);

    // === Metadata ===
    
    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] const std::string& version() const { return version_; }
    [[nodiscard]] const std::string& description() const { return description_; }
    [[nodiscard]] int priority() const { return priority_; }
    [[nodiscard]] bool is_free_driver() const { return is_free_driver_; }
    
    // === Hardware compatibility ===
    
    [[nodiscard]] const std::vector<HardwarePattern>& patterns() const { return patterns_; }
    
    /**
     * Check if config matches ALL patterns (each pattern must match at least one device).
     * This is AND logic - all pattern groups must be satisfied.
     */
    [[nodiscard]] bool matches_devices(const DeviceVector& devices) const;
    
    // === Dependencies ===
    
    [[nodiscard]] const std::vector<std::string>& dependencies() const { return dependencies_; }
    [[nodiscard]] const std::vector<std::string>& conflicts() const { return conflicts_; }
    [[nodiscard]] bool depends_on(const std::string& config_name) const;
    [[nodiscard]] bool conflicts_with(const std::string& config_name) const;
    
    // === System paths ===
    
    [[nodiscard]] BusType bus_type() const { return bus_type_; }
    [[nodiscard]] const std::filesystem::path& base_path() const { return base_path_; }
    [[nodiscard]] const std::filesystem::path& config_file() const { return config_file_; }

private:
    std::string name_;
    std::string version_;
    std::string description_;
    int priority_ = 0;
    bool is_free_driver_ = true;
    
    BusType bus_type_;
    std::filesystem::path base_path_;
    std::filesystem::path config_file_;

    std::vector<HardwarePattern> patterns_;
    std::vector<std::string> dependencies_;
    std::vector<std::string> conflicts_;
};

using ConfigVector = std::vector<Config>;
using ConfigResult = Result<Config, Error>;
using ConfigVectorResult = Result<ConfigVector, Error>;

} // namespace mcp::mhwd
