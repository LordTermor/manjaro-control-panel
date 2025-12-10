/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Kernel transaction builders - validates operations and produces agent commands.
 */

#pragma once

#include "../Types.hpp"
#include "../agent/Command.hpp"

namespace mcp::kernel {

enum class TransactionError {
    KernelNotFound,
    KernelInUse,
    UpdatesPending,
    NoPackagesSpecified,
    InvalidOperation
};

using CommandResult = Result<agent::Command, TransactionError>;

/**
 * Build install command for a kernel package.
 * 
 * Validates:
 * - Kernel exists in repositories
 * - No pending system updates
 * 
 * Includes headers and extra modules if requested.
 */
[[nodiscard]] Task<CommandResult>
build_install(
    const std::string& package_name,
    bool with_headers = true,
    bool with_extra_modules = true
);

/**
 * Build remove command for a kernel package.
 * 
 * Validates:
 * - Kernel is installed
 * - Not removing running kernel (unless force=true)
 * 
 * Includes headers and extra modules if requested.
 */
[[nodiscard]] Task<CommandResult>
build_remove(
    const std::string& package_name,
    bool with_headers = true,
    bool with_extra_modules = true,
    bool force = false
);

/**
 * Build system upgrade command.
 */
[[nodiscard]] Task<CommandResult>
build_upgrade(bool force_refresh = false);

} // namespace mcp::kernel
