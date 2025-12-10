/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Driver transaction builders - validates operations and produces agent commands.
 */

#pragma once

#include "../Types.hpp"
#include "../agent/Command.hpp"
#include "ConfigProvider.hpp"
#include "Types.hpp"

namespace mcp::mhwd {

using CommandResult = Result<agent::Command, Error>;

/**
 * Build install command for a driver config.
 * 
 * Validates:
 * - Config exists in repository
 * - Not already installed
 * - No conflicts with installed configs
 * 
 * Includes all dependencies in correct order.
 */
[[nodiscard]] Task<CommandResult>
build_install(
    const ConfigProvider& provider,
    const std::string& config_name,
    BusType type
);

/**
 * Build remove command for a driver config.
 * 
 * Validates:
 * - Config is installed
 * - Not required by other installed configs
 */
[[nodiscard]] Task<CommandResult>
build_remove(
    const ConfigProvider& provider,
    const std::string& config_name,
    BusType type
);

} // namespace mcp::mhwd
