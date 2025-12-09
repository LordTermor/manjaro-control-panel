/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "VersionInfo.h"

#include <pamac/version.hpp>

namespace mcp::qt::common {

VersionInfo::VersionInfo(QObject* parent)
    : QObject(parent)
{
    // Cache libpamac version at construction (requires GLib call)
    m_libpamacVersion = QString::fromStdString(pamac::get_version());
}

QString VersionInfo::mcpVersion() const
{
    return QStringLiteral(MCP_VERSION);
}

QString VersionInfo::qtVersion() const
{
    return QString::fromLatin1(qVersion());
}

QString VersionInfo::kf6Version() const
{
    return QStringLiteral("N/A");
}

QString VersionInfo::kirigamiVersion() const
{
    return QStringLiteral("N/A");
}

QString VersionInfo::libpamacVersion() const
{
    return m_libpamacVersion;
}

} // namespace mcp::qt::common
