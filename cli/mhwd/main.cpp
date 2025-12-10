/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * mcp-mhwd - CLI for hardware driver configuration management.
 * Uses libmcp-driver for device detection and driver management.
 */

#include "commands/list_command.hpp"
#include "commands/install_command.hpp"
#include "commands/remove_command.hpp"

#include <mhwd/DeviceProvider.hpp>
#include <mhwd/ConfigProvider.hpp>

#include <coro/sync_wait.hpp>

#include <CLI/CLI.hpp>
#include <fmt/core.h>

#include <string>

int main(int argc, char** argv) {
    using namespace mcp::cli::mhwd;

    CLI::App app{"mcp-mhwd - Manjaro Hardware Driver Manager CLI", "mcp-mhwd"};
    app.set_version_flag("-V,--version", "1.0.0");

    bool no_color = false;

    app.add_flag("--no-color", no_color, "Disable colored output");

    auto* list_cmd = app.add_subcommand("list", "List hardware drivers");
    list_cmd->alias("ls");

    bool list_pci = false;
    bool list_usb = false;
    bool list_installed = false;
    bool list_available = false;
    bool list_verbose = false;

    list_cmd->add_flag("--pci", list_pci, "Show PCI drivers");
    list_cmd->add_flag("--usb", list_usb, "Show USB drivers");
    list_cmd->add_flag("-i,--installed", list_installed, "Show only installed drivers");
    list_cmd->add_flag("-a,--available", list_available, "Show only available drivers");
    list_cmd->add_flag("-v,--verbose", list_verbose, "Show detailed information");

    auto* install_cmd = app.add_subcommand("install", "Install a driver configuration");
    std::string install_config;
    bool install_pci = false;
    bool install_usb = false;
    bool force_install = false;
    bool no_confirm = false;

    install_cmd->add_option("config", install_config, "Driver config name (e.g., video-nvidia)")
               ->required();
    install_cmd->add_flag("--pci", install_pci, "Install PCI driver");
    install_cmd->add_flag("--usb", install_usb, "Install USB driver");
    install_cmd->add_flag("-f,--force", force_install, "Force installation (ignore conflicts)");
    install_cmd->add_flag("-y,--noconfirm", no_confirm, "Skip confirmation prompt");

    auto* remove_cmd = app.add_subcommand("remove", "Remove a driver configuration");
    remove_cmd->alias("uninstall");
    
    std::string remove_config;
    bool remove_pci = false;
    bool remove_usb = false;

    remove_cmd->add_option("config", remove_config, "Driver config name")
              ->required();
    remove_cmd->add_flag("--pci", remove_pci, "Remove PCI driver");
    remove_cmd->add_flag("--usb", remove_usb, "Remove USB driver");
    remove_cmd->add_flag("-y,--noconfirm", no_confirm, "Skip confirmation prompt");

    app.require_subcommand(1);

    CLI11_PARSE(app, argc, argv);

    mcp::mhwd::DeviceProvider device_provider;
    coro::sync_wait(device_provider.scan());
    
    mcp::mhwd::ConfigProvider config_provider(device_provider);

    if (*list_cmd) {
        return ListCommand(
            config_provider,
            device_provider,
            list_pci,
            list_usb,
            list_installed,
            list_available,
            list_verbose,
            !no_color
        ).execute();
    }

    if (*install_cmd) {
        mcp::mhwd::BusType type = install_pci ? mcp::mhwd::BusType::PCI : mcp::mhwd::BusType::USB;
        return InstallCommand(
            config_provider,
            install_config,
            type,
            force_install,
            no_confirm,
            !no_color
        ).execute();
    }

    if (*remove_cmd) {
        mcp::mhwd::BusType type = remove_pci ? mcp::mhwd::BusType::PCI : mcp::mhwd::BusType::USB;
        return RemoveCommand(
            config_provider,
            remove_config,
            type,
            no_confirm,
            !no_color
        ).execute();
    }

    return 0;
}
