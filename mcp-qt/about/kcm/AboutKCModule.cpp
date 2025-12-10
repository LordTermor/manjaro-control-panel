/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * AboutKCModule implementation - simple KCM showing MCP about information.
 */

#include "AboutKCModule.h"
#include "VersionInfo.h"

#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KCMAboutFactory, "kcm_mcp_about.json", registerPlugin<AboutKCModule>();)

AboutKCModule::AboutKCModule(QObject *parent,
                             const KPluginMetaData &data,
                             const QVariantList &)
    : KQuickConfigModule(parent, data)
{
    setButtons(NoAdditionalButton);

    qmlRegisterSingletonType<mcp::qt::common::VersionInfo>(
        "org.manjaro.mcp.components", 1, 0, "VersionInfo",
        [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new mcp::qt::common::VersionInfo();
        });
}

void AboutKCModule::load()
{
    KQuickConfigModule::load();
}

#include "AboutKCModule.moc"
