/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Builds validated transaction agent command lines for driver operations.
 */

#include "mhwd/DriverTransactionBuilder.hpp"

#include <algorithm>
#include <ranges>

namespace rg = std::ranges;

namespace mcp::mhwd {

DriverTransactionBuilder::AgentCommandResult
DriverTransactionBuilder::install(
    const ConfigProvider& provider,
    const std::string& config_name,
    BusType type)
{
    auto config_result = provider.find_config(config_name, type);
    if (!config_result) {
        return std::unexpected(TransactionError::ConfigNotFound);
    }

    const auto& config = *config_result;

    auto installed = provider.get_installed_configs(type);
    if (installed) {
        auto is_installed = rg::any_of(*installed, [&config](const auto& inst) {
            return inst.name() == config.name();
        });
        
        if (is_installed) {
            return std::unexpected(TransactionError::AlreadyInstalled);
        }
    }

    auto conflicts = provider.find_conflicts(config, type);
    if (!conflicts.empty()) {
        return std::unexpected(TransactionError::HasConflicts);
    }

    auto dependencies = provider.resolve_dependencies(config, type);

    AgentCommand cmd;
    cmd.operation = "install";

    for (const auto& dep : dependencies) {
        cmd.packages.push_back(dep.name());
    }

    cmd.packages.push_back(config.name());

    return cmd;
}

DriverTransactionBuilder::AgentCommandResult
DriverTransactionBuilder::remove(
    const ConfigProvider& provider,
    const std::string& config_name,
    BusType type)
{
    auto installed = provider.get_installed_configs(type);
    if (!installed) {
        return std::unexpected(TransactionError::InvalidOperation);
    }

    auto it = rg::find_if(*installed, [&config_name](const auto& inst) {
        return inst.name() == config_name;
    });

    if (it == installed->end()) {
        return std::unexpected(TransactionError::ConfigNotInstalled);
    }

    const Config& config = *it;

    auto required_by = provider.find_required_by(config, type);
    if (!required_by.empty()) {
        return std::unexpected(TransactionError::RequiredByOthers);
    }

    AgentCommand cmd;
    cmd.operation = "remove";
    cmd.packages.push_back(config_name);

    return cmd;
}

} // namespace mcp::mhwd
