/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/ConfigProvider.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>

/*
 * Driver configuration repository for queries and device matching.
 */

namespace rg = std::ranges;
namespace vw = std::ranges::views;

namespace mcp::mhwd {

namespace fs = std::filesystem;

ConfigProvider::ConfigProvider(const DeviceProvider& device_provider)
    : device_provider_(device_provider)
{
}

std::expected<std::vector<Config>, Error>
ConfigProvider::get_available_configs(BusType type) const
{
    const auto config_dir = (type == BusType::USB) ? c_usb_config_dir : c_pci_config_dir;
    return load_configs_from_dir(config_dir, type);
}

std::expected<std::vector<Config>, Error>
ConfigProvider::get_installed_configs(BusType type) const
{
    const auto config_dir = (type == BusType::USB) ? c_usb_database_dir : c_pci_database_dir;
    return load_configs_from_dir(config_dir, type);
}

std::expected<Config, Error>
ConfigProvider::find_config(const std::string& name, BusType type) const
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
ConfigProvider::find_matching_configs(BusType type) const
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

    rg::sort(matching, rg::greater{}, &Config::priority);

    return matching;
}

std::vector<Config>
ConfigProvider::find_matching_configs_for_device(const Device& device) const
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

    rg::sort(matching, rg::greater{}, &Config::priority);

    return matching;
}

std::vector<Config>
ConfigProvider::resolve_dependencies(const Config& config, BusType type) const
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
ConfigProvider::find_conflicts(const Config& config, BusType type) const
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
ConfigProvider::find_required_by(const Config& config, BusType type) const
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
ConfigProvider::load_configs_from_dir(const fs::path& dir, BusType type) const
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
ConfigProvider::find_config_files(const fs::path& dir) const
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
