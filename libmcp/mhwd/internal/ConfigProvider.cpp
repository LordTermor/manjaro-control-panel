/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/ConfigProvider.hpp"

#include <coro/when_all.hpp>
#include <fmt/base.h>

#include <algorithm>
#include <filesystem>
#include <ranges>
#include <unordered_set>

/*
 * Driver configuration repository for queries and device matching.
 */

namespace rg = std::ranges;
namespace vw = std::ranges::views;

namespace mcp::mhwd {

namespace fs = std::filesystem;

namespace {

auto extract_names = [](const auto& configs) {
    return configs | vw::transform(&Config::name) | rg::to<std::unordered_set<std::string>>();
};

auto build_name_map(const ConfigVector& configs) {
    auto map = std::unordered_map<std::string, Config>{};
    for (const auto& cfg : configs) {
        map.emplace(cfg.name(), cfg);
    }
    return map;
}

auto not_in_set(const std::unordered_set<std::string>& names) {
    return [&names](const std::string& name) { return !names.contains(name); };
};

}

ConfigProvider::ConfigProvider(const DeviceProvider& device_provider)
    : device_provider_(device_provider)
{
}

Task<ConfigVectorResult>
ConfigProvider::get_available_configs(BusType type) const
{

    const auto config_dir = (type == BusType::USB) ? c_usb_config_dir : c_pci_config_dir;
    co_return load_configs_from_dir(config_dir, type);
}

Task<ConfigVectorResult>
ConfigProvider::get_installed_configs(BusType type) const
{

    const auto config_dir = (type == BusType::USB) ? c_usb_database_dir : c_pci_database_dir;
    co_return load_configs_from_dir(config_dir, type);
}

Task<ConfigResult>
ConfigProvider::find_config(const std::string& name, BusType type) const
{
    auto configs_result = co_await get_available_configs(type);
    if (!configs_result) {
        co_return std::unexpected(configs_result.error());
    }

    auto it = rg::find_if(*configs_result, [&name](const auto& config) {
        return config.name() == name;
    });

    if (it != configs_result->end()) {
        co_return *it;
    }

    co_return std::unexpected(Error::NotFound);
}

Task<ConfigVector>
ConfigProvider::find_matching_configs(BusType type) const
{
    const auto& devices = (type == BusType::USB)
        ? device_provider_.usb_devices()
        : device_provider_.pci_devices();

    auto configs_result = co_await get_available_configs(type);
    if (!configs_result) {
        co_return ConfigVector{};
    }

    auto device_vec = devices | rg::to<DeviceVector>();
    
    auto matching = *configs_result
        | vw::filter([&device_vec](const auto& config) {
            return config.matches_devices(device_vec);
        })
        | rg::to<ConfigVector>();

    rg::sort(matching, rg::greater{}, &Config::priority);

    co_return matching;
}

Task<ConfigVector>
ConfigProvider::find_matching_configs_for_device(const Device& device) const
{
    auto configs_result = co_await get_available_configs(device.bus_type());
    if (!configs_result) {
        co_return ConfigVector{};
    }

    auto matching = *configs_result
        | vw::filter([&device](const auto& config) {
            return rg::any_of(config.patterns(), [&device](const auto& pattern) {
                return device.matches(pattern);
            });
        })
        | rg::to<ConfigVector>();

    rg::sort(matching, rg::greater{}, &Config::priority);

    co_return matching;
}

Task<ConfigVector>
ConfigProvider::resolve_dependencies(const Config& config, BusType type) const
{
    auto [installed, available] = co_await coro::when_all(
        get_installed_configs(type),
        get_available_configs(type)
    );

    if (!installed.return_value() || !available.return_value()) {
        co_return ConfigVector{};
    }

    const auto installed_names = extract_names(*installed.return_value());
    const auto available_map = build_name_map(*available.return_value());

    ConfigVector result;
    auto result_names = std::unordered_set<std::string>{};

    auto process_deps = [&](const auto& self, const Config& cfg) -> Task<void> {
        auto unresolved_deps = cfg.dependencies()
            | vw::filter(not_in_set(installed_names))
            | vw::filter(not_in_set(result_names))
            | rg::to<std::vector<std::string>>();

        for (const auto& dep_name : unresolved_deps) {
            auto it = available_map.find(dep_name);
            if (it == available_map.end()) {
                continue;
            }

            co_await self(self, it->second);

            if (result_names.insert(dep_name).second) {
                result.push_back(it->second);
            }
        }
    };

    co_await process_deps(process_deps, config);
    co_return result;
}

Task<ConfigVector>
ConfigProvider::find_conflicts(const Config& config, BusType type) const
{
    auto installed = co_await get_installed_configs(type);
    if (!installed) {
        co_return ConfigVector{};
    }

    auto dependencies = co_await resolve_dependencies(config, type);
    auto all_to_install = dependencies;
    all_to_install.push_back(config);

    const auto installed_names = extract_names(*installed);

    auto conflict_names = all_to_install
        | vw::transform(&Config::conflicts)
        | vw::join
        | vw::filter([&installed_names](const auto& name) { return installed_names.contains(name); })
        | rg::to<std::unordered_set<std::string>>();

    auto conflicts = *installed
        | vw::filter([&conflict_names](const Config& cfg) { return conflict_names.contains(cfg.name()); })
        | rg::to<ConfigVector>();

    co_return conflicts;
}

Task<ConfigVector>
ConfigProvider::find_required_by(const Config& config, BusType type) const
{
    auto installed = co_await get_installed_configs(type);
    if (!installed) {
        co_return ConfigVector{};
    }

    auto required_by = *installed
        | vw::filter([&config](const Config& cfg) { return cfg.depends_on(config.name()); })
        | rg::to<ConfigVector>();

    co_return required_by;
}

ConfigVectorResult
ConfigProvider::load_configs_from_dir(const fs::path& dir, BusType type) const
{
    if (!fs::exists(dir)) {
        return std::unexpected(Error::InvalidPath);
    }

    auto config_files = find_config_files(dir);
    ConfigVector configs;

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
