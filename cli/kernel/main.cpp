/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * mcp-kernel - CLI for kernel package discovery and management.
 * Uses libmcp-kernel for kernel information and fmtlib for output.
 */

#include "commands/list_command.hpp"
#include "commands/info_command.hpp"
#include "commands/running_command.hpp"
#include "commands/install_command.hpp"
#include "common/output.hpp"

#include <pamac/config.hpp>
#include <pamac/database.hpp>

#include <CLI/CLI.hpp>
#include <fmt/core.h>

#include <string>

int main(int argc, char** argv) {
    using namespace mcp::cli;
    using namespace mcp::cli::kernel;

    CLI::App app{"mcp-kernel - Manjaro Kernel Manager CLI", "mcp-kernel"};
    app.set_version_flag("-V,--version", "1.0.0");

    // Global options
    bool no_color = false;
    std::string config_path = "/etc/pamac.conf";

    app.add_flag("--no-color", no_color, "Disable colored output");
    app.add_option("-c,--config", config_path, "Path to pamac.conf")
       ->check(CLI::ExistingFile);

    // List subcommand
    auto* list_cmd = app.add_subcommand("list", "List available kernels");
    list_cmd->alias("ls");

    bool list_installed = false;
    bool list_verbose = false;
    bool list_json = false;

    list_cmd->add_flag("-i,--installed", list_installed, "Show only installed kernels");
    list_cmd->add_flag("-v,--verbose", list_verbose, "Show detailed information");
    list_cmd->add_flag("-j,--json", list_json, "Output in JSON format");

    // Running subcommand
    auto* running_cmd = app.add_subcommand("running", "Show currently running kernel");
    running_cmd->alias("current");

    // Info subcommand
    auto* info_cmd = app.add_subcommand("info", "Show detailed kernel information");
    std::string info_package;
    info_cmd->add_option("package", info_package, "Kernel package name (e.g., linux66)")
            ->required();

    // Install subcommand
    auto* install_cmd = app.add_subcommand("install", "Install a kernel package");
    std::string install_package;
    bool download_only = false;
    bool no_confirm = false;

    install_cmd->add_option("package", install_package, "Kernel package name (e.g., linux66)")
               ->required();
    install_cmd->add_flag("-d,--download-only", download_only, "Download packages without installing");
    install_cmd->add_flag("-y,--noconfirm", no_confirm, "Skip confirmation prompt");

    // Default behavior (no subcommand = list)
    app.require_subcommand(0, 1);

    CLI11_PARSE(app, argc, argv);

    // Setup color output
    out().set_color_enabled(!no_color);

    // Initialize the pamac database
    auto status = pamac::Database::initialize(config_path);
    if (status != pamac::DatabaseStatus::Ok &&
        status != pamac::DatabaseStatus::AlreadyInitialized) {
        out().error("Failed to initialize package database.");
        return 1;
    }

    // Route to appropriate command
    if (*list_cmd || app.get_subcommands().empty()) {
        return ListCommand(list_installed, list_verbose, list_json).execute();
    }

    if (*running_cmd) {
        return RunningCommand().execute();
    }

    if (*info_cmd) {
        return InfoCommand(info_package).execute();
    }

    if (*install_cmd) {
        return InstallCommand(install_package, download_only, no_confirm).execute();
    }

    return 0;
}
