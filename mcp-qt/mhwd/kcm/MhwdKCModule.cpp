/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "MhwdKCModule.h"
#include "../MhwdViewModel.h"
#include "VersionInfo.h"

#include <KAboutData>
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(KCMMhwdFactory, "kcm_mcp_mhwd.json", registerPlugin<MhwdKCModule>();)

MhwdKCModule::MhwdKCModule(QObject *parent,
                           const KPluginMetaData &data,
                           const QVariantList &args)
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
   
    m_viewModel = std::make_unique<MhwdViewModel>();
    setVm(m_viewModel.get());
}

void MhwdKCModule::load()
{
    KQuickConfigModule::load();
}

MhwdViewModel *MhwdKCModule::vm() const
{
    return m_vm;
}

void MhwdKCModule::setVm(MhwdViewModel *newVm)
{
    if (m_vm == newVm)
        return;
    m_vm = newVm;
    Q_EMIT vmChanged();
}

#include "MhwdKCModule.moc"
