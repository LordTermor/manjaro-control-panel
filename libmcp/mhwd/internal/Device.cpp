/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "mhwd/Device.hpp"

#include <algorithm>

/*
 * Hardware pattern matching implementation.
 * Performs ID matching with wildcard and blacklist support.
 */

namespace mcp::mhwd {

namespace {

constexpr bool matches_id(const std::string& device_id, const std::string& pattern)
{
    return pattern == "*" || pattern == device_id;
}

}

bool Device::matches(const HardwarePattern& pattern) const
{
    auto matches_any = [](const std::string& device_id, const std::vector<std::string>& patterns) {
        return std::ranges::any_of(patterns, [&device_id](const std::string& p) { 
            return matches_id(device_id, p); 
        });
    };

    return matches_any(info_.class_id, pattern.class_ids) &&
           !matches_any(info_.class_id, pattern.blacklisted_class_ids) &&
           matches_any(info_.vendor_id, pattern.vendor_ids) &&
           !matches_any(info_.vendor_id, pattern.blacklisted_vendor_ids) &&
           matches_any(info_.device_id, pattern.device_ids) &&
           !matches_any(info_.device_id, pattern.blacklisted_device_ids);
}

} // namespace mcp::mhwd
