/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelPage.h"
#include "../KernelListModel.h"

#include <QBoxLayout>
#include <QDesktopServices>
#include <QFrame>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QUrl>

/*
 * Main kernel manager page.
 * Top section shows Current/Recommended kernel cards.
 * Bottom section shows full kernel list with custom delegate.
 */

namespace mcp::qt::kernel {

KernelPage::KernelPage(KernelViewModel* viewModel, QWidget* parent)
    : QWidget(parent)
    , m_viewModel(viewModel)
{
    setupUi();
    setupConnections();
    
    // Initial data load
    onKernelsDataChanged();
}

void KernelPage::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);
    
    m_selectedKernels = new SelectedKernelsWidget(this);
    mainLayout->addWidget(m_selectedKernels);
    
    auto* separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(separator);
    
    m_kernelListView = new QListView(this);
    m_kernelListView->setAlternatingRowColors(true);
    m_kernelListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_kernelListView->setFocusPolicy(Qt::NoFocus);
    m_kernelListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    
    auto* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(m_viewModel->model());
    proxyModel->setSortRole(KernelListModel::MajorVersion);
    proxyModel->sort(0, Qt::DescendingOrder);
    
    m_kernelListView->setModel(proxyModel);
    
    m_listDelegate = new KernelListDelegate(this);
    m_kernelListView->setItemDelegate(m_listDelegate);
    
    mainLayout->addWidget(m_kernelListView, 1);
}

void KernelPage::setupConnections()
{
    connect(m_viewModel, &KernelViewModel::kernelsDataChanged,
            this, &KernelPage::onKernelsDataChanged);
    connect(m_viewModel, &KernelViewModel::fetchProgress,
            this, &KernelPage::onFetchProgress);
    
    connect(m_listDelegate, &KernelListDelegate::installClicked,
            this, &KernelPage::onInstallClicked);
    connect(m_listDelegate, &KernelListDelegate::removeClicked,
            this, &KernelPage::onRemoveClicked);
    connect(m_listDelegate, &KernelListDelegate::changelogClicked,
            this, &KernelPage::onChangelogClicked);
    
    connect(m_selectedKernels, &SelectedKernelsWidget::installClicked,
            this, &KernelPage::onCardInstallClicked);
    connect(m_selectedKernels, &SelectedKernelsWidget::removeClicked,
            this, &KernelPage::onCardRemoveClicked);
    connect(m_selectedKernels, &SelectedKernelsWidget::changelogClicked,
            this, &KernelPage::onCardChangelogClicked);
}

void KernelPage::onKernelsDataChanged()
{
    m_selectedKernels->setInUseKernel(m_viewModel->inUseKernelData());
    m_selectedKernels->setRecommendedKernel(m_viewModel->recommendedKernelData());
}

void KernelPage::onFetchProgress(int current, int total, const QString& kernelName)
{
    Q_UNUSED(current)
    Q_UNUSED(total)
    Q_UNUSED(kernelName)
}

void KernelPage::onInstallClicked(const QModelIndex& index)
{
    QModelIndex sourceIndex = index;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(m_kernelListView->model())) {
        sourceIndex = proxy->mapToSource(index);
    }
    
    QString name = sourceIndex.data(KernelListModel::Name).toString();
    QStringList extraModules = sourceIndex.data(KernelListModel::ExtraModules).toStringList();
    
    confirmAndInstall(name, extraModules);
}

void KernelPage::onRemoveClicked(const QModelIndex& index)
{
    QModelIndex sourceIndex = index;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(m_kernelListView->model())) {
        sourceIndex = proxy->mapToSource(index);
    }
    
    QString name = sourceIndex.data(KernelListModel::Name).toString();
    confirmAndRemove(name);
}

void KernelPage::onChangelogClicked(const QModelIndex& index)
{
    QModelIndex sourceIndex = index;
    if (auto* proxy = qobject_cast<QSortFilterProxyModel*>(m_kernelListView->model())) {
        sourceIndex = proxy->mapToSource(index);
    }
    
    QString changelogUrl = sourceIndex.data(KernelListModel::ChangelogUrl).toString();
    openChangelog(changelogUrl);
}

void KernelPage::onCardInstallClicked(const QString& name, const QStringList& extraModules)
{
    confirmAndInstall(name, extraModules);
}

void KernelPage::onCardRemoveClicked(const QString& name)
{
    confirmAndRemove(name);
}

void KernelPage::onCardChangelogClicked(const QString& changelogUrl)
{
    openChangelog(changelogUrl);
}

void KernelPage::confirmAndInstall(const QString& kernelName, const QStringList& extraModules)
{
    QString message = tr("Install Linux kernel %1?").arg(kernelName);
    
    if (!extraModules.isEmpty()) {
        message += "\n\n" + tr("The following packages will also be installed:");
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

void KernelPage::confirmAndRemove(const QString& kernelName)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Removal"),
        tr("Remove Linux kernel %1?\n\nThis action cannot be undone.").arg(kernelName),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        m_viewModel->removeKernel(kernelName);
    }
}

void KernelPage::openChangelog(const QString& changelogUrl)
{
    if (!changelogUrl.isEmpty()) {
        QDesktopServices::openUrl(QUrl(changelogUrl));
    }
}

} // namespace mcp::qt::kernel
