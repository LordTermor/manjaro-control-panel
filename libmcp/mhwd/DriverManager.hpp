/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DriverManager - driver configuration management with libpamac integration.
 * Provides config querying, matching, and transaction building.
 */

#pragma once

#include "Config.hpp"
#include "DeviceProvider.hpp"
#include "internal/Types.hpp"

#include <pamac/database.hpp>
#include <pamac/transaction.hpp>

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
 * Driver configuration manager.
 * 
 * Manages driver configurations, device matching, and package operations.
 * Builds libpamac transactions for installation/removal.
 * 
 * Usage:
 *   auto db = pamac::Database::instance();
 *   DeviceProvider devices;
 *   DriverManager manager(devices, *db);
 *   
 *   // Find configs for current hardware
 *   auto matching = manager.find_matching_configs(BusType::PCI);
 *   
 *   // Build transaction
 *   if (auto result = manager.add_to_install("video-nvidia", BusType::PCI); result) {
 *       auto& txn = manager.transaction();
 *       txn.signal_emit_action.connect([](auto msg) { ... });
 *       co_await txn.run_async();
 *   }
 */
class DriverManager {
public:
    /**
     * Construct manager with device provider and package database.
     */
    explicit DriverManager(const DeviceProvider& device_provider, pamac::Database& database);

    // === Config queries ===

    /**
     * Get all available configs from /var/lib/mhwd/db/{pci,usb}/
     * Parses on-demand.
     */
    [[nodiscard]] std::expected<std::vector<Config>, Error>
    get_available_configs(BusType type) const;

    /**
     * Get installed configs from /var/lib/mhwd/local/{pci,usb}/
     * Parses on-demand.
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

    // === Transaction building ===

    /**
     * Add config to installation transaction.
     * Resolves dependencies and checks for conflicts.
     * Returns error if conflicts detected.
     */
    [[nodiscard]] std::expected<void, Error>
    add_to_install(const std::string& config_name, BusType type);

    /**
     * Add config to removal transaction.
     * Checks if anything requires this config.
     * Returns error if required by others.
     */
    [[nodiscard]] std::expected<void, Error>
    add_to_remove(const std::string& config_name, BusType type);

    /**
     * Get configured transaction ready to run.
     * User should connect to signals and then co_await run_async().
     */
    [[nodiscard]] pamac::Transaction& transaction() { return transaction_; }

    /**
     * Reset transaction to start fresh.
     */
    void reset_transaction();

private:
    const DeviceProvider& device_provider_;
    pamac::Transaction transaction_;

    // Dependency resolution
    [[nodiscard]] std::vector<Config>
    resolve_dependencies(const Config& config, BusType type) const;

    [[nodiscard]] std::vector<Config>
    find_conflicts(const Config& config, BusType type) const;

    [[nodiscard]] std::vector<Config>
    find_required_by(const Config& config, BusType type) const;

    // Config loading helpers
    [[nodiscard]] std::expected<std::vector<Config>, Error>
    load_configs_from_dir(const std::filesystem::path& dir, BusType type) const;

    [[nodiscard]] std::vector<std::filesystem::path>
    find_config_files(const std::filesystem::path& dir) const;
};

} // namespace mcp::mhwd
