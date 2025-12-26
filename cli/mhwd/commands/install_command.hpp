/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <mhwd/ConfigProvider.hpp>

namespace mcp::cli::mhwd {

/**
 * Install a driver configuration.
 */
class InstallCommand {
public:
    InstallCommand(
        mcp::mhwd::ConfigProvider& provider,
        const std::string& config_name,
        mcp::mhwd::BusType type,
        bool force,
        bool no_confirm,
        bool color_enabled
    )
        : provider_(provider)
        , config_name_(config_name)
        , type_(type)
        , force_(force)
        , no_confirm_(no_confirm)
        , color_enabled_(color_enabled)
    {
    }

    int execute();

private:
    mcp::mhwd::ConfigProvider& provider_;
    std::string config_name_;
    mcp::mhwd::BusType type_;
    bool force_;
    bool no_confirm_;
    bool color_enabled_;
};

} // namespace mcp::cli::mhwd
