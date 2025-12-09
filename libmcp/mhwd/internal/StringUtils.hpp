/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * String manipulation utilities for config parsing.
 */

#pragma once

#include <ranges>
#include <string>
#include <string_view>
#include <vector>

namespace mcp::mhwd::string_utils {

namespace rg = std::ranges;
namespace vw = std::ranges::views;

/**
 * Trim whitespace from both ends of a string.
 */
inline std::string trim(std::string_view str)
{
    const auto start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }

    const auto end = str.find_last_not_of(" \t\r\n");
    return std::string(str.substr(start, end - start + 1));
}

/**
 * Remove surrounding quotes from a string.
 */
inline std::string trim_quotes(std::string_view str)
{
    if (str.size() >= 2 && str.front() == '"' && str.back() == '"') {
        return std::string(str.substr(1, str.size() - 2));
    }
    return std::string(str);
}

/**
 * Convert string to lowercase.
 */
inline std::string to_lower(const std::string& str)
{
    return str 
        | vw::transform([](unsigned char c) { return std::tolower(c); })
        | rg::to<std::string>();
}

/**
 * Split space-separated values and convert to lowercase.
 * Filters out empty values.
 */
inline std::vector<std::string> split_values(std::string_view str)
{
    auto trimmed = trim(str);
    if (trimmed.empty()) {
        return {};
    }
    
    return trimmed 
        | vw::split(' ')
        | vw::filter([](auto&& word) { return !rg::empty(word); })
        | vw::transform([](auto&& word) {
            return to_lower(std::string(rg::begin(word), rg::end(word)));
        })
        | rg::to<std::vector>();
}

} // namespace mcp::mhwd::string_utils
