/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Base device scanner template using CRTP pattern.
 * Provides common udev scanning logic for different bus types.
 */

#pragma once

#include "../../Types.hpp"
#include "UdevUtils.hpp"
#include "../../Device.hpp"

#include <vector>

namespace mcp::mhwd {

template<typename Derived>
class DeviceScanner {
public:
    static std::vector<Device> scan()
    {
        using namespace udev;
        
        std::vector<Device> devices;

        UdevPtr udev_ctx(udev_new());
        if (!udev_ctx) {
            return devices;
        }

        UdevEnumeratePtr enumerate(udev_enumerate_new(udev_ctx.get()));
        if (!enumerate) {
            return devices;
        }

        udev_enumerate_add_match_subsystem(enumerate.get(), Derived::subsystem());
        udev_enumerate_scan_devices(enumerate.get());

        udev_list_entry* entry;
        udev_list_entry_foreach(entry, udev_enumerate_get_list_entry(enumerate.get()))
        {
            const char* syspath = udev_list_entry_get_name(entry);
            UdevDevicePtr device(udev_device_new_from_syspath(udev_ctx.get(), syspath));
            
            if (!device || !Derived::is_valid(device.get())) {
                continue;
            }

            DeviceInfo info = Derived::extract_info(device.get(), syspath);
            devices.emplace_back(std::move(info), Derived::bus_type());
        }

        return devices;
    }
};

} // namespace mcp::mhwd
