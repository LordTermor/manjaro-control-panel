/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-FileCopyrightText: 2025 Daniil Ludwig <eightbyte81@gmail.com>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include "KernelItemWidget.h"
#include "../KernelViewModel.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

/*
 * Main kernel manager page widget.
 * Top section: Current/Recommended kernel cards.
 * Bottom section: Scrollable list of all kernels.
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
    void populateKernelList();
    void onInstallClicked(const KernelData& kernelData);
    void onRemoveClicked(const KernelData& kernelData);
    void onChangelogClicked(const QString& changelogUrl);

private:
    void setupUi();
    void setupConnections();
    
    void confirmAndInstall(const KernelData& kernelData);
    void confirmAndRemove(const KernelData& kernelData);

    KernelViewModel* m_viewModel;
    
    // Top section - cards
    QLabel* m_inUseLabel;
    KernelItemWidget* m_inUseCard;
    QLabel* m_recommendedLabel;
    KernelItemWidget* m_recommendedCard;
    
    // Kernel list
    QVBoxLayout* m_kernelListLayout;
    QList<KernelItemWidget*> m_listItems;
    QList<QLabel*> m_sectionHeaders;
    QList<QFrame*> m_sectionSeparators;

    QHBoxLayout* m_cardsLayout;
};

} // namespace mcp::qt::kernel
