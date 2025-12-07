/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "kernel/Kernel.hpp"

#include <algorithm>
#include <array>
#include <ranges>
#include <string_view>
#include <vector>

namespace mcp::cli::kernel {

/**
 * Check if a kernel package is an actual kernel (not a driver/module).
 * 
 * Valid examples:
 *   - linux66, linux612, linux61 (no suffix)
 *   - linux66-rt, linux612-rt (real-time)
 *   - linux-lts, linux-zen, linux-hardened (special kernels)
 * 
 * Invalid examples:
 *   - linux66-nvidia, linux66-headers (drivers/headers)
 *   - linux66-virtualbox-host-modules (modules)
 */
inline bool is_actual_kernel(const mcp::kernel::Kernel& k) {
    const auto& name = k.package_name;
    
    auto dash_count = std::ranges::count(name, '-');
    
    // No dashes: linux66, linux612 - always valid
    if (dash_count == 0) {
        return true;
    }
    
    // One dash: check for valid kernel suffixes
    if (dash_count == 1) {
        static constexpr std::array valid_suffixes = {
            std::string_view{"-rt"},
            std::string_view{"-lts"},
            std::string_view{"-zen"},
            std::string_view{"-hardened"},
            std::string_view{"-cachyos"},
        };
        
        for (const auto& suffix : valid_suffixes) {
            if (name.ends_with(suffix)) {
                return true;
            }
        }
    }
    
    return false;
}

inline void filter_actual_kernels(std::vector<mcp::kernel::Kernel>& kernels) {
    std::erase_if(kernels, [](const auto& k) { return !is_actual_kernel(k); });
}

inline void filter_installed(std::vector<mcp::kernel::Kernel>& kernels) {
    std::erase_if(kernels, [](const auto& k) { return !k.is_installed(); });
}

} // namespace mcp::cli::kernel
