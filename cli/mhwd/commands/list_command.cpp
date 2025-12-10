/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "list_command.hpp"
#include "common/output.hpp"
#include "common/symbols.hpp"

#include <coro/sync_wait.hpp>

#include <fmt/color.h>
#include <fmt/core.h>

#include <algorithm>

namespace mcp::cli::mhwd {

using mcp::cli::out;
using namespace mcp::cli::symbol;

int ListCommand::execute()
{
    out().set_color_enabled(color_enabled_);
    
    bool show_both = !show_pci_ && !show_usb_;

    if (show_pci_ || show_both) {
        out().header("PCI Drivers", symbol::pci);
        list_configs(mcp::mhwd::BusType::PCI);
    }

    if (show_usb_ || show_both) {
        out().header("USB Drivers", symbol::usb);
        list_configs(mcp::mhwd::BusType::USB);
    }

    return 0;
}

void ListCommand::list_configs(mcp::mhwd::BusType type)
{
    auto installed_result = coro::sync_wait(provider_.get_installed_configs(type));
    auto available_result = coro::sync_wait(provider_.get_available_configs(type));

    if (!installed_result && !available_result) {
        out().error("Error loading configurations");
        return;
    }

    std::vector<mcp::mhwd::Config> installed = installed_result.value_or(std::vector<mcp::mhwd::Config>{});
    std::vector<mcp::mhwd::Config> available = available_result.value_or(std::vector<mcp::mhwd::Config>{});

    auto matching_configs = coro::sync_wait(provider_.find_matching_configs(type));

    if (matching_configs.empty()) {
        out().info("No suitable drivers found for your hardware");
        return;
    }

    if (!available_only_ && !installed.empty()) {
        std::vector<mcp::mhwd::Config> installed_matching;
        for (const auto& inst : installed) {
            bool matches = std::ranges::any_of(matching_configs, [&](const auto& match) {
                return match.name() == inst.name();
            });
            if (matches) {
                installed_matching.push_back(inst);
            }
        }

        if (!installed_matching.empty()) {
            fmt::print("\n{} {}\n",
                       fmt::styled(symbol::installed, fmt::fg(fmt::color::green) | fmt::emphasis::bold),
                       fmt::styled("Installed", fmt::emphasis::bold));
            for (const auto& config : installed_matching) {
                print_config(config, true);
            }
        }
    }

    if (!installed_only_) {
        std::vector<mcp::mhwd::Config> available_matching;
        for (const auto& avail : matching_configs) {
            bool is_installed = std::ranges::any_of(installed, [&](const auto& inst) {
                return inst.name() == avail.name();
            });
            if (!is_installed) {
                available_matching.push_back(avail);
            }
        }

        if (!available_matching.empty()) {
            fmt::print("\n{} {}\n",
                       fmt::styled(symbol::circle, fmt::fg(fmt::color::cyan) | fmt::emphasis::bold),
                       fmt::styled("Available", fmt::emphasis::bold));
            for (const auto& config : available_matching) {
                print_config(config, false);
            }
        }
    }

    fmt::print("\n");
}

void ListCommand::print_config(const mcp::mhwd::Config& config, bool is_installed)
{
    auto name_style = color_enabled_ ? 
        (is_installed ? fmt::fg(fmt::color::green) | fmt::emphasis::bold : 
                       fmt::fg(fmt::color::cyan) | fmt::emphasis::bold) :
        fmt::text_style{};

    fmt::print("  {} ", fmt::styled(symbol::arrow, fmt::fg(fmt::color::dark_gray)));
    fmt::print(name_style, "{}", config.name());
    
    if (verbose_) {
        fmt::print(" {}", fmt::styled(fmt::format("v{}", config.version()), 
                                     fmt::emphasis::faint));
        fmt::print(" {} ", fmt::styled(symbol::bullet, fmt::fg(fmt::color::dark_gray)));
        fmt::print("{}", config.description());
        
        if (config.is_free_driver()) {
            fmt::print(" {}", fmt::styled("[free]", fmt::fg(fmt::color::green)));
        } else {
            fmt::print(" {}", fmt::styled("[proprietary]", fmt::fg(fmt::color::yellow)));
        }
    } else {
        if (!config.is_free_driver() && color_enabled_) {
            fmt::print(" {}", fmt::styled("⚡", fmt::fg(fmt::color::yellow)));
        }
    }
    
    fmt::print("\n");
}

} // namespace mcp::cli::mhwd
