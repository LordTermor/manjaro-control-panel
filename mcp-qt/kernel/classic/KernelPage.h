/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include "KernelListDelegate.h"
#include "SelectedKernelsWidget.h"
#include "../KernelViewModel.h"

#include <QWidget>
#include <QListView>
#include <QLabel>
#include <QProgressBar>

/*
 * Main kernel manager page widget.
 * Combines SelectedKernelsWidget (top) with kernel list view (bottom).
 */

namespace mcp::qt::kernel {

class KernelPage : public QWidget
{
    Q_OBJECT

public:
    explicit KernelPage(KernelViewModel* viewModel, QWidget* parent = nullptr);
    ~KernelPage() override = default;

private Q_SLOTS:
    void onKernelsDataChanged();
    void onFetchProgress(int current, int total, const QString& kernelName);
    
    void onInstallClicked(const QModelIndex& index);
    void onRemoveClicked(const QModelIndex& index);
    void onChangelogClicked(const QModelIndex& index);
    
    void onCardInstallClicked(const QString& name, const QStringList& extraModules);
    void onCardRemoveClicked(const QString& name);
    void onCardChangelogClicked(const QString& changelogUrl);

private:
    void setupUi();
    void setupConnections();
    
    void confirmAndInstall(const QString& kernelName, const QStringList& extraModules);
    void confirmAndRemove(const QString& kernelName);
    void openChangelog(const QString& changelogUrl);

    KernelViewModel* m_viewModel;
    
    SelectedKernelsWidget* m_selectedKernels;
    QListView* m_kernelListView;
    KernelListDelegate* m_listDelegate;
};

} // namespace mcp::qt::kernel
