/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Unicode symbols for CLI visual output.
 * Provides consistent glyphs for status indicators, progress, and decorations.
 */

#pragma once

#include <string_view>

namespace mcp::cli::symbol {

// Status indicators
inline constexpr std::string_view check   = "✓";
inline constexpr std::string_view cross   = "✗";
inline constexpr std::string_view warning = "⚠";
inline constexpr std::string_view info    = "ℹ";

// Decorations
inline constexpr std::string_view arrow   = "→";
inline constexpr std::string_view bullet  = "•";
inline constexpr std::string_view star    = "★";
inline constexpr std::string_view diamond = "◆";
inline constexpr std::string_view circle  = "●";
inline constexpr std::string_view square  = "■";

// State indicators
inline constexpr std::string_view running   = "▶";
inline constexpr std::string_view installed = "▶";

// Module-specific icons
inline constexpr std::string_view kernel = "🐧";
inline constexpr std::string_view driver = "🔧";
inline constexpr std::string_view pci    = "💻";
inline constexpr std::string_view usb    = "🔌";

} // namespace mcp::cli::symbol
