/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <string_view>

namespace mcp::cli {

/// Unicode symbols for visual output
namespace symbol {

inline constexpr std::string_view check   = "✓";
inline constexpr std::string_view cross   = "✗";
inline constexpr std::string_view arrow   = "→";
inline constexpr std::string_view bullet  = "•";
inline constexpr std::string_view star    = "★";
inline constexpr std::string_view diamond = "◆";
inline constexpr std::string_view circle  = "●";
inline constexpr std::string_view square  = "■";
inline constexpr std::string_view warning = "⚠";
inline constexpr std::string_view info    = "ℹ";
inline constexpr std::string_view running = "▶";
inline constexpr std::string_view kernel  = "🐧";

} // namespace symbol

} // namespace mcp::cli
