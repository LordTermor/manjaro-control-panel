/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Device - hardware device with pattern matching.
 * Thin wrapper around DeviceInfo that knows how to match against patterns.
 */

#pragma once

#include "internal/Types.hpp"

namespace mcp::mhwd {

/**
 * Hardware device detected on the system.
 * 
 * Represents a PCI or USB device with identity information
 * and ability to match against driver configuration patterns.
 */
class Device {
    DeviceInfo info_;
    BusType bus_type_;

public:
    /**
     * Construct device from detected hardware info.
     */
    Device(DeviceInfo info, BusType type)
        : info_(std::move(info)), bus_type_(type)
    {
    }

    [[nodiscard]] const DeviceInfo& info() const { return info_; }
    [[nodiscard]] BusType bus_type() const { return bus_type_; }

    /**
     * Check if this device matches a hardware pattern.
     * Supports wildcards (*) and blacklists.
     */
    [[nodiscard]] bool matches(const HardwarePattern& pattern) const;

    auto operator<=>(const Device& rhs) const = default;
};

} // namespace mcp::mhwd
