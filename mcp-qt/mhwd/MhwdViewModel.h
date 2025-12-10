/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * MhwdViewModel - business logic for hardware driver management UI.
 * Manages device categorization and driver operations using QCoro.
 */

#pragma once

#include "DeviceListModel.h"
#include "DriverData.h"

#include <mhwd/ConfigProvider.hpp>
#include <mhwd/DeviceProvider.hpp>

#include <TransactionAgentLauncher.h>

#include <QCoroQmlTask>
#include <QCoroTask>

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

    Q_INVOKABLE QCoro::QmlTask refreshDevices();
    Q_INVOKABLE QCoro::QmlTask installDriver(const QString& driverId);
    Q_INVOKABLE QCoro::QmlTask removeDriver(const QString& driverId);

Q_SIGNALS:
    void errorOccurred(QString title, QString message);

private:
    QCoro::Task<void> init();
    QCoro::Task<void> populateCategories();
    QCoro::Task<QList<DriverData>> findDrivers(const mcp::mhwd::Device& device);
    QCoro::Task<DeviceData> createDeviceData(const mcp::mhwd::Device& device);
    
    DriverData createDriverData(const mcp::mhwd::Config& config, bool installed) const;
    QString determineCategoryForDevice(const mcp::mhwd::Device& device) const;
    QString determineIconForDevice(const mcp::mhwd::Device& device, const QString& category) const;

    std::unique_ptr<DeviceListModel> m_categoryModel;
    mcp::mhwd::DeviceProvider m_deviceProvider;
    std::unique_ptr<mcp::mhwd::ConfigProvider> m_configProvider;
    mcp::qt::common::TransactionAgentLauncher m_transactionLauncher;
};

} // namespace mcp::qt::mhwd
