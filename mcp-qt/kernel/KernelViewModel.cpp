/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "KernelViewModel.h"
#include <QQmlEngine>
#include <QQuickItem>

namespace mcp::qt::kernel {

void KernelViewModel::init()
{
    // Connect transaction launcher signals
    connect(&m_transactionLauncher, &common::TransactionAgentLauncher::finished, this, 
        [this](bool success, int exitCode) {
            Q_UNUSED(exitCode);
            
            if (success) {
                // Refresh file database after successful transaction
                auto db_result = pamac::Database::instance();
                if (db_result) {
                    db_result->get().refresh_tmp_files_dbs();
                }
            }
            
            setCurrentTransactionKernelName(QString{});
            
            // Refresh kernel list after transaction (success or failure)
            // to show updated installation status
            fetchAndUpdateKernels();
        });

    fetchAndUpdateKernels();
}

KernelListModel *KernelViewModel::model() const
{
    return &m_model;
}

void KernelViewModel::installKernel(const QString &pkgName)
{
    setCurrentTransactionKernelName(pkgName);
    
    // Build package list: kernel + headers
    QStringList packages;
    packages << pkgName;
    packages << (pkgName + "-headers");
    
    // Launch transaction agent
    m_transactionLauncher.installPackages(packages);
}

void KernelViewModel::removeKernel(const QString &pkgName)
{
    setCurrentTransactionKernelName(pkgName);
    
    // Build package list: kernel + headers
    QStringList packages;
    packages << pkgName;
    packages << (pkgName + "-headers");
    
    // Launch transaction agent (force=false, don't remove running kernel)
    m_transactionLauncher.removePackages(packages, false);
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

QVariantMap KernelViewModel::inUseKernelData() const
{
    return m_inUseKernelData;
}

QVariantMap KernelViewModel::recommendedKernelData() const
{
    return m_recommendedKernelData;
}

void KernelViewModel::fetchAndUpdateKernels()
{
    // Fetch kernels with progress callback
    auto progress_callback = [this](int current, int total, const std::string& kernel_name) {
        Q_EMIT fetchProgress(current, total, QString::fromStdString(kernel_name));
    };
    
    auto result = m_provider.get_kernels(progress_callback);

    if (!result) {
        qWarning() << "Failed to fetch kernels:" << static_cast<int>(result.error());
        return;
    }

    auto kernels_copy = *result;
    
    // Queue the update to happen on the main thread
    QMetaObject::invokeMethod(
        this, [this, kernels_copy]() {
            // Extract in-use and recommended kernels
            QVariantMap newInUseData;
            QVariantMap newRecommendedData;
            
            for (const auto& kernel : kernels_copy) {
                QVariantMap kernelMap;
                kernelMap["name"] = QString::fromStdString(kernel.package_name);
                kernelMap["version"] = QString::fromStdString(kernel.version.to_string());
                kernelMap["isInUse"] = kernel.is_in_use();
                kernelMap["isRecommended"] = kernel.is_recommended();
                kernelMap["isInstalled"] = kernel.is_installed();
                kernelMap["majorVersion"] = kernel.version.major;
                kernelMap["minorVersion"] = kernel.version.minor;
                kernelMap["changelogUrl"] = QString::fromStdString(kernel.changelog_url);
                kernelMap["isLTS"] = kernel.is_lts();
                
                QStringList extraModsList;
                for (const auto& mod : kernel.extra_modules) {
                    extraModsList.append(QString::fromStdString(mod));
                }
                kernelMap["extraModules"] = extraModsList;
                
                if (kernel.is_in_use()) {
                    newInUseData = kernelMap;
                } else if (kernel.is_recommended()) {
                    newRecommendedData = kernelMap;
                }
            }

            // Update kernel data if changed
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

            // Update model (this will trigger UI updates for the main list)
            m_model.setKernels(kernels_copy);
        }, ::Qt::QueuedConnection);
}

} // namespace mcp::qt::kernel
