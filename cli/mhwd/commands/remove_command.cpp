/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "remove_command.hpp"
#include "common/output.hpp"

#include <coro/sync_wait.hpp>

#include <fmt/color.h>
#include <fmt/core.h>

#include <algorithm>
#include <iostream>
#include <ranges>

namespace mcp::cli::mhwd {

using mcp::cli::out;

int RemoveCommand::execute()
{
    out().set_color_enabled(color_enabled_);
    
    auto installed = coro::sync_wait(provider_.get_installed_configs(type_));
    if (!installed) {
        out().error("Failed to load installed configurations");
        return 1;
    }
    
    auto it = std::ranges::find_if(*installed, [this](const auto& cfg) {
        return cfg.name() == config_name_;
    });
    
    if (it == installed->end()) {
        out().warning(fmt::format("Configuration '{}' is not installed", config_name_));
        return 0;
    }

    if (!no_confirm_) {
        fmt::print("Remove configuration '{}'? [y/N] ", 
                   fmt::styled(config_name_, fmt::emphasis::bold));
        std::string response;
        std::getline(std::cin, response);
        
        if (response != "y" && response != "Y") {
            out().info("Removal cancelled");
            return 0;
        }
    }

    fmt::print("Removing '{}'...\n", 
               fmt::styled(config_name_, fmt::emphasis::bold));
    
    // TODO: Build and execute transaction with build_remove
    out().info("Transaction building not yet implemented");
    out().success(fmt::format("Would remove '{}'", config_name_));
    
    return 0;
}

} // namespace mcp::cli::mhwd
