/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Kernel model - represents a Linux kernel package with version info and flags.
 * Clean data model without dependencies on external services.
 */

#pragma once

#include <compare>
#include <string>
#include <vector>

namespace mcp::kernel {

/**
 * Kernel version with major.minor.patch components.
 * Provides comparison operators for version ordering.
 */
struct KernelVersion {
    int major = 0;
    int minor = 0;
    std::string patch;

    [[nodiscard]] std::string to_string() const
    {
        auto result = std::to_string(major) + "." + std::to_string(minor);
        if (!patch.empty()) {
            result += "." + patch;
        }
        return result;
    }

    auto operator<=>(const KernelVersion& rhs) const
    {
        if (auto cmp = major <=> rhs.major; cmp != 0) return cmp;
        return minor <=> rhs.minor;
    }

    bool operator==(const KernelVersion& rhs) const = default;
};

/**
 * Kernel flags - bitfield for kernel characteristics.
 */
struct KernelFlags {
    bool lts : 1 = false;
    bool recommended : 1 = false;
    bool installed : 1 = false;
    bool not_supported : 1 = false;
    bool real_time : 1 = false;
    bool in_use : 1 = false;
    bool experimental : 1 = false;
};

/**
 * Kernel model - complete representation of a kernel package.
 */
struct Kernel {
    std::string package_name;
    KernelVersion version;
    KernelFlags flags;
    std::string repo;
    std::string installed_version;
    std::string available_version;
    std::string changelog_url;
    std::vector<std::string> extra_modules;

    [[nodiscard]] bool is_installed() const { return flags.installed; }
    [[nodiscard]] bool is_lts() const { return flags.lts; }
    [[nodiscard]] bool is_recommended() const { return flags.recommended; }
    [[nodiscard]] bool is_in_use() const { return flags.in_use; }
    [[nodiscard]] bool is_supported() const { return !flags.not_supported; }

    bool operator==(const Kernel& rhs) const
    {
        return package_name == rhs.package_name && version == rhs.version;
    }

    auto operator<=>(const Kernel& rhs) const
    {
        if (auto cmp = package_name <=> rhs.package_name; cmp != 0) return cmp;
        return version <=> rhs.version;
    }
};

using KernelVector = std::vector<Kernel>;

} // namespace mcp::kernel
