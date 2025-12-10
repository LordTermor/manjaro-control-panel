/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Unified transaction agent command for kernel and driver operations.
 * Simple command representation with factory functions for construction.
 */

#pragma once

#include <string>
#include <vector>

namespace mcp::agent {

struct Command {
    std::string operation;              // "install", "remove", "upgrade"
    std::vector<std::string> packages;
    bool force = false;                 // Force removal even if in use
    bool refresh = false;               // Refresh package databases before upgrade
};

inline Command make_install(std::vector<std::string> packages)
{
    return {.operation = "install", .packages = std::move(packages)};
}

inline Command make_remove(std::vector<std::string> packages, bool force = false)
{
    return {.operation = "remove", .packages = std::move(packages), .force = force};
}

inline Command make_upgrade(bool refresh = false)
{
    return {.operation = "upgrade", .packages = {}, .refresh = refresh};
}

} // namespace mcp::agent
