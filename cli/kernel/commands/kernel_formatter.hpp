/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "kernel/Kernel.hpp"
#include "common/colors.hpp"
#include "common/output.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include <string>
#include <string_view>

namespace mcp::cli::kernel {

using mcp::kernel::Kernel;

/**
 * Formatting utilities for kernel display.
 */
class KernelFormatter {
public:
    /// Get the display style for a kernel name
    static fmt::text_style name_style(const Kernel& k) {
        if (k.flags.in_use) return fmt::fg(fmt::color::cyan);
        if (k.flags.installed) return fmt::fg(fmt::color::green);
        return fmt::text_style();
    }

    /// Get the row prefix for a kernel (e.g., running indicator)
    static std::string row_prefix(const Kernel& k) {
        if (k.flags.in_use && out().color_enabled()) {
            return fmt::format("{} ", fmt::styled(symbol::running, fmt::fg(fmt::color::green)));
        } else if (k.flags.in_use) {
            return fmt::format("{} ", symbol::running);
        }
        return "";
    }

    /// Format a badge
    static std::string badge(std::string_view text, fmt::color color) {
        if (out().color_enabled()) {
            return fmt::format("{}", fmt::styled(fmt::format("[{}]", text), fmt::fg(color)));
        }
        return fmt::format("[{}]", text);
    }

    /// Format status badges for a kernel
    static std::string badges(const Kernel& k) {
        std::string result;

        if (k.flags.in_use) {
            result += badge("RUNNING", fmt::color::green) + " ";
        }
        if (k.flags.installed) {
            result += badge("installed", fmt::color::green) + " ";
        }
        if (k.flags.lts) {
            result += badge("LTS", fmt::color::yellow) + " ";
        }
        if (k.flags.recommended) {
            result += badge("recommended", fmt::color::cyan) + " ";
        }
        if (k.flags.real_time) {
            result += badge("RT", fmt::color::magenta) + " ";
        }
        if (k.flags.experimental) {
            result += badge("experimental", fmt::color::yellow) + " ";
        }
        if (k.flags.not_supported) {
            result += badge("unsupported", fmt::color::red) + " ";
        }

        return result;
    }

    /// Get version string for display
    static std::string version_string(const Kernel& k) {
        if (!k.available_version.empty()) {
            return k.available_version;
        }
        return k.version.to_string();
    }

    /// Print detailed kernel information
    static void print_detail(const Kernel& k) {
        auto& o = out();

        o.header(fmt::format("Kernel: {}", k.package_name));

        auto print_field = [](std::string_view label, auto&& value) {
            fmt::print("{:>16}: {}\n", label, value);
        };

        auto print_field_styled = [](std::string_view label, std::string_view value, fmt::text_style style) {
            if (out().color_enabled()) {
                fmt::print("{:>16}: {}\n", label, fmt::styled(value, style));
            } else {
                fmt::print("{:>16}: {}\n", label, value);
            }
        };

        print_field_styled("Package", k.package_name, fmt::emphasis::bold);
        print_field("Version", k.version.to_string());
        print_field_styled("Repository", k.repo, fmt::fg(fmt::color::blue));

        if (!k.available_version.empty()) {
            print_field("Available", k.available_version);
        }

        if (k.flags.installed && !k.installed_version.empty()) {
            print_field_styled("Installed", k.installed_version, fmt::fg(fmt::color::green));
        }

        print_field("Status", badges(k));

        // Flags breakdown
        fmt::print("\n  Flags:\n");

        auto flag_line = [](std::string_view name, bool value) {
            if (out().color_enabled()) {
                if (value) {
                    fmt::print("    {} {}\n", fmt::styled(symbol::check, fmt::fg(fmt::color::green)), name);
                } else {
                    fmt::print("    {} {}\n", fmt::styled(symbol::cross, fmt::emphasis::faint), name);
                }
            } else {
                fmt::print("    {} {}\n", value ? symbol::check : symbol::cross, name);
            }
        };

        flag_line("Installed", k.flags.installed);
        flag_line("Running", k.flags.in_use);
        flag_line("LTS", k.flags.lts);
        flag_line("Recommended", k.flags.recommended);
        flag_line("Real-time", k.flags.real_time);
        flag_line("Experimental", k.flags.experimental);
        flag_line("Supported", !k.flags.not_supported);

        fmt::print("\n");
    }
};

} // namespace mcp::cli::kernel
