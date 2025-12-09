/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include <QObject>
#include <QtQml>

#include "KernelData.h"

#include <kernel/KernelProvider.hpp>
#include <kernel/KernelManager.hpp>

#include <pamac/database.hpp>

#include <TransactionAgentLauncher.h>

#include "KernelListModel.h"

namespace mcp::qt::kernel {

class KernelViewModel : public QObject
{
    Q_OBJECT

    QML_ELEMENT

    Q_PROPERTY(KernelListModel *model READ model CONSTANT)
    Q_PROPERTY(mcp::qt::common::TransactionAgentLauncher *transactionLauncher READ transactionLauncher CONSTANT)
    Q_PROPERTY(QString currentTransactionKernelName READ currentTransactionKernelName WRITE
                   setCurrentTransactionKernelName NOTIFY currentTransactionKernelNameChanged)
    Q_PROPERTY(KernelData inUseKernelData READ inUseKernelData NOTIFY kernelsDataChanged)
    Q_PROPERTY(KernelData recommendedKernelData READ recommendedKernelData NOTIFY kernelsDataChanged)

public:
    explicit KernelViewModel(KernelListModel &model, QObject *parent = nullptr)
        : QObject(parent), m_model(model)
    {
        init();
    }

    ~KernelViewModel() = default;

    KernelListModel *model() const;

    Q_INVOKABLE void installKernel(const QString &pkgName);
    Q_INVOKABLE void removeKernel(const QString &pkgName);

    mcp::qt::common::TransactionAgentLauncher *transactionLauncher();

    QString currentTransactionKernelName() const;
    void setCurrentTransactionKernelName(const QString &newCurrentTransactionKernelName);

    KernelData inUseKernelData() const;
    KernelData recommendedKernelData() const;

Q_SIGNALS:
    void currentTransactionKernelNameChanged(QString currentTransactionKernelName);
    void kernelsDataChanged();
    
    void fetchProgress(int current, int total, QString kernelName);

private:
    void init();
    void fetchAndUpdateKernels();

    KernelListModel &m_model;
    mcp::kernel::KernelProvider m_provider;

    mcp::qt::common::TransactionAgentLauncher m_transactionLauncher;
    QString m_currentTransactionKernelName;
    KernelData m_inUseKernelData;
    KernelData m_recommendedKernelData;
};
} // namespace mcp::qt::kernel
