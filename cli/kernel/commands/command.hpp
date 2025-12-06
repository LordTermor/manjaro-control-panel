/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

namespace mcp::cli {

/**
 * Base interface for CLI commands.
 */
class Command {
public:
    virtual ~Command() = default;
    
    /// Execute the command, returns exit code
    [[nodiscard]] virtual int execute() = 0;
};

} // namespace mcp::cli
