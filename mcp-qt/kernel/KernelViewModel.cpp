/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
/*
 * KernelViewModel - business logic for kernel management UI.
 */

#include "KernelViewModel.h"

#include <qcoro/task.h>
#include "pamac/transaction.hpp"

#include <QQmlEngine>
#include <QQuickItem>

namespace mcp::qt::kernel {

void KernelViewModel::init()
{
    connect(&m_transactionLauncher, &common::TransactionAgentLauncher::finished, this, 
        [this](bool success, [[maybe_unused]] int exitCode) {
            
            if (success) {
                // Refresh package database after kernel transaction
                []() -> QCoro::Task<void> {
                    auto db_result = pamac::Database::instance();
                    if (!db_result) {
                        qWarning() << "Failed to get database instance for refresh";
                        co_return;
                    }
                    
                    auto refresh_txn = pamac::Transaction(db_result.value().get());

                    bool refresh_success = co_await refresh_txn.refresh_dbs_async();
                    if (!refresh_success) {
                        qWarning() << "Failed to refresh databases after kernel transaction";
                        co_return;
                    }
                    
                    co_await refresh_txn.run_async();
                }();
            }
            
            setCurrentTransactionKernelName(QString{});
            fetchAndUpdateKernels();
        });

    fetchAndUpdateKernels();
}

KernelListModel *KernelViewModel::model() const
{
    return &m_model;
}

void KernelViewModel::installKernel(const KernelData &kernelData)
{
    QString kernelName = kernelData.name;
    setCurrentTransactionKernelName(kernelName);
    
    [this, kernelName]() -> QCoro::Task<void> {
        auto cmd_result = co_await mcp::kernel::KernelTransactionBuilder::install(
            kernelName.toStdString(),
            true,  // with_headers
            true   // with_extra_modules
        );
        
        if (!cmd_result) {
            setCurrentTransactionKernelName(QString{});
            
            using mcp::kernel::TransactionError;
            switch (cmd_result.error()) {
                case TransactionError::UpdatesPending:
                    Q_EMIT updatesPendingError();
                    break;
                    
                case TransactionError::KernelNotFound:
                    Q_EMIT transactionError(
                        tr("Kernel Not Found"),
                        tr("The kernel package '%1' could not be found in repositories.").arg(kernelName)
                    );
                    break;
                    
                default:
                    Q_EMIT transactionError(
                        tr("Installation Error"),
                        tr("Failed to prepare kernel installation. Error code: %1\n\n"
                           "This is unexpected. Please report this to developers.")
                           .arg(static_cast<int>(cmd_result.error()))
                    );
                    break;
            }
            co_return;
        }
        
        m_transactionLauncher.launchCommand(*cmd_result);
    }();
}

void KernelViewModel::removeKernel(const KernelData &kernelData)
{
    QString kernelName = kernelData.name;
    setCurrentTransactionKernelName(kernelName);
    
    [this, kernelName]() -> QCoro::Task<void> {
        auto cmd_result = co_await mcp::kernel::KernelTransactionBuilder::remove(
            kernelName.toStdString(),
            true,   // with_headers
            true,   // with_extra_modules
            false   // force
        );
        
        if (!cmd_result) {
            setCurrentTransactionKernelName(QString{});
            
            using mcp::kernel::TransactionError;
            switch (cmd_result.error()) {
                case TransactionError::KernelInUse:
                    Q_EMIT transactionError(
                        tr("Kernel In Use"),
                        tr("Cannot remove kernel '%1' because it is currently running.\n\n"
                           "Please boot into a different kernel first.").arg(kernelName)
                    );
                    break;
                    
                case TransactionError::KernelNotFound:
                    Q_EMIT transactionError(
                        tr("Kernel Not Found"),
                        tr("The kernel package '%1' is not installed.").arg(kernelName)
                    );
                    break;
                    
                default:
                    Q_EMIT transactionError(
                        tr("Removal Error"),
                        tr("Failed to prepare kernel removal. Error code: %1\n\n"
                           "This is unexpected. Please report this to developers.")
                           .arg(static_cast<int>(cmd_result.error()))
                    );
                    break;
            }
            co_return;
        }
        
        m_transactionLauncher.launchCommand(*cmd_result);
    }();
}

common::TransactionAgentLauncher *KernelViewModel::transactionLauncher()
{
    return &m_transactionLauncher;
}

QString KernelViewModel::currentTransactionKernelName() const
{
    return m_currentTransactionKernelName;
}

void KernelViewModel::setCurrentTransactionKernelName(const QString &newCurrentTransactionKernelName)
{
    if (m_currentTransactionKernelName == newCurrentTransactionKernelName)
        return;
    m_currentTransactionKernelName = newCurrentTransactionKernelName;
    Q_EMIT currentTransactionKernelNameChanged(m_currentTransactionKernelName);
}

KernelData KernelViewModel::inUseKernelData() const
{
    return m_inUseKernelData;
}

KernelData KernelViewModel::recommendedKernelData() const
{
    return m_recommendedKernelData;
}

void KernelViewModel::fetchAndUpdateKernels()
{
    // Launch async kernel fetching with QCoro
    [this]() -> QCoro::Task<void> {
        auto result = co_await m_provider.get_kernels_async();

        if (!result) {
            qWarning() << "Failed to fetch kernels:" << static_cast<int>(result.error());
            co_return;
        }

        auto kernels_copy = *result;
        
        KernelData newInUseData;
        KernelData newRecommendedData;
        
        for (const auto& kernel : kernels_copy) {
            KernelData kernelData;
            kernelData.name = QString::fromStdString(kernel.package_name);
            kernelData.version = QString::fromStdString(kernel.version.to_string());
            kernelData.isInUse = kernel.is_in_use();
            kernelData.isRecommended = kernel.is_recommended();
            kernelData.isInstalled = kernel.is_installed();
            kernelData.majorVersion = kernel.version.major;
            kernelData.minorVersion = kernel.version.minor;
            kernelData.changelogUrl = QString::fromStdString(kernel.changelog_url);
            kernelData.isLTS = kernel.is_lts();
            
            QStringList extraModsList;
            for (const auto& mod : kernel.extra_modules) {
                extraModsList.append(QString::fromStdString(mod));
            }
            kernelData.extraModules = extraModsList;
            
            if (kernel.is_in_use()) {
                newInUseData = kernelData;
            } else if (kernel.is_recommended()) {
                newRecommendedData = kernelData;
            }
        }

        bool changed = false;
        if (m_inUseKernelData != newInUseData) {
            m_inUseKernelData = newInUseData;
            changed = true;
        }
        if (m_recommendedKernelData != newRecommendedData) {
            m_recommendedKernelData = newRecommendedData;
            changed = true;
        }
        
        if (changed) {
            Q_EMIT kernelsDataChanged();
        }

        m_model.setKernels(kernels_copy);
    }();
}

} // namespace mcp::qt::kernel
