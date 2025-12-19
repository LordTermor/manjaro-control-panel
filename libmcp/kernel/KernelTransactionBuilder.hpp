/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Builds validated transaction agent command lines for kernel operations.
 * Validates operations and produces ready-to-execute command arguments.
 */

#pragma once

#include "KernelProvider.hpp"

#include <coro/task.hpp>

#include <expected>
#include <string>
#include <vector>

namespace mcp::kernel {

enum class TransactionError {
    KernelNotFound,
    KernelInUse,
    UpdatesPending,
    NoPackagesSpecified,
    InvalidOperation
};

struct AgentCommand {
    std::string operation;  // "install", "remove", "upgrade"
    std::vector<std::string> packages;
    bool force = false;
    bool refresh = false;
};

/**
 * Builds transaction agent commands for kernel operations with validation.
 * 
 * Usage:
 *   auto install_cmd = co_await KernelTransactionBuilder::install(
 *       "linux66",
 *       true  // with_headers
 *   );
 *   
 *   if (install_cmd) {
 *       // spawn agent with: install_cmd->operation, install_cmd->packages
 *   }
 * 
 * Builder validates:
 * - Kernel exists in repositories
 * - No pending system updates (for install)
 * - Not removing running kernel (unless forced)
 * - Includes headers and extra modules as needed
 */
class KernelTransactionBuilder {
public:
    KernelTransactionBuilder() = default;

    using AgentCommandResult = std::expected<AgentCommand, TransactionError>;

    [[nodiscard]] static coro::task<AgentCommandResult>
    install(
        const std::string& package_name,
        bool with_headers = true,
        bool with_extra_modules = true
    );

    [[nodiscard]] static coro::task<AgentCommandResult>
    remove(
        const std::string& package_name,
        bool with_headers = true,
        bool with_extra_modules = true,
        bool force = false
    );

    [[nodiscard]] static coro::task<AgentCommandResult>
    upgrade(bool force_refresh = false);

private:
    [[nodiscard]] static coro::task<bool> has_pending_updates_async();
    
    [[nodiscard]] static coro::task<bool> is_safe_to_remove_async(const std::string& package_name);
    
    [[nodiscard]] static std::string get_headers_package(const std::string& package_name);
};

} // namespace mcp::kernel
