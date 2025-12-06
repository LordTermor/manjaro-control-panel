/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include <KQuickConfigModule>

#include "../KernelViewModel.h"

namespace {
using namespace mcp::qt::kernel;
}
class KernelKCModule : public KQuickConfigModule
{
    Q_OBJECT

    Q_PROPERTY(KernelViewModel *vm READ vm WRITE setVm NOTIFY vmChanged)

public:
    KernelKCModule(QObject *parent, const KPluginMetaData &data, const QVariantList &args);
    virtual ~KernelKCModule() override = default;

    KernelViewModel *vm() const;
    void setVm(KernelViewModel *newVm);

public Q_SLOTS:
    virtual void load() override;

Q_SIGNALS:
    void vmChanged();

private:
    std::unique_ptr<KernelListModel> m_model;
    std::unique_ptr<KernelViewModel> m_viewModel;
    KernelViewModel *m_vm = nullptr;
};
