/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "command.hpp"
#include "kernel_filter.hpp"
#include "kernel_formatter.hpp"
#include "common/output.hpp"
#include "common/table.hpp"

#include "kernel/KernelProvider.hpp"

#include <fmt/core.h>

#include <algorithm>
#include <functional>
#include <vector>

namespace mcp::cli::kernel {

using namespace mcp::kernel;

/**
 * Command to list available kernels.
 */
class ListCommand : public Command {
    bool m_installed_only;
    bool m_verbose;
    bool m_json;

public:
    explicit ListCommand(bool installed_only = false, bool verbose = false, bool json = false)
        : m_installed_only(installed_only)
        , m_verbose(verbose)
        , m_json(json)
    {}

    [[nodiscard]] int execute() override {
        KernelProvider provider;
        auto result = provider.get_kernels();

        if (!result) {
            return handle_error(result.error());
        }

        auto kernels = std::move(*result);

        // Filter to actual kernels only
        filter_actual_kernels(kernels);

        if (m_installed_only) {
            filter_installed(kernels);
        }

        if (kernels.empty()) {
            out().info("No kernels found matching the criteria.");
            return 0;
        }

        if (m_json) {
            return print_json(kernels);
        }

        return print_table(kernels);
    }

private:
    int handle_error(KernelError error) {
        switch (error) {
            case KernelError::DatabaseNotInitialized:
                out().error("Failed to initialize package database. Is pamac available?");
                break;
            case KernelError::ParseError:
                out().error("Failed to parse kernel information.");
                break;
            case KernelError::NotFound:
                out().error("No kernels found.");
                break;
        }
        return 1;
    }

    int print_json(const std::vector<Kernel>& kernels) {
        fmt::print("[\n");
        for (size_t i = 0; i < kernels.size(); ++i) {
            const auto& k = kernels[i];
            fmt::print(
                R"(  {{"package": "{}", "version": "{}", "repo": "{}", )"
                R"("installed": {}, "running": {}, "lts": {}, "supported": {}}}{})"
                "\n",
                k.package_name,
                k.available_version,
                k.repo,
                k.flags.installed,
                k.flags.in_use,
                k.flags.lts,
                !k.flags.not_supported,
                i < kernels.size() - 1 ? "," : ""
            );
        }
        fmt::print("]\n");
        return 0;
    }

    int print_table(const std::vector<Kernel>& kernels) {
        // Partition kernels: installed first
        std::vector<std::reference_wrapper<const Kernel>> installed;
        std::vector<std::reference_wrapper<const Kernel>> available;

        for (const auto& k : kernels) {
            if (k.is_installed()) {
                installed.emplace_back(k);
            } else {
                available.emplace_back(k);
            }
        }

        // Build table
        Table table;
        table.add_column("PACKAGE", Align::Left)
             .add_column("VERSION", Align::Right)
             .add_column("STATUS", Align::Left);

        // Add installed kernels
        for (const auto& ref : installed) {
            const auto& k = ref.get();
            add_kernel_row(table, k);
        }

        // Add separator if both sections exist
        if (!installed.empty() && !available.empty() && !m_installed_only) {
            table.add_separator();
        }

        // Add available kernels
        if (!m_installed_only) {
            for (const auto& ref : available) {
                const auto& k = ref.get();
                add_kernel_row(table, k);
            }
        }

        // Calculate widths and print
        table.auto_size();
        out().header("Available Kernels");
        table.print();

        // Summary
        fmt::print("\n");
        out().info(fmt::format("{} kernel(s) installed, {} available",
                               installed.size(), available.size()));

        return 0;
    }

    void add_kernel_row(Table& table, const Kernel& k) {
        std::vector<Cell> cells;

        // Name cell with color
        cells.emplace_back(k.package_name, KernelFormatter::name_style(k));

        // Version cell
        cells.emplace_back(KernelFormatter::version_string(k), fmt::emphasis::faint);

        // Status badges
        cells.emplace_back(KernelFormatter::badges(k));

        // Add row with prefix
        table.add_row(std::move(cells), KernelFormatter::row_prefix(k));

        // Verbose: extra info line
        if (m_verbose) {
            // TODO: Add verbose row support to table
        }
    }
};

} // namespace mcp::cli::kernel
