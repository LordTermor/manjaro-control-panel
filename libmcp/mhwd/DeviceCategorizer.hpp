#pragma once

#include <string>
#include <vector>
#include <libudev.h>
#include <memory>

namespace mcp::mhwd {

enum class DeviceCategory {
    Display,
    Network,
    Audio,
    Storage,
    Input,
    Misc
};

struct CategorizedDevice {
    std::string id;
    std::string name;
    std::string vendor;
    std::string subsystem;
    std::string devpath;
    DeviceCategory category;
    bool pci_device;
    bool usb_device;
};

class DeviceCategorizer {
public:
    DeviceCategorizer();
    ~DeviceCategorizer();

    std::vector<CategorizedDevice> enumerateDevices();

private:
    struct udev_deleter {
        void operator()(udev* u) const { udev_unref(u); }
    };
    
    struct udev_enumerate_deleter {
        void operator()(udev_enumerate* e) const { udev_enumerate_unref(e); }
    };
    
    struct udev_device_deleter {
        void operator()(udev_device* d) const { udev_device_unref(d); }
    };

    std::unique_ptr<udev, udev_deleter> m_udev;

    DeviceCategory categorizeDevice(udev_device* dev);
    std::string getDeviceProperty(udev_device* dev, const char* property);
    std::string getVendorName(udev_device* dev);
    std::string getDeviceName(udev_device* dev);
};

inline const char* categoryToString(DeviceCategory cat) {
    switch (cat) {
        case DeviceCategory::Display: return "Display";
        case DeviceCategory::Network: return "Network";
        case DeviceCategory::Audio: return "Audio";
        case DeviceCategory::Storage: return "Storage";
        case DeviceCategory::Input: return "Input";
        case DeviceCategory::Misc: return "Misc";
    }
    return "Misc";
}

} // namespace mcp::mhwd
