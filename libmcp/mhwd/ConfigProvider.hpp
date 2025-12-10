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

#include "../Types.hpp"
#include "Config.hpp"
#include "DeviceProvider.hpp"

#include <filesystem>

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
 * Does NOT handle transactions - use mhwd::build_install/build_remove for that.
 * 
 * Usage:
 *   DeviceProvider devices;
 *   co_await devices.scan();
 *   ConfigProvider provider(devices);
 *   
 *   auto matching = co_await provider.find_matching_configs(BusType::PCI);
 *   auto config = co_await provider.find_config("video-nvidia", BusType::PCI);
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
    [[nodiscard]] Task<ConfigVectorResult>
    get_available_configs(BusType type) const;

    /**
     * Get installed configs from /var/lib/mhwd/local/{pci,usb}/
     */
    [[nodiscard]] Task<ConfigVectorResult>
    get_installed_configs(BusType type) const;

    /**
     * Find specific config by name.
     */
    [[nodiscard]] Task<ConfigResult>
    find_config(const std::string& name, BusType type) const;

    // === Device matching ===

    /**
     * Find configs that match detected hardware.
     */
    [[nodiscard]] Task<ConfigVector>
    find_matching_configs(BusType type) const;

    /**
     * Find configs matching specific device.
     */
    [[nodiscard]] Task<ConfigVector>
    find_matching_configs_for_device(const Device& device) const;

    // === Dependency analysis ===

    /**
     * Resolve all dependencies for a config (transitive).
     */
    [[nodiscard]] Task<ConfigVector>
    resolve_dependencies(const Config& config, BusType type) const;

    /**
     * Find installed configs that conflict with given config.
     */
    [[nodiscard]] Task<ConfigVector>
    find_conflicts(const Config& config, BusType type) const;

    /**
     * Find installed configs that depend on given config.
     */
    [[nodiscard]] Task<ConfigVector>
    find_required_by(const Config& config, BusType type) const;

private:
    const DeviceProvider& device_provider_;

    [[nodiscard]] ConfigVectorResult
    load_configs_from_dir(const std::filesystem::path& dir, BusType type) const;

    [[nodiscard]] std::vector<std::filesystem::path>
    find_config_files(const std::filesystem::path& dir) const;
};

} // namespace mcp::mhwd
