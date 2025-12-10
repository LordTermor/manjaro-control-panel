/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DeviceData - Q_GADGET for device information.
 * Value type exposing mcp::mhwd::Device data to QML.
 */

#pragma once

#include <QMetaType>
#include <QString>
#include <QList>
#include "DriverData.h"

namespace mcp::qt::mhwd {

class DeviceData
{
    Q_GADGET

    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString vendor MEMBER vendor)
    Q_PROPERTY(QString classId MEMBER classId)
    Q_PROPERTY(QString vendorId MEMBER vendorId)
    Q_PROPERTY(QString deviceId MEMBER deviceId)
    Q_PROPERTY(QString busType MEMBER busType)
    Q_PROPERTY(QString icon MEMBER icon)
    Q_PROPERTY(bool hasDrivers MEMBER hasDrivers)
    Q_PROPERTY(bool valid READ isValid)
    Q_PROPERTY(QList<DriverData> drivers MEMBER drivers)
    Q_PROPERTY(QString driver MEMBER driver)

public:
    QString id;
    QString name;
    QString vendor;
    QString classId;
    QString vendorId;
    QString deviceId;
    QString busType;
    QString icon;
    bool hasDrivers = false;
    QList<DriverData> drivers;
    QString driver;

    bool isValid() const { return !id.isEmpty() && !name.isEmpty(); }

    bool operator==(const DeviceData& other) const = default;
};

} // namespace mcp::qt::mhwd

Q_DECLARE_METATYPE(mcp::qt::mhwd::DeviceData)
