/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Classic Qt Widgets-based Kernel Manager implementation.
 * Uses plain widgets for maximum compatibility with traditional DEs.
 */

#include "KernelWidgetView.h"
#include "../KernelListModel.h"

#include <QBoxLayout>
#include <QDesktopServices>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QUrl>

namespace mcp::qt::kernel {

KernelWidgetView::KernelWidgetView(KernelViewModel* viewModel, QWidget* parent)
    : QWidget(parent)
    , m_viewModel(viewModel)
{
    setupUi();
    setupConnections();
    
    // Initial population
    onKernelsDataChanged();
}

void KernelWidgetView::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);
    
    auto* topLayout = new QHBoxLayout();
    topLayout->setSpacing(12);
    
    m_inUseCard = new QGroupBox(tr("Current"), this);
    auto* inUseLayout = new QHBoxLayout(m_inUseCard);
    inUseLayout->setSpacing(8);
    
    auto* inUseInfoLayout = new QVBoxLayout();
    inUseInfoLayout->setSpacing(2);
    
    m_inUseName = new QLabel(this);
    QFont nameFont = m_inUseName->font();
    nameFont.setBold(true);
    m_inUseName->setFont(nameFont);
    inUseInfoLayout->addWidget(m_inUseName);
    
    m_inUseVersionAndChangelog = new QLabel(this);
    m_inUseVersionAndChangelog->setOpenExternalLinks(true);
    m_inUseVersionAndChangelog->setTextFormat(Qt::RichText);
    inUseInfoLayout->addWidget(m_inUseVersionAndChangelog);
    
    inUseLayout->addLayout(inUseInfoLayout, 1);
    
    m_inUseActionBtn = new QPushButton(tr("In Use"), this);
    m_inUseActionBtn->setEnabled(false);
    inUseLayout->addWidget(m_inUseActionBtn);
    
    topLayout->addWidget(m_inUseCard);
    
    m_recommendedCard = new QGroupBox(tr("Recommended"), this);
    auto* recommendedLayout = new QHBoxLayout(m_recommendedCard);
    recommendedLayout->setSpacing(8);
    
    auto* recommendedInfoLayout = new QVBoxLayout();
    recommendedInfoLayout->setSpacing(2);
    
    m_recommendedName = new QLabel(this);
    m_recommendedName->setFont(nameFont);
    recommendedInfoLayout->addWidget(m_recommendedName);
    
    m_recommendedVersionAndChangelog = new QLabel(this);
    m_recommendedVersionAndChangelog->setOpenExternalLinks(true);
    m_recommendedVersionAndChangelog->setTextFormat(Qt::RichText);
    recommendedInfoLayout->addWidget(m_recommendedVersionAndChangelog);
    
    recommendedLayout->addLayout(recommendedInfoLayout, 1);
    
    m_recommendedActionBtn = new QPushButton(tr("Install"), this);
    recommendedLayout->addWidget(m_recommendedActionBtn);
    
    topLayout->addWidget(m_recommendedCard);
    
    mainLayout->addLayout(topLayout);
    
    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);
    
    auto* tableLabel = new QLabel(tr("Available Kernels"), this);
    QFont tableFont = tableLabel->font();
    tableFont.setBold(true);
    tableLabel->setFont(tableFont);
    mainLayout->addWidget(tableLabel);
    
    m_kernelTable = new QTableWidget(this);
    m_kernelTable->setColumnCount(5);
    m_kernelTable->setHorizontalHeaderLabels({
        tr("Name"), tr("Version"), tr("Status"), tr("Type"), tr("")
    });
    m_kernelTable->horizontalHeader()->setStretchLastSection(false);
    m_kernelTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_kernelTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_kernelTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_kernelTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_kernelTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_kernelTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_kernelTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_kernelTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_kernelTable->verticalHeader()->setVisible(false);
    m_kernelTable->setAlternatingRowColors(true);
    mainLayout->addWidget(m_kernelTable, 1);
    
    auto* statusLayout = new QHBoxLayout();
    m_statusLabel = new QLabel(tr("Ready"), this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_progressBar);
    mainLayout->addLayout(statusLayout);
}

void KernelWidgetView::setupConnections()
{
    connect(m_viewModel, &KernelViewModel::kernelsDataChanged,
            this, &KernelWidgetView::onKernelsDataChanged);
    connect(m_viewModel, &KernelViewModel::fetchProgress,
            this, &KernelWidgetView::onFetchProgress);
    
    connect(m_viewModel->model(), &QAbstractItemModel::modelReset,
            this, &KernelWidgetView::refreshKernelTable);
    connect(m_viewModel->model(), &QAbstractItemModel::dataChanged,
            this, &KernelWidgetView::refreshKernelTable);
    
    connect(m_recommendedActionBtn, &QPushButton::clicked, this, &KernelWidgetView::onInstallClicked);
}

void KernelWidgetView::onKernelsDataChanged()
{
    QVariantMap inUseData = m_viewModel->inUseKernelData();
    if (!inUseData.isEmpty()) {
        m_inUseName->setText(inUseData.value("name").toString());
        
        QString version = inUseData.value("version").toString();
        QString changelogUrl = inUseData.value("changelogUrl").toString();
        
        QStringList tags;
        if (inUseData.value("isLTS").toBool()) tags << "LTS";
        if (inUseData.value("isRealTime").toBool()) tags << "Real-Time";
        
        QString infoText = version;
        if (!changelogUrl.isEmpty()) {
            infoText += QString(" | <a href=\"%1\">Changelog</a>").arg(changelogUrl);
        }
        if (!tags.isEmpty()) {
            infoText += " | " + tags.join(", ");
        }
        m_inUseVersionAndChangelog->setText(infoText);
        
        m_inUseActionBtn->setText(tr("In Use"));
        m_inUseActionBtn->setEnabled(false);
        m_inUseCard->setVisible(true);
    } else {
        m_inUseCard->setVisible(false);
    }
    
    QVariantMap recommendedData = m_viewModel->recommendedKernelData();
    if (!recommendedData.isEmpty()) {
        m_recommendedName->setText(recommendedData.value("name").toString());
        
        QString version = recommendedData.value("version").toString();
        QString changelogUrl = recommendedData.value("changelogUrl").toString();
        
        QStringList tags;
        if (recommendedData.value("isLTS").toBool()) tags << "LTS";
        if (recommendedData.value("isRealTime").toBool()) tags << "Real-Time";
        
        QString infoText = version;
        if (!changelogUrl.isEmpty()) {
            infoText += QString(" | <a href=\"%1\">Changelog</a>").arg(changelogUrl);
        }
        if (!tags.isEmpty()) {
            infoText += " | " + tags.join(", ");
        }
        m_recommendedVersionAndChangelog->setText(infoText);
        
        m_recommendedActionBtn->setProperty("kernelName", recommendedData.value("name"));
        m_recommendedActionBtn->setProperty("extraModules", recommendedData.value("extraModules"));
        
        bool isInstalled = recommendedData.value("isInstalled").toBool();
        m_recommendedActionBtn->setEnabled(!isInstalled);
        m_recommendedActionBtn->setText(isInstalled ? tr("Installed") : tr("Install"));
        m_recommendedCard->setVisible(true);
    } else {
        m_recommendedCard->setVisible(false);
    }
    
    refreshKernelTable();
}

void KernelWidgetView::onFetchProgress(int current, int total, const QString& kernelName)
{
    if (total == 0) {
        m_progressBar->setVisible(false);
        m_statusLabel->setText(tr("Ready"));
        return;
    }
    
    m_progressBar->setVisible(true);
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(current);
    m_statusLabel->setText(tr("Loading %1...").arg(kernelName));
    
    if (current >= total) {
        m_progressBar->setVisible(false);
        m_statusLabel->setText(tr("Ready"));
    }
}

void KernelWidgetView::refreshKernelTable()
{
    populateKernelTable();
}

void KernelWidgetView::populateKernelTable()
{
    KernelListModel* model = m_viewModel->model();
    int rowCount = model->rowCount(QModelIndex());
    
    m_kernelTable->setRowCount(rowCount);
    
    for (int row = 0; row < rowCount; ++row) {
        QModelIndex idx = model->index(row, 0);
        
        QString name = model->data(idx, KernelListModel::Name).toString();
        QString version = model->data(idx, KernelListModel::Version).toString();
        bool isInstalled = model->data(idx, KernelListModel::IsInstalled).toBool();
        bool isInUse = model->data(idx, KernelListModel::IsInUse).toBool();
        bool isLTS = model->data(idx, KernelListModel::IsLTS).toBool();
        bool isRT = model->data(idx, KernelListModel::IsRealTime).toBool();
        bool isEOL = model->data(idx, KernelListModel::IsEOL).toBool();
        bool isExperimental = model->data(idx, KernelListModel::IsExperimental).toBool();
        QString changelogUrl = model->data(idx, KernelListModel::ChangelogUrl).toString();
        QStringList extraModules = model->data(idx, KernelListModel::ExtraModules).toStringList();
        
        // Name
        auto* nameItem = new QTableWidgetItem(name);
        if (isInUse) {
            QFont boldFont = nameItem->font();
            boldFont.setBold(true);
            nameItem->setFont(boldFont);
        }
        m_kernelTable->setItem(row, 0, nameItem);
        
        // Version with changelog link
        auto* versionLabel = new QLabel(this);
        versionLabel->setTextFormat(Qt::RichText);
        versionLabel->setOpenExternalLinks(true);
        if (!changelogUrl.isEmpty()) {
            versionLabel->setText(QString("%1 (<a href=\"%2\">changelog</a>)").arg(version, changelogUrl));
        } else {
            versionLabel->setText(version);
        }
        m_kernelTable->setCellWidget(row, 1, versionLabel);
        
        // Status
        QString status;
        if (isInUse) {
            status = tr("In Use");
        } else if (isInstalled) {
            status = tr("Installed");
        } else {
            status = tr("Available");
        }
        auto* statusItem = new QTableWidgetItem(status);
        if (isInUse) {
            statusItem->setForeground(QColor(0, 128, 0));  // Green
        }
        m_kernelTable->setItem(row, 2, statusItem);
        
        // Type tags
        QStringList tags;
        if (isLTS) tags << "LTS";
        if (isRT) tags << "RT";
        if (isEOL) tags << "EOL";
        if (isExperimental) tags << "Experimental";
        auto* typeItem = new QTableWidgetItem(tags.join(", "));
        if (isEOL) {
            typeItem->setForeground(QColor(200, 100, 0));  // Orange warning
        }
        m_kernelTable->setItem(row, 3, typeItem);
        
        // Action button (Install/Remove)
        auto* actionBtn = new QPushButton(this);
        actionBtn->setProperty("kernelName", name);
        actionBtn->setProperty("extraModules", extraModules);
        
        if (isInUse) {
            actionBtn->setText(tr("In Use"));
            actionBtn->setEnabled(false);
        } else if (isInstalled) {
            actionBtn->setText(tr("Remove"));
            connect(actionBtn, &QPushButton::clicked, this, &KernelWidgetView::onRemoveClicked);
        } else {
            actionBtn->setText(tr("Install"));
            connect(actionBtn, &QPushButton::clicked, this, &KernelWidgetView::onInstallClicked);
        }
        m_kernelTable->setCellWidget(row, 4, actionBtn);
    }
}

void KernelWidgetView::onInstallClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QString kernelName = btn->property("kernelName").toString();
    QStringList extraModules = btn->property("extraModules").toStringList();
    
    confirmAndInstall(kernelName, extraModules);
}

void KernelWidgetView::onRemoveClicked()
{
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;
    
    QString kernelName = btn->property("kernelName").toString();
    confirmAndRemove(kernelName);
}

void KernelWidgetView::confirmAndInstall(const QString& kernelName, const QStringList& extraModules)
{
    QString message = tr("Install kernel %1?").arg(kernelName);
    
    if (!extraModules.isEmpty()) {
        message += "\n\n" + tr("The following extra packages will also be installed:");
        for (const QString& mod : extraModules) {
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
        m_viewModel->installKernel(kernelName);
    }
}

void KernelWidgetView::confirmAndRemove(const QString& kernelName)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Removal"),
        tr("Remove kernel %1?\n\nThis action cannot be undone.").arg(kernelName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_viewModel->removeKernel(kernelName);
    }
}

} // namespace mcp::qt::kernel
