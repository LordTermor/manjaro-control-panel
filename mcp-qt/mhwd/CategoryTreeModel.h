/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * CategoryTreeModel - tree model for hardware categories and devices.
 * Structure: Category (parent) -> Devices (children) -> Drivers (exposed via DriversModel role)
 */

#pragma once

#include "DeviceListModel.h"

#include <QAbstractItemModel>
#include <QString>
#include <memory>
#include <vector>

namespace mcp::qt::mhwd {

class CategoryTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Role {
        // Common
        Name = Qt::UserRole + 1,
        Icon,
        Type, // "category" or "device"
        
        // Category-specific
        DeviceCount,
        DriversAvailableCount,
        
        // Device-specific
        Vendor,
        ClassId,
        VendorId,
        DeviceId,
        BusType,
        HasDrivers,
        DriversModel
    };
    Q_ENUM(Role)

    explicit CategoryTreeModel(QObject* parent = nullptr);

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addCategory(const QString& name, const QString& icon);
    void addDeviceToCategory(const QString& categoryName, const DeviceData& device);
    void clear();
    
    DeviceListModel* deviceModelForCategory(const QString& categoryName);
    DriverListModel* driverModelForDevice(const QString& categoryName, int deviceIndex);

Q_SIGNALS:
    void categoriesChanged();

private:
    struct Category {
        QString name;
        QString icon;
        std::unique_ptr<DeviceListModel> deviceModel;
    };

    std::vector<Category> m_categories;
    
    int findCategoryIndex(const QString& name) const;
};

} // namespace mcp::qt::mhwd
