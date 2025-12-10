/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DeviceListModel - 2D array model for categories and devices.
 * Structure: Category rows with device arrays. All-or-nothing setup via setupCategories.
 */

#pragma once

#include "DeviceData.h"

#include <QAbstractListModel>
#include <QString>
#include <QVariantList>
#include <vector>

namespace mcp::qt::mhwd {

struct CategoryData {
    QString name;
    QString icon;
    std::vector<DeviceData> devices;
};

class DeviceListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        // Category properties
        Name = Qt::UserRole + 1,
        Icon,
        DeviceCount,
        DriversAvailableCount,
        Devices // QVariantList of device objects
    };
    Q_ENUM(Role)

    explicit DeviceListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setupCategories(const std::vector<CategoryData>& categories);

Q_SIGNALS:
    void categoriesChanged();

private:
    std::vector<CategoryData> m_categories;
};

} // namespace mcp::qt::mhwd
