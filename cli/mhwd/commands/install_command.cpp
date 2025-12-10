/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "install_command.hpp"
#include "common/output.hpp"

#include <coro/sync_wait.hpp>

#include <fmt/color.h>
#include <fmt/core.h>

#include <algorithm>
#include <iostream>
#include <ranges>

namespace mcp::cli::mhwd {

using mcp::cli::out;

int InstallCommand::execute()
{
    out().set_color_enabled(color_enabled_);
    
    auto configs = coro::sync_wait(provider_.get_available_configs(type_));
    if (!configs) {
        out().error("Failed to load configurations");
        return 1;
    }
    
    auto it = std::ranges::find_if(*configs, [this](const auto& cfg) {
        return cfg.name() == config_name_;
    });
    
    if (it == configs->end()) {
        out().error(fmt::format("Configuration '{}' not found", config_name_));
        return 1;
    }
    
    auto installed = coro::sync_wait(provider_.get_installed_configs(type_));
    if (installed) {
        bool already = std::ranges::any_of(*installed, [this](const auto& cfg) {
            return cfg.name() == config_name_;
        });
        if (already) {
            out().warning(fmt::format("Configuration '{}' is already installed", config_name_));
            return 0;
        }
    }

    if (!no_confirm_) {
        fmt::print("Install configuration '{}'? [y/N] ", 
                   fmt::styled(config_name_, fmt::emphasis::bold));
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y") {
            out().info("Installation cancelled");
            return 0;
        }
    }

    fmt::print("Installing '{}'...\n", 
               fmt::styled(config_name_, fmt::emphasis::bold));
    
    // TODO: Build and execute transaction with build_install
    out().info("Transaction building not yet implemented");
    out().success(fmt::format("Would install '{}'", config_name_));
    
    return 0;
}

} // namespace mcp::cli::mhwd
