/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "CategoryTreeModel.h"

namespace mcp::qt::mhwd {

CategoryTreeModel::CategoryTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex CategoryTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid()) {
        // Top-level: categories
        if (row < static_cast<int>(m_categories.size())) {
            return createIndex(row, column, quintptr(row) << 32);
        }
    } else {
        // Child: devices
        quintptr categoryIdx = parent.internalId() >> 32;
        if (categoryIdx < m_categories.size()) {
            const auto& category = m_categories[categoryIdx];
            if (row < category.deviceModel->rowCount()) {
                return createIndex(row, column, (categoryIdx << 32) | (quintptr(row) + 1));
            }
        }
    }

    return {};
}

QModelIndex CategoryTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid()) {
        return {};
    }

    quintptr id = child.internalId();
    quintptr categoryIdx = id >> 32;
    quintptr deviceIdx = id & 0xFFFFFFFF;

    if (deviceIdx == 0) {
        // This is a category (top-level)
        return {};
    }

    // This is a device, return its category parent
    return createIndex(static_cast<int>(categoryIdx), 0, categoryIdx << 32);
}

int CategoryTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        // Top-level: categories
        return static_cast<int>(m_categories.size());
    }

    quintptr id = parent.internalId();
    quintptr categoryIdx = id >> 32;
    quintptr deviceIdx = id & 0xFFFFFFFF;

    if (deviceIdx == 0 && categoryIdx < m_categories.size()) {
        // Category node: return device count
        return m_categories[categoryIdx].deviceModel->rowCount();
    }

    // Device node: no children
    return 0;
}

int CategoryTreeModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant CategoryTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    quintptr id = index.internalId();
    quintptr categoryIdx = id >> 32;
    quintptr deviceIdx = id & 0xFFFFFFFF;

    if (categoryIdx >= m_categories.size()) {
        return {};
    }

    const auto& category = m_categories[categoryIdx];

    if (deviceIdx == 0) {
        // Category node
        switch (role) {
        case Name: return category.name;
        case Icon: return category.icon;
        case Type: return QStringLiteral("category");
        case DeviceCount: return category.deviceModel->rowCount();
        case DriversAvailableCount: {
            int count = 0;
            for (int i = 0; i < category.deviceModel->rowCount(); ++i) {
                auto deviceIndex = category.deviceModel->index(i);
                if (category.deviceModel->data(deviceIndex, DeviceListModel::HasDrivers).toBool()) {
                    count++;
                }
            }
            return count;
        }
        default: return {};
        }
    } else {
        // Device node
        int deviceRow = static_cast<int>(deviceIdx - 1);
        auto deviceIndex = category.deviceModel->index(deviceRow);
        
        switch (role) {
        case Name: return category.deviceModel->data(deviceIndex, DeviceListModel::Name);
        case Vendor: return category.deviceModel->data(deviceIndex, DeviceListModel::Vendor);
        case Icon: return category.deviceModel->data(deviceIndex, DeviceListModel::Icon);
        case ClassId: return category.deviceModel->data(deviceIndex, DeviceListModel::ClassId);
        case VendorId: return category.deviceModel->data(deviceIndex, DeviceListModel::VendorId);
        case DeviceId: return category.deviceModel->data(deviceIndex, DeviceListModel::DeviceId);
        case BusType: return category.deviceModel->data(deviceIndex, DeviceListModel::BusType);
        case HasDrivers: return category.deviceModel->data(deviceIndex, DeviceListModel::HasDrivers);
        case DriversModel: return category.deviceModel->data(deviceIndex, DeviceListModel::DriversModel);
        case Type: return QStringLiteral("device");
        default: return {};
        }
    }
}

QHash<int, QByteArray> CategoryTreeModel::roleNames() const
{
    return {
        {Name, "name"},
        {Icon, "icon"},
        {Type, "type"},
        {DeviceCount, "deviceCount"},
        {DriversAvailableCount, "driversAvailableCount"},
        {Vendor, "vendor"},
        {ClassId, "classId"},
        {VendorId, "vendorId"},
        {DeviceId, "deviceId"},
        {BusType, "busType"},
        {HasDrivers, "hasDrivers"},
        {DriversModel, "driversModel"}
    };
}

void CategoryTreeModel::addCategory(const QString& name, const QString& icon)
{
    int existingIdx = findCategoryIndex(name);
    if (existingIdx >= 0) {
        return; // Category already exists
    }

    int row = static_cast<int>(m_categories.size());
    beginInsertRows(QModelIndex(), row, row);
    
    Category cat;
    cat.name = name;
    cat.icon = icon;
    cat.deviceModel = std::make_unique<DeviceListModel>(this);
    m_categories.push_back(std::move(cat));
    
    endInsertRows();
    Q_EMIT categoriesChanged();
}

void CategoryTreeModel::addDeviceToCategory(const QString& categoryName, const DeviceData& device)
{
    int catIdx = findCategoryIndex(categoryName);
    if (catIdx < 0) {
        return; // Category doesn't exist
    }

    auto& category = m_categories[catIdx];
    QModelIndex categoryIndex = createIndex(catIdx, 0, quintptr(catIdx) << 32);
    
    int deviceRow = category.deviceModel->rowCount();
    beginInsertRows(categoryIndex, deviceRow, deviceRow);
    
    std::vector<DeviceData> devices = category.deviceModel->devices();
    devices.push_back(device);
    category.deviceModel->setDevices(devices);
    
    endInsertRows();
}

void CategoryTreeModel::clear()
{
    beginResetModel();
    m_categories.clear();
    endResetModel();
    Q_EMIT categoriesChanged();
}

DeviceListModel* CategoryTreeModel::deviceModelForCategory(const QString& categoryName)
{
    int idx = findCategoryIndex(categoryName);
    if (idx < 0) {
        return nullptr;
    }
    return m_categories[idx].deviceModel.get();
}

DriverListModel* CategoryTreeModel::driverModelForDevice(const QString& categoryName, int deviceIndex)
{
    auto* deviceModel = deviceModelForCategory(categoryName);
    if (!deviceModel) {
        return nullptr;
    }
    return deviceModel->driverModelForDevice(deviceIndex);
}

int CategoryTreeModel::findCategoryIndex(const QString& name) const
{
    for (size_t i = 0; i < m_categories.size(); ++i) {
        if (m_categories[i].name == name) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

} // namespace mcp::qt::mhwd
