/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "MhwdViewModel.h"
#include "DeviceProvider.hpp"

namespace mcp::qt::mhwd {

MhwdViewModel::MhwdViewModel(QObject* parent)
    : QObject(parent)
    , m_categoryModel(std::make_unique<DeviceListModel>(this))
{
    populateCategories();
}

void MhwdViewModel::refreshDevices()
{
    populateCategories();
}

void MhwdViewModel::installDriver(const QString& deviceId, const QString& driverId)
{
    Q_EMIT driverOperationStarted(deviceId, driverId);
    
    // TODO: Implement actual driver installation via transaction agent
    // For now, just emit completion
    Q_EMIT driverOperationCompleted(deviceId, driverId, true);
}

void MhwdViewModel::removeDriver(const QString& deviceId, const QString& driverId)
{
    Q_EMIT driverOperationStarted(deviceId, driverId);
    
    // TODO: Implement actual driver removal via transaction agent
    Q_EMIT driverOperationCompleted(deviceId, driverId, true);
}

void MhwdViewModel::populateCategories()
{
    const std::vector<std::pair<QString, QString>> categoryIcons = {
        {QStringLiteral("Display"), QStringLiteral("video-display")},
        {QStringLiteral("Network"), QStringLiteral("network-wired")},
        {QStringLiteral("Audio"), QStringLiteral("audio-card")},
        {QStringLiteral("Storage"), QStringLiteral("drive-harddisk")},
        {QStringLiteral("Input"), QStringLiteral("input-mouse")},
        {QStringLiteral("Misc"), QStringLiteral("computer")}
    };
    
    ::mcp::mhwd::DeviceProvider provider;
    provider.scan();
    auto devices = provider.get_all_devices();
    
    std::vector<CategoryData> categories;
    for (const auto& [name, icon] : categoryIcons) {
        CategoryData catData;
        catData.name = name;
        catData.icon = icon;
        
        for (const auto& device : devices) {
            QString category = determineCategoryForDevice(device);
            if (category == name) {
                DeviceData data = createDeviceData(device);
                catData.devices.push_back(data);
            }
        }
        
        categories.push_back(std::move(catData));
    }
    
    m_categoryModel->setupCategories(categories);
}

DeviceData MhwdViewModel::createDeviceData(const mcp::mhwd::Device& device)
{
    DeviceData data;
    
    const auto& ids = device.hardware_ids();
    const auto& desc = device.description();
    const auto& sys = device.system_info();
    
    data.id = QString::fromStdString(sys.bus_id);
    data.name = QString::fromStdString(desc.model);
    data.vendor = QString::fromStdString(desc.vendor);
    data.classId = QString::fromStdString(ids.class_id);
    data.vendorId = QString::fromStdString(ids.vendor);
    data.deviceId = QString::fromStdString(ids.device);
    data.busType = device.bus_type() == mcp::mhwd::BusType::PCI ? QStringLiteral("PCI") : QStringLiteral("USB");
    
    QString category = determineCategoryForDevice(device);
    data.icon = determineIconForDevice(device, category);
    
    // TODO: Load matching configs
    data.hasDrivers = false;
    
    return data;
}

QString MhwdViewModel::determineCategoryForDevice(const mcp::mhwd::Device& device) const
{
    using mcp::mhwd::DeviceCategory;
    
    switch (device.category()) {
        case DeviceCategory::Graphics: return QStringLiteral("Display");
        case DeviceCategory::Network: return QStringLiteral("Network");
        case DeviceCategory::Audio: return QStringLiteral("Audio");
        case DeviceCategory::Storage: return QStringLiteral("Storage");
        case DeviceCategory::Input: return QStringLiteral("Input");
        case DeviceCategory::Unknown: return QStringLiteral("Misc");
    }
    return QStringLiteral("Misc");
}

QString MhwdViewModel::determineIconForDevice(const mcp::mhwd::Device& device, const QString& category) const
{
    if (category == QStringLiteral("Display")) return QStringLiteral("video-display");
    if (category == QStringLiteral("Network")) {
        return device.description().model.find("Wi-Fi") != std::string::npos 
            ? QStringLiteral("network-wireless")
            : QStringLiteral("network-wired");
    }
    if (category == QStringLiteral("Audio")) return QStringLiteral("audio-card");
    if (category == QStringLiteral("Storage")) {
        return device.description().model.find("NVMe") != std::string::npos
            ? QStringLiteral("drive-harddisk-solidstate")
            : QStringLiteral("drive-harddisk");
    }
    if (category == QStringLiteral("Input")) {
        const auto& name = device.description().model;
        if (name.find("Keyboard") != std::string::npos) return QStringLiteral("input-keyboard");
        if (name.find("Mouse") != std::string::npos) return QStringLiteral("input-mouse");
        return QStringLiteral("input-gaming");
    }
    
    return QStringLiteral("computer");
}

} // namespace mcp::qt::mhwd
