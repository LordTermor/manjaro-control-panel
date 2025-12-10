/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * AboutKCModule - KDE System Settings module for MCP about information.
 */

#pragma once

#include <KQuickConfigModule>

class AboutKCModule : public KQuickConfigModule
{
    Q_OBJECT

public:
    AboutKCModule(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    ~AboutKCModule() override = default;

public Q_SLOTS:
    void load() override;
};
