/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "colors.hpp"

#include <fmt/color.h>
#include <fmt/core.h>

#include <string_view>
#include <unistd.h>

namespace mcp::cli {

/**
 * Output manager for formatted terminal output.
 * Handles color support detection and provides helper methods.
 */
class Output {
    bool m_use_color = true;

    Output() : m_use_color(isatty(fileno(stdout)) != 0) {}

public:
    static Output& instance() {
        static Output s_instance;
        return s_instance;
    }

    void set_color_enabled(bool enabled) { m_use_color = enabled; }
    [[nodiscard]] bool color_enabled() const { return m_use_color; }

    void header(std::string_view title) const {
        if (m_use_color) {
            fmt::print("\n{} {}\n",
                       fmt::styled(symbol::kernel, fmt::emphasis::bold),
                       fmt::styled(title, fmt::emphasis::bold));
            fmt::print("{}\n", fmt::styled("─────────────────────────────────────────────────────",
                                          fmt::emphasis::faint));
        } else {
            fmt::print("\n{} {}\n", symbol::kernel, title);
            fmt::print("─────────────────────────────────────────────────────\n");
        }
    }

    void error(std::string_view message) const {
        if (m_use_color) {
            fmt::print(stderr, "{} {}\n",
                       fmt::styled("Error:", fmt::emphasis::bold | fmt::fg(fmt::color::red)),
                       message);
        } else {
            fmt::print(stderr, "Error: {}\n", message);
        }
    }

    void success(std::string_view message) const {
        if (m_use_color) {
            fmt::print("{} {}\n",
                       fmt::styled(symbol::check, fmt::fg(fmt::color::green)),
                       fmt::styled(message, fmt::emphasis::bold));
        } else {
            fmt::print("{} {}\n", symbol::check, message);
        }
    }

    void info(std::string_view message) const {
        if (m_use_color) {
            fmt::print("{} {}\n",
                       fmt::styled(symbol::info, fmt::fg(fmt::color::cyan)),
                       message);
        } else {
            fmt::print("{} {}\n", symbol::info, message);
        }
    }

    void print(std::string_view text) const {
        fmt::print("{}", text);
    }

    void println(std::string_view text = "") const {
        fmt::print("{}\n", text);
    }
};

/// Global output instance accessor
inline Output& out() { return Output::instance(); }

} // namespace mcp::cli
