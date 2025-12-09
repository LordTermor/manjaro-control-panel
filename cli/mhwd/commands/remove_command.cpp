/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "remove_command.hpp"
#include "common/output.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include <iostream>

namespace mcp::cli::mhwd {

using mcp::cli::out;

int RemoveCommand::execute()
{
    out().set_color_enabled(color_enabled_);
    
    auto result = manager_.add_to_remove(config_name_, type_);
    
    if (!result) {
        switch (result.error()) {
            case mcp::mhwd::Error::NotFound:
                out().error(fmt::format("Configuration '{}' not found", config_name_));
                break;
            case mcp::mhwd::Error::NotInstalled:
                out().warning(fmt::format("Configuration '{}' is not installed", config_name_));
                break;
            case mcp::mhwd::Error::RequiredByOthers:
                out().error(fmt::format("Configuration '{}' is required by other configurations", config_name_));
                break;
            default:
                out().error(fmt::format("Failed to remove '{}'", config_name_));
                break;
        }
        return 1;
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
    
    out().success(fmt::format("Successfully prepared removal of '{}'", config_name_));
    out().info("(Transaction execution not yet implemented)");
    
    return 0;
}

} // namespace mcp::cli::mhwd
