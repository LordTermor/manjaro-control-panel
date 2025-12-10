/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelPage.h"
#include "../KernelData.h"
#include "../KernelListModel.h"

#include <QAbstractItemModel>
#include <QDesktopServices>
#include <QFrame>
#include <QMessageBox>
#include <QScrollArea>
#include <QUrl>

/*
 * Main kernel manager page.
 * Uses KernelItemWidget in Card mode for top section,
 * and ListItem mode for the scrollable kernel list.
 */

namespace mcp::qt::kernel {

KernelPage::KernelPage(KernelViewModel* viewModel, QWidget* parent)
    : QWidget(parent)
    , m_viewModel(viewModel)
{
    setupUi();
    setupConnections();
    onKernelsDataChanged();
}

void KernelPage::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);
    
    auto* cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(12);
    
    auto* inUseLayout = new QVBoxLayout();
    inUseLayout->setSpacing(4);
    m_inUseLabel = new QLabel(tr("Currently used"), this);
    inUseLayout->addWidget(m_inUseLabel);
    m_inUseCard = new KernelItemWidget(KernelItemWidget::Card, this);
    inUseLayout->addWidget(m_inUseCard);
    cardsLayout->addLayout(inUseLayout, 1);
    
    auto* recommendedLayout = new QVBoxLayout();
    recommendedLayout->setSpacing(4);
    m_recommendedLabel = new QLabel(tr("Recommended"), this);
    recommendedLayout->addWidget(m_recommendedLabel);
    m_recommendedCard = new KernelItemWidget(KernelItemWidget::Card, this);
    recommendedLayout->addWidget(m_recommendedCard);
    cardsLayout->addLayout(recommendedLayout, 1);
    
    mainLayout->addLayout(cardsLayout);
    
    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);
    
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::StyledPanel);
    scrollArea->setProperty("_breeze_force_frame", true);

    
    auto* scrollContent = new QWidget(scrollArea);
    m_kernelListLayout = new QVBoxLayout(scrollContent);
    m_kernelListLayout->setContentsMargins(0, 0, 0, 0);
    m_kernelListLayout->setSpacing(0);
    
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);
}

void KernelPage::setupConnections()
{
    connect(m_viewModel, &KernelViewModel::kernelsDataChanged,
            this, &KernelPage::onKernelsDataChanged);
    
    connect(m_viewModel->model(), &QAbstractItemModel::modelReset,
            this, &KernelPage::populateKernelList);
    connect(m_viewModel->model(), &QAbstractItemModel::rowsInserted,
            this, &KernelPage::populateKernelList);
    
    connect(m_inUseCard, &KernelItemWidget::installClicked,
            this, &KernelPage::onInstallClicked);
    connect(m_inUseCard, &KernelItemWidget::removeClicked,
            this, &KernelPage::onRemoveClicked);
    connect(m_inUseCard, &KernelItemWidget::changelogClicked,
            this, &KernelPage::onChangelogClicked);
    
    connect(m_recommendedCard, &KernelItemWidget::installClicked,
            this, &KernelPage::onInstallClicked);
    connect(m_recommendedCard, &KernelItemWidget::removeClicked,
            this, &KernelPage::onRemoveClicked);
    connect(m_recommendedCard, &KernelItemWidget::changelogClicked,
            this, &KernelPage::onChangelogClicked);
}

void KernelPage::onKernelsDataChanged()
{
    KernelData inUseData = m_viewModel->inUseKernelData();
    if (inUseData.isValid()) {
        m_inUseCard->setKernelData(inUseData);
        m_inUseCard->setVisible(true);
        m_inUseLabel->setVisible(true);
    } else {
        m_inUseCard->setVisible(false);
        m_inUseLabel->setVisible(false);
    }
    
    KernelData recommendedData = m_viewModel->recommendedKernelData();
    if (recommendedData.isValid()) {
        m_recommendedCard->setKernelData(recommendedData);
        m_recommendedCard->setVisible(true);
        m_recommendedLabel->setVisible(true);
        
        m_recommendedLabel->setText(recommendedData.isInstalled 
            ? tr("Recommended (choose in boot menu)") 
            : tr("Recommended"));
    } else {
        m_recommendedCard->setVisible(false);
        m_recommendedLabel->setVisible(false);
    }
    
    populateKernelList();
}

void KernelPage::populateKernelList()
{
    for (auto* item : m_listItems) {
        m_kernelListLayout->removeWidget(item);
        delete item;
    }
    m_listItems.clear();
    
    for (auto* header : m_sectionHeaders) {
        m_kernelListLayout->removeWidget(header);
        delete header;
    }
    m_sectionHeaders.clear();
    
    for (auto* separator : m_sectionSeparators) {
        m_kernelListLayout->removeWidget(separator);
        delete separator;
    }
    m_sectionSeparators.clear();
    
    QLayoutItem* stretch = m_kernelListLayout->takeAt(m_kernelListLayout->count() - 1);
    if (stretch) delete stretch;
    
    KernelListModel* model = m_viewModel->model();
    
    // Model already returns items sorted by category (In use, Installed, LTS, Other)
    // so we just iterate in order and add section headers when category changes
    QString currentCategory;
    int itemIndexInSection = 0;
    
    for (int row = 0; row < model->rowCount(QModelIndex()); ++row) {
        QModelIndex idx = model->index(row, 0);
        
        QString category = model->data(idx, KernelListModel::Category).toString();
        if (category != currentCategory) {
            if (!currentCategory.isEmpty()) {
                auto* separator = new QFrame(this);
                separator->setFrameShape(QFrame::HLine);
                separator->setFrameShadow(QFrame::Sunken);
                m_kernelListLayout->addWidget(separator);
                m_sectionSeparators.append(separator);
            }
            
            currentCategory = category;
            itemIndexInSection = 0;
            
            auto* header = new QLabel(category, this);
            QFont headerFont = header->font();
            headerFont.setBold(true);
            header->setFont(headerFont);
            header->setContentsMargins(12, 12, 12, 6);
            m_kernelListLayout->addWidget(header);
            m_sectionHeaders.append(header);
        }
        
        QVariantMap data;
        data["name"] = model->data(idx, KernelListModel::Name);
        data["version"] = model->data(idx, KernelListModel::Version);
        data["changelogUrl"] = model->data(idx, KernelListModel::ChangelogUrl);
        data["extraModules"] = model->data(idx, KernelListModel::ExtraModules);
        data["isInstalled"] = model->data(idx, KernelListModel::IsInstalled);
        data["isInUse"] = model->data(idx, KernelListModel::IsInUse);
        data["isLTS"] = model->data(idx, KernelListModel::IsLTS);
        data["isRealTime"] = model->data(idx, KernelListModel::IsRealTime);
        data["isEOL"] = model->data(idx, KernelListModel::IsEOL);
        data["isExperimental"] = model->data(idx, KernelListModel::IsExperimental);
        
        auto* item = new KernelItemWidget(KernelItemWidget::ListItem, this);
        item->setKernelData(data);
        
        // Zebra striping per section
        item->setAlternateBackground(itemIndexInSection % 2 == 1);
        itemIndexInSection++;
        
        connect(item, &KernelItemWidget::installClicked,
                this, &KernelPage::onInstallClicked);
        connect(item, &KernelItemWidget::removeClicked,
                this, &KernelPage::onRemoveClicked);
        connect(item, &KernelItemWidget::changelogClicked,
                this, &KernelPage::onChangelogClicked);
        
        m_kernelListLayout->addWidget(item);
        m_listItems.append(item);
    }
    
    m_kernelListLayout->addStretch();
}

void KernelPage::onInstallClicked(const KernelData& kernelData)
{
    confirmAndInstall(kernelData);
}

void KernelPage::onRemoveClicked(const KernelData& kernelData)
{
    confirmAndRemove(kernelData);
}

void KernelPage::onChangelogClicked(const QString& changelogUrl)
{
    if (!changelogUrl.isEmpty()) {
        QDesktopServices::openUrl(QUrl(changelogUrl));
    }
}

void KernelPage::confirmAndInstall(const KernelData& kernelData)
{
    QString message = tr("Install Linux kernel %1?").arg(kernelData.name);
    
    if (!kernelData.extraModules.isEmpty()) {
        message += "\n\n" + tr("The following packages will also be installed:");
        for (const QString& mod : kernelData.extraModules) {
            message += "\n  • " + mod;
        }
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Installation"),
        message,
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_viewModel->installKernel(kernelData);
    }
}

void KernelPage::confirmAndRemove(const KernelData& kernelData)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Removal"),
        tr("Remove Linux kernel %1?\n\nThis action cannot be undone.").arg(kernelData.name),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_viewModel->removeKernel(kernelData);
    }
}

} // namespace mcp::qt::kernel
