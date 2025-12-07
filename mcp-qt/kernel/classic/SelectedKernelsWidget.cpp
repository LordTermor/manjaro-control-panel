/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "SelectedKernelsWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

/*
 * Container for Current and Recommended kernel cards.
 * Lays out two KernelCardWidget instances horizontally and forwards their signals.
 */

namespace mcp::qt::kernel {

SelectedKernelsWidget::SelectedKernelsWidget(QWidget* parent)
    : QWidget(parent)
    , m_inUseCard(new KernelCardWidget(this))
    , m_recommendedCard(new KernelCardWidget(this))
    , m_inUseLabel(new QLabel(tr("Currently used"), this))
    , m_recommendedLabel(new QLabel(tr("Recommended"), this))
{
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(12);
    
    auto* inUseLayout = new QVBoxLayout();
    inUseLayout->setSpacing(4);
    inUseLayout->addWidget(m_inUseLabel);
    inUseLayout->addWidget(m_inUseCard, 1);
    
    auto* recommendedLayout = new QVBoxLayout();
    recommendedLayout->setSpacing(4);
    recommendedLayout->addWidget(m_recommendedLabel);
    recommendedLayout->addWidget(m_recommendedCard, 1);
    
    mainLayout->addLayout(inUseLayout, 1);
    mainLayout->addLayout(recommendedLayout, 1);
    
    // Forward signals
    connect(m_inUseCard, &KernelCardWidget::installClicked,
            this, &SelectedKernelsWidget::installClicked);
    connect(m_inUseCard, &KernelCardWidget::removeClicked,
            this, &SelectedKernelsWidget::removeClicked);
    connect(m_inUseCard, &KernelCardWidget::changelogClicked,
            this, &SelectedKernelsWidget::changelogClicked);
    
    connect(m_recommendedCard, &KernelCardWidget::installClicked,
            this, &SelectedKernelsWidget::installClicked);
    connect(m_recommendedCard, &KernelCardWidget::removeClicked,
            this, &SelectedKernelsWidget::removeClicked);
    connect(m_recommendedCard, &KernelCardWidget::changelogClicked,
            this, &SelectedKernelsWidget::changelogClicked);
}

void SelectedKernelsWidget::setInUseKernel(const QVariantMap& data)
{
    m_inUseCard->setKernelData(data);
    bool hasData = !data.isEmpty();
    m_inUseCard->setVisible(hasData);
    m_inUseLabel->setVisible(hasData);
}

void SelectedKernelsWidget::setRecommendedKernel(const QVariantMap& data)
{
    m_recommendedCard->setKernelData(data);
    bool hasData = !data.isEmpty();
    m_recommendedCard->setVisible(hasData);
    m_recommendedLabel->setVisible(hasData);
    
    if (hasData) {
        bool isInstalled = data.value("isInstalled").toBool();
        if (isInstalled) {
            m_recommendedLabel->setText(tr("Recommended (choose in boot menu)"));
        } else {
            m_recommendedLabel->setText(tr("Recommended"));
        }
    }
}

} // namespace mcp::qt::kernel
