/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/DriverManager.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>

/*
 * Driver configuration repository and transaction builder.
 * Manages config loading, device-to-driver matching, dependency resolution,
 * conflict detection, and libpamac transaction construction.
 */

namespace rg = std::ranges;
namespace vw = std::ranges::views;

namespace mcp::mhwd {

namespace fs = std::filesystem;

DriverManager::DriverManager(const DeviceProvider& device_provider, pamac::Database& database)
    : device_provider_(device_provider)
    , transaction_(database)
{
}

std::expected<std::vector<Config>, Error>
DriverManager::get_available_configs(BusType type) const
{
    const auto config_dir = (type == BusType::USB) ? c_usb_config_dir : c_pci_config_dir;
    return load_configs_from_dir(config_dir, type);
}

std::expected<std::vector<Config>, Error>
DriverManager::get_installed_configs(BusType type) const
{
    const auto config_dir = (type == BusType::USB) ? c_usb_database_dir : c_pci_database_dir;
    return load_configs_from_dir(config_dir, type);
}

std::expected<Config, Error>
DriverManager::find_config(const std::string& name, BusType type) const
{
    auto configs_result = get_available_configs(type);
    if (!configs_result) {
        return std::unexpected(configs_result.error());
    }

    auto it = rg::find_if(*configs_result, [&name](const auto& config) {
        return config.name() == name;
    });

    if (it != configs_result->end()) {
        return *it;
    }

    return std::unexpected(Error::NotFound);
}

std::vector<Config>
DriverManager::find_matching_configs(BusType type) const
{
    const auto& devices = (type == BusType::USB)
        ? device_provider_.get_usb_devices()
        : device_provider_.get_pci_devices();

    auto configs_result = get_available_configs(type);
    if (!configs_result) {
        return {};
    }

    auto device_vec = devices | rg::to<std::vector>();
    
    auto matching = *configs_result
        | vw::filter([&device_vec](const auto& config) {
            return config.matches_devices(device_vec);
        })
        | rg::to<std::vector>();

    // Sort by priority (highest first)
    rg::sort(matching, rg::greater{}, &Config::priority);

    return matching;
}

std::vector<Config>
DriverManager::find_matching_configs_for_device(const Device& device) const
{
    auto configs_result = get_available_configs(device.bus_type());
    if (!configs_result) {
        return {};
    }

    auto matching = *configs_result
        | vw::filter([&device](const auto& config) {
            return rg::any_of(config.patterns(), [&device](const auto& pattern) {
                return device.matches(pattern);
            });
        })
        | rg::to<std::vector>();

    // Sort by priority (highest first)
    rg::sort(matching, rg::greater{}, &Config::priority);

    return matching;
}

std::expected<void, Error>
DriverManager::add_to_install(const std::string& config_name, BusType type)
{
    auto config_result = find_config(config_name, type);
    if (!config_result) {
        return std::unexpected(config_result.error());
    }

    const auto& config = *config_result;

    auto installed = get_installed_configs(type);
    if (installed) {
        auto is_installed = rg::any_of(*installed, [&config](const auto& inst) {
            return inst.name() == config.name();
        });
        
        if (is_installed) {
            return std::unexpected(Error::AlreadyInstalled);
        }
    }

    auto dependencies = resolve_dependencies(config, type);

    auto conflicts = find_conflicts(config, type);
    if (!conflicts.empty()) {
        return std::unexpected(Error::HasConflicts);
    }

    for (auto it = dependencies.rbegin(); it != dependencies.rend(); ++it) {
        transaction_.add_pkg_to_install(it->name());
    }

    transaction_.add_pkg_to_install(config.name());

    return {};
}

std::expected<void, Error>
DriverManager::add_to_remove(const std::string& config_name, BusType type)
{
    auto installed = get_installed_configs(type);
    if (!installed) {
        return std::unexpected(installed.error());
    }

    auto it = rg::find_if(*installed, [&config_name](const auto& inst) {
        return inst.name() == config_name;
    });

    if (it == installed->end()) {
        return std::unexpected(Error::NotInstalled);
    }

    const Config& config = *it;

    auto required_by = find_required_by(config, type);
    if (!required_by.empty()) {
        return std::unexpected(Error::RequiredByOthers);
    }

    transaction_.add_pkg_to_remove(config_name);

    return {};
}

void DriverManager::reset_transaction()
{
}

std::vector<Config>
DriverManager::resolve_dependencies(const Config& config, BusType type) const
{
    std::vector<Config> result;
    auto installed = get_installed_configs(type);
    if (!installed) {
        return result;
    }

    for (const auto& dep_name : config.dependencies()) {
        bool already_installed = rg::any_of(*installed, [&dep_name](const Config& c) {
            return c.name() == dep_name;
        });

        if (already_installed) {
            continue;
        }

        auto names_view = result | vw::transform(&Config::name);
        if (rg::contains(names_view, dep_name)) {
            continue;
        }

        auto dep_config = find_config(dep_name, type);
        if (dep_config) {
            result.push_back(*dep_config);

            auto sub_deps = resolve_dependencies(*dep_config, type);
            for (auto& sub : sub_deps) {
                auto names_view = result | vw::transform(&Config::name);
                if (!rg::contains(names_view, sub.name())) {
                    result.push_back(std::move(sub));
                }
            }
        }
    }

    return result;
}

std::vector<Config>
DriverManager::find_conflicts(const Config& config, BusType type) const
{
    std::vector<Config> conflicts;

    auto installed = get_installed_configs(type);
    if (!installed) {
        return conflicts;
    }

    auto dependencies = resolve_dependencies(config, type);
    auto all_to_install = dependencies;
    all_to_install.push_back(config);

    for (const auto& to_install : all_to_install) {
        for (const auto& conflict_name : to_install.conflicts()) {
            for (const auto& inst : *installed) {
                if (inst.name() == conflict_name) {
                    auto names_view = conflicts | vw::transform(&Config::name);
                    if (!rg::contains(names_view, conflict_name)) {
                        conflicts.push_back(inst);
                    }
                    break;
                }
            }
        }
    }

    return conflicts;
}

std::vector<Config>
DriverManager::find_required_by(const Config& config, BusType type) const
{
    std::vector<Config> required_by;

    auto installed = get_installed_configs(type);
    if (!installed) {
        return required_by;
    }

    for (const auto& inst : *installed) {
        if (inst.depends_on(config.name())) {
            auto names_view = required_by | vw::transform(&Config::name);
            if (!rg::contains(names_view, inst.name())) {
                required_by.push_back(inst);
            }
        }
    }

    return required_by;
}

std::expected<std::vector<Config>, Error>
DriverManager::load_configs_from_dir(const fs::path& dir, BusType type) const
{
    if (!fs::exists(dir)) {
        return std::unexpected(Error::InvalidPath);
    }

    auto config_files = find_config_files(dir);
    std::vector<Config> configs;

    for (const auto& file : config_files) {
        auto config = Config::from_file(file, type);
        if (config) {
            configs.push_back(std::move(*config));
        }
    }

    return configs;
}

std::vector<fs::path>
DriverManager::find_config_files(const fs::path& dir) const
{
    std::vector<fs::path> files;

    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return files;
    }

    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().filename() == c_config_filename) {
            files.push_back(entry.path());
        }
    }

    return files;
}

} // namespace mcp::mhwd
