/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "DriverListModel.h"

namespace mcp::qt::mhwd {

DriverListModel::DriverListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int DriverListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(m_drivers.size());
}

QVariant DriverListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_drivers.size())) {
        return {};
    }

    const auto& driver = m_drivers[index.row()];

    switch (role) {
    case Id: return driver.id;
    case Name: return driver.name;
    case Version: return driver.version;
    case Info: return driver.info;
    case OpenSource: return driver.openSource;
    case Installed: return driver.installed;
    case Recommended: return driver.recommended;
    case Priority: return driver.priority;
    default: return {};
    }
}

QHash<int, QByteArray> DriverListModel::roleNames() const
{
    return {
        {Id, "id"},
        {Name, "name"},
        {Version, "version"},
        {Info, "info"},
        {OpenSource, "openSource"},
        {Installed, "installed"},
        {Recommended, "recommended"},
        {Priority, "priority"}
    };
}

void DriverListModel::setDrivers(const std::vector<DriverData>& drivers)
{
    beginResetModel();
    m_drivers = drivers;
    endResetModel();
    Q_EMIT driversChanged();
}

} // namespace mcp::qt::mhwd
