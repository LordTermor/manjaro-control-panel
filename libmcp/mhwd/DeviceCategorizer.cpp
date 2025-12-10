#include "DeviceCategorizer.hpp"
#include <cstring>
#include <iostream>

namespace mcp::mhwd {

DeviceCategorizer::DeviceCategorizer()
    : m_udev(udev_new())
{
    if (!m_udev) {
        throw std::runtime_error("Failed to create udev context");
    }
}

DeviceCategorizer::~DeviceCategorizer() = default;

std::string DeviceCategorizer::getDeviceProperty(udev_device* dev, const char* property) {
    const char* value = udev_device_get_property_value(dev, property);
    return value ? value : "";
}

std::string DeviceCategorizer::getVendorName(udev_device* dev) {
    auto vendor = getDeviceProperty(dev, "ID_VENDOR_FROM_DATABASE");
    if (vendor.empty()) {
        vendor = getDeviceProperty(dev, "ID_VENDOR");
    }
    return vendor;
}

std::string DeviceCategorizer::getDeviceName(udev_device* dev) {
    auto model = getDeviceProperty(dev, "ID_MODEL_FROM_DATABASE");
    if (model.empty()) {
        model = getDeviceProperty(dev, "ID_MODEL");
    }
    if (model.empty()) {
        const char* sysname = udev_device_get_sysname(dev);
        model = sysname ? sysname : "Unknown Device";
    }
    return model;
}

DeviceCategory DeviceCategorizer::categorizeDevice(udev_device* dev) {
    const char* subsystem = udev_device_get_subsystem(dev);
    if (!subsystem) return DeviceCategory::Misc;

    // PCI class-based categorization
    auto pci_class = getDeviceProperty(dev, "ID_PCI_CLASS");
    if (!pci_class.empty()) {
        unsigned long pci_class_code = std::stoul(pci_class, nullptr, 16);
        
        // PCI class codes (upper 16 bits)
        unsigned int base_class = (pci_class_code >> 16) & 0xFF;
        unsigned int sub_class = (pci_class_code >> 8) & 0xFF;
        
        switch (base_class) {
            case 0x03: // Display controller
                return DeviceCategory::Display;
            case 0x02: // Network controller
                return DeviceCategory::Network;
            case 0x04: // Multimedia controller
                if (sub_class == 0x01 || sub_class == 0x03) // Audio/HD Audio
                    return DeviceCategory::Audio;
                return DeviceCategory::Misc;
            case 0x01: // Mass storage controller
                return DeviceCategory::Storage;
            case 0x0C: // Serial bus controller
                if (sub_class == 0x03) // USB controller
                    return DeviceCategory::Misc;
                return DeviceCategory::Misc;
            default:
                break;
        }
    }

    // USB interface class-based categorization
    auto usb_interface_class = getDeviceProperty(dev, "ID_USB_INTERFACE_CLASS");
    if (!usb_interface_class.empty()) {
        int usb_class = std::stoi(usb_interface_class);
        
        switch (usb_class) {
            case 0x01: // Audio
                return DeviceCategory::Audio;
            case 0x03: // HID (Human Interface Device)
                return DeviceCategory::Input;
            case 0x08: // Mass Storage
                return DeviceCategory::Storage;
            case 0x0E: // Video
                return DeviceCategory::Display;
            case 0xE0: // Wireless (Bluetooth, WiFi)
                return DeviceCategory::Network;
            default:
                break;
        }
    }

    // Subsystem-based categorization
    std::string subsys(subsystem);
    
    if (subsys == "drm" || subsys == "graphics") {
        return DeviceCategory::Display;
    }
    
    if (subsys == "net" || subsys == "ieee80211") {
        return DeviceCategory::Network;
    }
    
    if (subsys == "sound" || subsys == "audio") {
        return DeviceCategory::Audio;
    }
    
    if (subsys == "block" || subsys == "nvme" || subsys == "scsi") {
        return DeviceCategory::Storage;
    }
    
    if (subsys == "input" || subsys == "hid") {
        return DeviceCategory::Input;
    }

    // Device type hints from properties
    auto devtype = getDeviceProperty(dev, "DEVTYPE");
    if (devtype == "disk") {
        return DeviceCategory::Storage;
    }

    // ID_INPUT_* properties for input devices
    if (!getDeviceProperty(dev, "ID_INPUT_KEYBOARD").empty() ||
        !getDeviceProperty(dev, "ID_INPUT_MOUSE").empty() ||
        !getDeviceProperty(dev, "ID_INPUT_TOUCHPAD").empty()) {
        return DeviceCategory::Input;
    }

    return DeviceCategory::Misc;
}

std::vector<CategorizedDevice> DeviceCategorizer::enumerateDevices() {
    std::vector<CategorizedDevice> devices;

    // Enumerate PCI devices
    {
        std::unique_ptr<udev_enumerate, udev_enumerate_deleter> enumerate(
            udev_enumerate_new(m_udev.get()));
        
        if (!enumerate) {
            return devices;
        }

        udev_enumerate_add_match_subsystem(enumerate.get(), "pci");
        udev_enumerate_scan_devices(enumerate.get());

        struct udev_list_entry* devices_list = udev_enumerate_get_list_entry(enumerate.get());
        struct udev_list_entry* entry;

        udev_list_entry_foreach(entry, devices_list) {
            const char* path = udev_list_entry_get_name(entry);
            std::unique_ptr<udev_device, udev_device_deleter> dev(
                udev_device_new_from_syspath(m_udev.get(), path));

            if (!dev) continue;

            CategorizedDevice device;
            device.id = path;
            device.name = getDeviceName(dev.get());
            device.vendor = getVendorName(dev.get());
            device.subsystem = "pci";
            device.devpath = path;
            device.category = categorizeDevice(dev.get());
            device.pci_device = true;
            device.usb_device = false;

            // Filter out bridges, host controllers, and other infrastructure
            auto pci_class = getDeviceProperty(dev.get(), "ID_PCI_CLASS");
            if (!pci_class.empty()) {
                unsigned long class_code = std::stoul(pci_class, nullptr, 16);
                unsigned int base_class = (class_code >> 16) & 0xFF;
                
                // Skip bridges (0x06), memory controllers (0x05), etc.
                if (base_class == 0x06 || base_class == 0x05) {
                    device.category = DeviceCategory::Misc;
                }
            }

            devices.push_back(std::move(device));
        }
    }

    // Enumerate USB devices
    {
        std::unique_ptr<udev_enumerate, udev_enumerate_deleter> enumerate(
            udev_enumerate_new(m_udev.get()));
        
        if (!enumerate) {
            return devices;
        }

        udev_enumerate_add_match_subsystem(enumerate.get(), "usb");
        udev_enumerate_add_match_property(enumerate.get(), "DEVTYPE", "usb_device");
        udev_enumerate_scan_devices(enumerate.get());

        struct udev_list_entry* devices_list = udev_enumerate_get_list_entry(enumerate.get());
        struct udev_list_entry* entry;

        udev_list_entry_foreach(entry, devices_list) {
            const char* path = udev_list_entry_get_name(entry);
            std::unique_ptr<udev_device, udev_device_deleter> dev(
                udev_device_new_from_syspath(m_udev.get(), path));

            if (!dev) continue;

            auto vendor_id = getDeviceProperty(dev.get(), "ID_VENDOR_ID");
            auto product_id = getDeviceProperty(dev.get(), "ID_MODEL_ID");
            
            // Skip root hubs and generic hubs
            if (vendor_id == "1d6b" || product_id == "0000") {
                continue;
            }

            CategorizedDevice device;
            device.id = path;
            device.name = getDeviceName(dev.get());
            device.vendor = getVendorName(dev.get());
            device.subsystem = "usb";
            device.devpath = path;
            device.category = categorizeDevice(dev.get());
            device.pci_device = false;
            device.usb_device = true;

            devices.push_back(std::move(device));
        }
    }

    return devices;
}

} // namespace mcp::mhwd
