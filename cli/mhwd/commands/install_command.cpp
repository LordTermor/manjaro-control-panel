/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "install_command.hpp"
#include "common/output.hpp"

#include <ProgressFlattener.hpp>
#include <pamac/transaction.hpp>

#include <fmt/color.h>
#include <fmt/core.h>

#include <iostream>

namespace mcp::cli::mhwd {

using mcp::cli::out;

int InstallCommand::execute()
{
    out().set_color_enabled(color_enabled_);
    
    auto result = manager_.add_to_install(config_name_, type_);
    
    if (!result) {
        switch (result.error()) {
            case mcp::mhwd::Error::NotFound:
                out().error(fmt::format("Configuration '{}' not found", config_name_));
                break;
            case mcp::mhwd::Error::AlreadyInstalled:
                out().warning(fmt::format("Configuration '{}' is already installed", config_name_));
                break;
            case mcp::mhwd::Error::HasConflicts:
                out().error(fmt::format("Configuration '{}' has conflicts", config_name_));
                break;
            default:
                out().error(fmt::format("Failed to install '{}'", config_name_));
                break;
        }
        return 1;
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
    
    auto& txn = manager_.transaction();
    mcp::ProgressFlattener flattener;
    flattener.connect_to_transaction(txn);
    
    std::string current_phase;
    flattener.signal_progress_changed.connect([](double progress) {
        int percent = static_cast<int>(progress * 100);
        fmt::print("\r[{:3}%] ", percent);
        std::cout.flush();
    });
    
    flattener.signal_phase_changed.connect([&current_phase](const std::string& phase) {
        current_phase = phase;
        fmt::print("\n{}: ", phase);
        std::cout.flush();
    });
    
    flattener.signal_details_changed.connect([](const std::string& details) {
        if (!details.empty()) {
            fmt::print("{} ", details);
            std::cout.flush();
        }
    });
    
    txn.signal_emit_error.connect(
        [](const std::string& message, const std::vector<std::string>& details) {
            fmt::print("\n");
            out().error(message);
            for (const auto& detail : details) {
                fmt::print("  - {}\n", detail);
            }
        });
    
    // TODO: Execute transaction with co_await txn.run_async()
    fmt::print("\n");
    out().success(fmt::format("Successfully prepared installation of '{}'", config_name_));
    out().info("(Async transaction execution requires coroutine support)");
    
    return 0;
}

} // namespace mcp::cli::mhwd
