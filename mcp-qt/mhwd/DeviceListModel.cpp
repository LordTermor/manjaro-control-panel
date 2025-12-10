/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "DeviceListModel.h"

#include <QVariantMap>

namespace mcp::qt::mhwd {

DeviceListModel::DeviceListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int DeviceListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } 
    return static_cast<int>(m_categories.size());
}

QVariant DeviceListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_categories.size())) {
        return {};
    }

    const auto& category = m_categories[index.row()];

    switch (role) {
    case Name:
        return category.data.name;
    case Icon:
        return category.data.icon;
    case DeviceCount:
        return static_cast<int>(category.data.devices.size());
    case DriversAvailableCount: {
        int count = 0;
        for (const auto& device : category.data.devices) {
            if (device.hasDrivers) {
                count++;
            }
        }
        return count;
    }
    case Devices: {
        QVariantList deviceList;
        for (size_t i = 0; i < category.data.devices.size(); ++i) {
            const auto& device = category.data.devices[i];
            QVariantMap deviceMap;
            deviceMap[QStringLiteral("id")] = device.id;
            deviceMap[QStringLiteral("name")] = device.name;
            deviceMap[QStringLiteral("vendor")] = device.vendor;
            deviceMap[QStringLiteral("classId")] = device.classId;
            deviceMap[QStringLiteral("vendorId")] = device.vendorId;
            deviceMap[QStringLiteral("deviceId")] = device.deviceId;
            deviceMap[QStringLiteral("busType")] = device.busType;
            deviceMap[QStringLiteral("icon")] = device.icon;
            deviceMap[QStringLiteral("hasDrivers")] = device.hasDrivers;
            deviceMap[QStringLiteral("driversModel")] = QVariant::fromValue(category.driverModels[i].get());
            deviceList.append(deviceMap);
        }
        return deviceList;
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> DeviceListModel::roleNames() const
{
    return {
        {Name, "name"},
        {Icon, "icon"},
        {DeviceCount, "deviceCount"},
        {DriversAvailableCount, "driversAvailableCount"},
        {Devices, "devices"}
    };
}

void DeviceListModel::setupCategories(const std::vector<CategoryData>& categories)
{
    beginResetModel();
    m_categories.clear();
    m_categories.reserve(categories.size());
    
    for (const auto& catData : categories) {
        CategoryEntry entry;
        entry.data = catData;
        entry.driverModels.reserve(catData.devices.size());
        
        for (size_t i = 0; i < catData.devices.size(); ++i) {
            entry.driverModels.push_back(std::make_unique<DriverListModel>(this));
        }
        
        m_categories.push_back(std::move(entry));
    }
    
    endResetModel();
    Q_EMIT categoriesChanged();
}

DriverListModel* DeviceListModel::driverModelForDevice(int categoryIndex, int deviceIndex)
{
    if (categoryIndex < 0 || categoryIndex >= static_cast<int>(m_categories.size())) {
        return nullptr;
    }
    
    const auto& category = m_categories[categoryIndex];
    if (deviceIndex < 0 || deviceIndex >= static_cast<int>(category.driverModels.size())) {
        return nullptr;
    }
    
    return category.driverModels[deviceIndex].get();
}

} // namespace mcp::qt::mhwd
