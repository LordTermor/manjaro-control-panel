/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "DeviceListModel.h"

#include <QVariantMap>
#include <ranges>

#include <qcontainerfwd.h>
#include <qhashfunctions.h>

namespace mcp::qt::mhwd {

DeviceListModel::DeviceListModel(QObject* parent)
    : QAbstractListModel(parent) {
}

int DeviceListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_categories.size());
}

QVariant DeviceListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_categories.size())) {
        return {};
    }

    const auto& category = m_categories[static_cast<size_t>(index.row())];

    switch (role) {
    case Name:
        return category.name;
    case Icon:
        return category.icon;
    case DeviceCount:
        return static_cast<int>(category.devices.size());
    case DriversAvailableCount: {
        int count = 0;
        for (const auto& device : category.devices) {
            if (device.hasDrivers) {
                count++;
            }
        }
        return count;
    }
    case Devices: {
        QVariantList deviceList;
        for (const auto& device : category.devices) {
            QVariantMap deviceMap;

            auto drivers = device.drivers | std::views::transform([](auto driver) -> QVariant {
                               return QVariant::fromValue(driver);
                           });
            deviceMap[QStringLiteral("id")] = device.id;
            deviceMap[QStringLiteral("name")] = device.name;
            deviceMap[QStringLiteral("vendor")] = device.vendor;
            deviceMap[QStringLiteral("classId")] = device.classId;
            deviceMap[QStringLiteral("vendorId")] = device.vendorId;
            deviceMap[QStringLiteral("deviceId")] = device.deviceId;
            deviceMap[QStringLiteral("busType")] = device.busType;
            deviceMap[QStringLiteral("icon")] = device.icon;
            deviceMap[QStringLiteral("hasDrivers")] = device.hasDrivers;
            deviceMap[QStringLiteral("drivers")] = QVariantList(drivers.begin(), drivers.end());
            deviceMap[QStringLiteral("driver")] = device.driver;

            deviceList.append(deviceMap);
        }
        return deviceList;
    }
    default:
        return {};
    }
}

QHash<int, QByteArray> DeviceListModel::roleNames() const {
    return {{Name, "name"},
            {Icon, "icon"},
            {DeviceCount, "deviceCount"},
            {DriversAvailableCount, "driversAvailableCount"},
            {Devices, "devices"}};
}

void DeviceListModel::setupCategories(const std::vector<CategoryData>& categories) {
    beginResetModel();
    m_categories = categories;
    endResetModel();
    Q_EMIT categoriesChanged();
}

} // namespace mcp::qt::mhwd
