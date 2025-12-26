/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DriverListModel - list of drivers for a single device.
 * Used as nested model within DeviceListModel.
 */

#pragma once

#include "DriverData.h"

#include <QAbstractListModel>

namespace mcp::qt::mhwd {

class DriverListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Role {
        Id = Qt::UserRole + 1,
        Name,
        Version,
        Info,
        OpenSource,
        Installed,
        Recommended,
        Priority
    };
    Q_ENUM(Role)

    explicit DriverListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setDrivers(const std::vector<DriverData>& drivers);
    const std::vector<DriverData>& drivers() const { return m_drivers; }

Q_SIGNALS:
    void driversChanged();

private:
    std::vector<DriverData> m_drivers;
};

} // namespace mcp::qt::mhwd
