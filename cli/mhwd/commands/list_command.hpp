/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <mhwd/DeviceProvider.hpp>
#include <mhwd/ConfigProvider.hpp>

namespace mcp::cli::mhwd {

/**
 * List available and installed driver configurations.
 */
class ListCommand {
public:
    ListCommand(
        mcp::mhwd::ConfigProvider& provider,
        const mcp::mhwd::DeviceProvider& devices,
        bool pci,
        bool usb,
        bool installed_only,
        bool available_only,
        bool verbose,
        bool color_enabled
    )
        : provider_(provider)
        , devices_(devices)
        , show_pci_(pci)
        , show_usb_(usb)
        , installed_only_(installed_only)
        , available_only_(available_only)
        , verbose_(verbose)
        , color_enabled_(color_enabled)
    {
    }

    int execute();

private:
    mcp::mhwd::ConfigProvider& provider_;
    const mcp::mhwd::DeviceProvider& devices_;
    bool show_pci_;
    bool show_usb_;
    bool installed_only_;
    bool available_only_;
    bool verbose_;
    bool color_enabled_;

    void list_configs(mcp::mhwd::BusType type);
    void print_config(const mcp::mhwd::Config& config, bool is_installed);
};

} // namespace mcp::cli::mhwd
