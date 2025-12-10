/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include <KQuickConfigModule>

#include "../MhwdViewModel.h"

namespace {
using namespace mcp::qt::mhwd;
}
class MhwdKCModule : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(MhwdViewModel *vm READ vm WRITE setVm NOTIFY vmChanged)

public:
    MhwdKCModule(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    virtual ~MhwdKCModule() override = default;

    MhwdViewModel *vm() const;
    void setVm(MhwdViewModel *newVm);

public Q_SLOTS:
    virtual void load() override;

Q_SIGNALS:
    void vmChanged();

private:
    std::unique_ptr<MhwdViewModel> m_viewModel;
    MhwdViewModel *m_vm = nullptr;
};
