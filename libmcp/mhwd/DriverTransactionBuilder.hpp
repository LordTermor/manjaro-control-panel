/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Builds validated transaction agent command lines for driver operations.
 */

#pragma once


#include "ConfigProvider.hpp"

#include <expected>
#include <string>
#include <vector>

namespace mcp::mhwd {

enum class TransactionError {
    ConfigNotFound,
    ConfigNotInstalled,
    AlreadyInstalled,
    HasConflicts,
    RequiredByOthers,
    InvalidOperation
};

struct AgentCommand {
    std::string operation;  // "install", "remove"
    std::vector<std::string> packages;
};

/**
 * Builds transaction agent commands for driver config operations with validation.
 * 
 * Usage:
 *   DeviceProvider devices;
 *   ConfigProvider provider(devices);
 *   
 *   auto install_cmd = DriverTransactionBuilder::install(
 *       provider,
 *       "video-nvidia",
 *       BusType::PCI
 *   );
 *   
 *   if (install_cmd) {
 *       // spawn agent with: install_cmd->operation, install_cmd->packages
 *   }
 * 
 * Builder validates:
 * - Config exists in repository
 * - Not already installed (for install)
 * - Is installed (for remove)
 * - No conflicts with installed configs
 * - Not required by other installed configs (for remove)
 * - Includes all dependencies in correct order
 */
class DriverTransactionBuilder {
public:
    DriverTransactionBuilder() = default;

    using AgentCommandResult = std::expected<AgentCommand, TransactionError>;

    /**
     * Build install command with dependency resolution and conflict checking.
     */
    [[nodiscard]] static AgentCommandResult
    install(
        const ConfigProvider& provider,
        const std::string& config_name,
        BusType type
    );

    /**
     * Build remove command with dependency checking.
     */
    [[nodiscard]] static AgentCommandResult
    remove(
        const ConfigProvider& provider,
        const std::string& config_name,
        BusType type
    );
};

} // namespace mcp::mhwd
