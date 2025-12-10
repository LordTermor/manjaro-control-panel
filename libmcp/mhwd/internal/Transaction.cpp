/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Driver transaction builders - validates operations and produces agent commands.
 */

#include "mhwd/Transaction.hpp"

#include <algorithm>

namespace rg = std::ranges;

namespace mcp::mhwd {

Task<CommandResult>
build_install(
    const ConfigProvider& provider,
    const std::string& config_name,
    BusType type)
{
    auto config_result = co_await provider.find_config(config_name, type);
    if (!config_result) {
        co_return std::unexpected(Error::NotFound);
    }

    const auto& config = *config_result;

    auto installed = co_await provider.get_installed_configs(type);
    if (installed) {
        auto is_installed = rg::any_of(*installed, [&config](const auto& inst) {
            return inst.name() == config.name();
        });
        
        if (is_installed) {
            co_return std::unexpected(Error::AlreadyInstalled);
        }
    }

    auto conflicts = co_await provider.find_conflicts(config, type);
    if (!conflicts.empty()) {
        co_return std::unexpected(Error::HasConflicts);
    }

    auto dependencies = co_await provider.resolve_dependencies(config, type);

    std::vector<std::string> packages;
    for (const auto& dep : dependencies) {
        packages.push_back(dep.name());
    }
    packages.push_back(config.name());

    co_return agent::make_install(std::move(packages));
}

Task<CommandResult>
build_remove(
    const ConfigProvider& provider,
    const std::string& config_name,
    BusType type)
{
    auto installed = co_await provider.get_installed_configs(type);
    if (!installed) {
        co_return std::unexpected(Error::InvalidOperation);
    }

    auto it = rg::find_if(*installed, [&config_name](const auto& inst) {
        return inst.name() == config_name;
    });

    if (it == installed->end()) {
        co_return std::unexpected(Error::NotInstalled);
    }

    const Config& config = *it;

    auto required_by = co_await provider.find_required_by(config, type);
    if (!required_by.empty()) {
        co_return std::unexpected(Error::RequiredByOthers);
    }

    co_return agent::make_remove({config_name});
}

} // namespace mcp::mhwd
