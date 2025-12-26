/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * DriverData - Q_GADGET for individual driver/config info.
 * Value type exposing mcp::mhwd::Config data to QML.
 */

#pragma once

#include <QMetaType>
#include <QString>

namespace mcp::qt::mhwd {

class DriverData
{
    Q_GADGET

    Q_PROPERTY(QString id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(QString info MEMBER info)
    Q_PROPERTY(bool openSource MEMBER openSource)
    Q_PROPERTY(bool installed MEMBER installed)
    Q_PROPERTY(bool recommended MEMBER recommended)
    Q_PROPERTY(int priority MEMBER priority)
    Q_PROPERTY(bool valid READ isValid)

public:
    QString id;
    QString name;
    QString version;
    QString info;
    bool openSource = true;
    bool installed = false;
    bool recommended = false;
    int priority = 0;

    bool isValid() const { return !id.isEmpty() && !name.isEmpty(); }

    bool operator==(const DriverData& other) const = default;
};

} // namespace mcp::qt::mhwd

Q_DECLARE_METATYPE(mcp::qt::mhwd::DriverData)
