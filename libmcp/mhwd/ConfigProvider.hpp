/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * ConfigProvider - driver configuration discovery and device matching.
 * Queries available/installed configs and matches them to hardware.
 */

#pragma once

#include "Config.hpp"
#include "DeviceProvider.hpp"

#include <expected>
#include <filesystem>
#include <vector>

namespace mcp::mhwd {

// Default paths for driver configs
constexpr std::string_view c_pci_config_dir = "/var/lib/mhwd/db/pci";
constexpr std::string_view c_usb_config_dir = "/var/lib/mhwd/db/usb";
constexpr std::string_view c_pci_database_dir = "/var/lib/mhwd/local/pci";
constexpr std::string_view c_usb_database_dir = "/var/lib/mhwd/local/usb";
constexpr std::string_view c_config_filename = "MHWDCONFIG";

/**
 * Driver configuration provider.
 * 
 * Manages driver configuration queries and device-to-driver matching.
 * Does NOT handle transactions - use DriverTransactionBuilder for that.
 * 
 * Usage:
 *   DeviceProvider devices;
 *   ConfigProvider provider(devices);
 *   
 *   // Find configs for current hardware
 *   auto matching = provider.find_matching_configs(BusType::PCI);
 *   
 *   // Check specific config
 *   auto config = provider.find_config("video-nvidia", BusType::PCI);
 */
class ConfigProvider {
public:
    /**
     * Construct provider with device provider.
     */
    explicit ConfigProvider(const DeviceProvider& device_provider);

    // === Config queries ===

    /**
     * Get all available configs from /var/lib/mhwd/db/{pci,usb}/
     */
    [[nodiscard]] std::expected<std::vector<Config>, Error>
    get_available_configs(BusType type) const;

    /**
     * Get installed configs from /var/lib/mhwd/local/{pci,usb}/
     */
    [[nodiscard]] std::expected<std::vector<Config>, Error>
    get_installed_configs(BusType type) const;

    /**
     * Find specific config by name.
     */
    [[nodiscard]] std::expected<Config, Error>
    find_config(const std::string& name, BusType type) const;

    // === Device matching ===

    /**
     * Find configs that match detected hardware.
     */
    [[nodiscard]] std::vector<Config>
    find_matching_configs(BusType type) const;

    /**
     * Find configs matching specific device.
     */
    [[nodiscard]] std::vector<Config>
    find_matching_configs_for_device(const Device& device) const;

    // === Dependency analysis ===

    /**
     * Resolve all dependencies for a config (transitive).
     */
    [[nodiscard]] std::vector<Config>
    resolve_dependencies(const Config& config, BusType type) const;

    /**
     * Find installed configs that conflict with given config.
     */
    [[nodiscard]] std::vector<Config>
    find_conflicts(const Config& config, BusType type) const;

    /**
     * Find installed configs that depend on given config.
     */
    [[nodiscard]] std::vector<Config>
    find_required_by(const Config& config, BusType type) const;

private:
    const DeviceProvider& device_provider_;

    [[nodiscard]] std::expected<std::vector<Config>, Error>
    load_configs_from_dir(const std::filesystem::path& dir, BusType type) const;

    [[nodiscard]] std::vector<std::filesystem::path>
    find_config_files(const std::filesystem::path& dir) const;
};

} // namespace mcp::mhwd
