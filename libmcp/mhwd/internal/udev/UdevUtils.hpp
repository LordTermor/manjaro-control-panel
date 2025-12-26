/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * RAII wrappers and utility functions for libudev.
 */

#pragma once

extern "C" {
#include <libudev.h>
}

#include <format>
#include <memory>
#include <string>
#include <string_view>

namespace mcp::mhwd::udev {

// RAII wrappers for udev resources
struct UdevDeleter {
    void operator()(::udev* ctx) const
    {
        if (ctx) {
            udev_unref(ctx);
        }
    }
};

struct UdevEnumerateDeleter {
    void operator()(::udev_enumerate* enumerate) const
    {
        if (enumerate) {
            udev_enumerate_unref(enumerate);
        }
    }
};

struct UdevDeviceDeleter {
    void operator()(::udev_device* device) const
    {
        if (device) {
            udev_device_unref(device);
        }
    }
};

using UdevPtr = std::unique_ptr<::udev, UdevDeleter>;
using UdevEnumeratePtr = std::unique_ptr<::udev_enumerate, UdevEnumerateDeleter>;
using UdevDevicePtr = std::unique_ptr<::udev_device, UdevDeviceDeleter>;

// String conversion utilities
inline std::string to_hex(unsigned long value, int width)
{
    return std::format("{:0{}x}", value, width);
}

inline std::string safe_string(const char* str)
{
    return str ? std::string(str) : std::string{};
}

inline std::string safe_attr(::udev_device* device, std::string_view attr)
{
    const char* value = udev_device_get_sysattr_value(device, attr.data());
    return safe_string(value);
}

inline std::string safe_property(::udev_device* device, std::string_view property)
{
    const char* value = udev_device_get_property_value(device, property.data());
    return safe_string(value);
}

inline unsigned long hex_to_ulong(const char* hex_str)
{
    if (!hex_str) return 0;
    return std::strtoul(hex_str, nullptr, 16);
}

} // namespace mcp::mhwd::udev
