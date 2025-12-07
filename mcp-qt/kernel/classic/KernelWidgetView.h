/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

/*
 * Classic Qt Widgets-based Kernel Manager.
 * Provides a traditional desktop UI suitable for lightweight DEs like XFCE/LXQt.
 * Uses plain widgets: QTableWidget, QPushButton, QLabel - no custom painting.
 */

#include "../KernelListModel.h"
#include "../KernelViewModel.h"

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>

namespace mcp::qt::kernel {

class KernelWidgetView : public QWidget
{
    Q_OBJECT

public:
    explicit KernelWidgetView(KernelViewModel* viewModel, QWidget* parent = nullptr);
    ~KernelWidgetView() override = default;

private Q_SLOTS:
    void onKernelsDataChanged();
    void onFetchProgress(int current, int total, const QString& kernelName);
    void onInstallClicked();
    void onRemoveClicked();
    void refreshKernelTable();

private:
    void setupUi();
    void setupConnections();
    
    void populateKernelTable();
    
    void confirmAndInstall(const QString& kernelName, const QStringList& extraModules);
    void confirmAndRemove(const QString& kernelName);

    KernelViewModel* m_viewModel;
    
    // Top section: Current and Recommended kernel cards
    QGroupBox* m_inUseCard;
    QLabel* m_inUseName;
    QLabel* m_inUseVersionAndChangelog;
    QPushButton* m_inUseActionBtn;
    
    QGroupBox* m_recommendedCard;
    QLabel* m_recommendedName;
    QLabel* m_recommendedVersionAndChangelog;
    QPushButton* m_recommendedActionBtn;
    
    // Main kernel list
    QTableWidget* m_kernelTable;
    
    // Status bar area
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
};

} // namespace mcp::qt::kernel
