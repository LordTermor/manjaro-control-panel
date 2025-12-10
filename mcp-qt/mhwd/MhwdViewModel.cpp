/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "MhwdViewModel.h"
#include "ConfigProvider.hpp"
#include "Device.hpp"
#include "DeviceProvider.hpp"
#include "Transaction.hpp"
#include <TransactionAgentLauncher.h>

#include <QCoroQmlTask>
#include <QCoroTask>

#include <qcontainerfwd.h>

namespace mcp::qt::mhwd {

MhwdViewModel::MhwdViewModel(QObject* parent)
    : QObject(parent)
    , m_categoryModel(std::make_unique<DeviceListModel>(this))
{
    QCoro::connect(init(), this, [](){});
}

QCoro::Task<void> MhwdViewModel::init()
{
    co_await m_deviceProvider.scan();
    m_configProvider = std::make_unique<mcp::mhwd::ConfigProvider>(m_deviceProvider);
    co_await populateCategories();
}

QCoro::QmlTask MhwdViewModel::refreshDevices()
{
    return populateCategories();
}

QCoro::Task<QList<DriverData>> MhwdViewModel::findDrivers(const ::mcp::mhwd::Device& device)
{
    QList<DriverData> result;
    
    const auto configs = co_await m_configProvider->find_matching_configs_for_device(device);
    
    auto installedConfigs = co_await m_configProvider->get_installed_configs(device.bus_type());
    std::vector<std::string> installedNames;
    if (installedConfigs) {
        for (const auto& cfg : *installedConfigs) {
            installedNames.push_back(cfg.name());
        }
    }
    
    for (const auto& config : configs) {
        bool installed = std::ranges::contains(installedNames, config.name());
        result.append(createDriverData(config, installed));
    }
    
    // Higher priority first
    std::ranges::sort(result, [](const DriverData& a, const DriverData& b) {
        return a.priority > b.priority;
    });
    
    co_return result;
}

QCoro::QmlTask MhwdViewModel::installDriver(const QString& driverId)
{
    return [](MhwdViewModel* self, QString driverId) -> QCoro::Task<bool> {
        auto result = co_await mcp::mhwd::build_install(*self->m_configProvider, driverId.toStdString(), ::mcp::mhwd::BusType::PCI);
        
        if (!result) {
            co_return false;
        }

        self->m_transactionLauncher.launchCommand(*result);
        
        co_return true;
    }(this, driverId);
}

QCoro::QmlTask MhwdViewModel::removeDriver(const QString& driverId)
{
    return [](MhwdViewModel* self, QString driverId) -> QCoro::Task<bool> {
        auto result = co_await mcp::mhwd::build_remove(*self->m_configProvider, driverId.toStdString(), ::mcp::mhwd::BusType::PCI);
        
        if (!result) {
            co_return false;
        }

        self->m_transactionLauncher.launchCommand(*result);
        
        co_return true;
    }(this, driverId);
}

QCoro::Task<void> MhwdViewModel::populateCategories()
{
    const std::vector<std::pair<QString, QString>> categoryIcons = {
        {QStringLiteral("Display"), QStringLiteral("video-display")},
        {QStringLiteral("Network"), QStringLiteral("network-wired")},
        {QStringLiteral("Audio"), QStringLiteral("audio-card")},
        {QStringLiteral("Storage"), QStringLiteral("drive-harddisk")},
        {QStringLiteral("Input"), QStringLiteral("input-mouse")},
        {QStringLiteral("Misc"), QStringLiteral("computer")}
    };
    
    co_await m_deviceProvider.scan();
    auto devices = m_deviceProvider.all_devices();
    
    std::vector<CategoryData> categories;
    for (const auto& [name, icon] : categoryIcons) {
        CategoryData catData;
        catData.name = name;
        catData.icon = icon;
        
        for (const auto& device : devices) {
            QString category = determineCategoryForDevice(device);
            if (category == name) {
                DeviceData data = co_await createDeviceData(device);
                catData.devices.push_back(data);
            }
        }
        
        categories.push_back(std::move(catData));
    }
    
    m_categoryModel->setupCategories(categories);
}

QCoro::Task<DeviceData> MhwdViewModel::createDeviceData(const mcp::mhwd::Device& device)
{
    DeviceData data;
    
    data.id = QString::fromStdString(device.bus_id());
    data.name = QString::fromStdString(device.device_name());
    data.vendor = QString::fromStdString(device.vendor_name());
    data.classId = QString::fromStdString(device.class_id());
    data.vendorId = QString::fromStdString(device.vendor_id());
    data.deviceId = QString::fromStdString(device.device_id());
    data.busType = device.bus_type() == mcp::mhwd::BusType::PCI ? QStringLiteral("PCI") : QStringLiteral("USB");
    data.driver = QString::fromStdString(device.driver());
    
    QString category = determineCategoryForDevice(device);
    data.icon = determineIconForDevice(device, category);

    auto drivers = co_await findDrivers(device);
    data.drivers = drivers;

    auto configs = co_await m_configProvider->find_matching_configs_for_device(device);
    data.hasDrivers = !configs.empty();
    
    co_return data;
}

DriverData MhwdViewModel::createDriverData(const mcp::mhwd::Config& config, bool installed) const
{
    DriverData data;
    
    data.id = QString::fromStdString(config.name());
    data.name = QString::fromStdString(config.name());
    data.version = QString::fromStdString(config.version());
    data.info = QString::fromStdString(config.description());
    data.openSource = config.is_free_driver();
    data.installed = installed;
    data.recommended = config.priority() >= 50;
    data.priority = config.priority();
    
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
        return device.device_name().find("Wi-Fi") != std::string::npos 
            ? QStringLiteral("network-wireless")
            : QStringLiteral("network-wired");
    }
    if (category == QStringLiteral("Audio")) return QStringLiteral("audio-card");
    if (category == QStringLiteral("Storage")) {
        return device.device_name().find("NVMe") != std::string::npos
            ? QStringLiteral("drive-harddisk-solidstate")
            : QStringLiteral("drive-harddisk");
    }
    if (category == QStringLiteral("Input")) {
        const auto& name = device.device_name();
        if (name.find("Keyboard") != std::string::npos) return QStringLiteral("input-keyboard");
        if (name.find("Mouse") != std::string::npos) return QStringLiteral("input-mouse");
        return QStringLiteral("input-gaming");
    }
    
    return QStringLiteral("computer");
}

} // namespace mcp::qt::mhwd
