/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * MhwdViewModel - business logic for hardware driver management UI.
 * Manages device categorization and driver operations.
 */

#pragma once

#include "DeviceListModel.h"

#include <mhwd/DeviceProvider.hpp>

#include <QObject>
#include <QtQml>

namespace mcp::qt::mhwd {

class MhwdViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(DeviceListModel* categoryModel READ categoryModel CONSTANT)

public:
    explicit MhwdViewModel(QObject* parent = nullptr);

    DeviceListModel* categoryModel() const { return m_categoryModel.get(); }

    Q_INVOKABLE void refreshDevices();
    Q_INVOKABLE void installDriver(const QString& deviceId, const QString& driverId);
    Q_INVOKABLE void removeDriver(const QString& deviceId, const QString& driverId);

Q_SIGNALS:
    void driverOperationStarted(QString deviceId, QString driverId);
    void driverOperationCompleted(QString deviceId, QString driverId, bool success);
    void errorOccurred(QString title, QString message);

private:
    void populateCategories();
    DeviceData createDeviceData(const mcp::mhwd::Device& device);
    QString determineCategoryForDevice(const mcp::mhwd::Device& device) const;
    QString determineIconForDevice(const mcp::mhwd::Device& device, const QString& category) const;

    std::unique_ptr<DeviceListModel> m_categoryModel;
};

} // namespace mcp::qt::mhwd
