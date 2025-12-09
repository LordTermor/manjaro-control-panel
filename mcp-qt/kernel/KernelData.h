/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQml>

namespace mcp::qt::kernel {

/*
 * Value type representing kernel metadata for UI display.
 * Used by KernelViewModel to expose in-use and recommended kernel info.
 */
class KernelData
{
    Q_GADGET

    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString version MEMBER version)
    Q_PROPERTY(int majorVersion MEMBER majorVersion)
    Q_PROPERTY(int minorVersion MEMBER minorVersion)
    Q_PROPERTY(QString changelogUrl MEMBER changelogUrl)
    Q_PROPERTY(QStringList extraModules MEMBER extraModules)
    Q_PROPERTY(bool isInUse MEMBER isInUse)
    Q_PROPERTY(bool isInstalled MEMBER isInstalled)
    Q_PROPERTY(bool isRecommended MEMBER isRecommended)
    Q_PROPERTY(bool isLTS MEMBER isLTS)
    Q_PROPERTY(bool valid READ isValid)

public:
    QString name;
    QString version;
    int majorVersion = 0;
    int minorVersion = 0;
    QString changelogUrl;
    QStringList extraModules;
    bool isInUse = false;
    bool isInstalled = false;
    bool isRecommended = false;
    bool isLTS = false;

    bool isValid() const { return !name.isEmpty(); }

    bool operator==(const KernelData& other) const = default;
};

} // namespace mcp::qt::kernel

Q_DECLARE_METATYPE(mcp::qt::kernel::KernelData)
