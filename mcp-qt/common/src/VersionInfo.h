/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * VersionInfo - QML singleton exposing version information for all libraries.
 * Use in QML: import org.manjaro.mcp.components; VersionInfo.mcpVersion
 */

#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

namespace mcp::qt::common {

class VersionInfo : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString mcpVersion READ mcpVersion CONSTANT)
    Q_PROPERTY(QString qtVersion READ qtVersion CONSTANT)
    Q_PROPERTY(QString kf6Version READ kf6Version CONSTANT)
    Q_PROPERTY(QString kirigamiVersion READ kirigamiVersion CONSTANT)
    Q_PROPERTY(QString libpamacVersion READ libpamacVersion CONSTANT)

public:
    explicit VersionInfo(QObject* parent = nullptr);

    QString mcpVersion() const;
    QString qtVersion() const;
    QString kf6Version() const;
    QString kirigamiVersion() const;
    QString libpamacVersion() const;

private:
    QString m_libpamacVersion;
};

} // namespace mcp::qt::common
