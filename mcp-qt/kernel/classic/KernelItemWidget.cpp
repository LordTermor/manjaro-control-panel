/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelItemWidget.h"

#include <QIcon>
#include <QPalette>

/*
 * Unified kernel item widget.
 * Card mode: framed panel for Current/Recommended sections.
 * ListItem mode: flat row with optional zebra striping.
 */

namespace mcp::qt::kernel {

KernelItemWidget::KernelItemWidget(Mode mode, QWidget* parent)
    : QFrame(parent)
    , m_mode(mode)
{
    setupUi();
    
    if (m_mode == Card) {
        setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        setLineWidth(1);
        setProperty("_breeze_force_frame", true);
    } else {
        setFrameStyle(QFrame::NoFrame);
    }
    
    connect(m_actionButton, &QPushButton::clicked, this, [this]() {
        if (m_isInUse) return;
        if (m_isInstalled) {
            Q_EMIT removeClicked(m_name);
        } else {
            Q_EMIT installClicked(m_name, m_extraModules);
        }
    });
    
    connect(m_packageInfoLabel, &QLabel::linkActivated, this, [this](const QString&) {
        Q_EMIT changelogClicked(m_changelogUrl);
    });
}

void KernelItemWidget::setupUi()
{
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, m_mode == Card ? 8 : 6, 12, m_mode == Card ? 8 : 6);
    
    auto* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(4);
    
    auto* titleRow = new QHBoxLayout();
    titleRow->setSpacing(8);
    
    m_kernelNameLabel = new QLabel(this);
    QFont nameFont = m_kernelNameLabel->font();
    nameFont.setPointSize(m_mode == Card ? 12 : 11);
    nameFont.setBold(true);
    m_kernelNameLabel->setFont(nameFont);
    titleRow->addWidget(m_kernelNameLabel);
    
    m_inlineBadgesLayout = new QHBoxLayout();
    m_inlineBadgesLayout->setSpacing(6);
    titleRow->addLayout(m_inlineBadgesLayout);
    
    titleRow->addStretch();
    infoLayout->addLayout(titleRow);
    
    m_packageInfoLabel = new QLabel(this);
    m_packageInfoLabel->setTextFormat(Qt::RichText);
    m_packageInfoLabel->setOpenExternalLinks(false);
    QFont infoFont = m_packageInfoLabel->font();
    infoFont.setPointSize(9);
    m_packageInfoLabel->setFont(infoFont);
    infoLayout->addWidget(m_packageInfoLabel);
    
    mainLayout->addLayout(infoLayout, 1);
    
    m_statusBadgesLayout = new QHBoxLayout();
    m_statusBadgesLayout->setSpacing(6);
    mainLayout->addLayout(m_statusBadgesLayout);
    
    m_actionButton = new QPushButton(this);
    m_actionButton->setMinimumWidth(90);
    mainLayout->addWidget(m_actionButton);
    
    setMinimumSize(m_mode == Card ? QSize(300, 60) : QSize(400, 50));
}

void KernelItemWidget::setKernelData(const KernelData& data)
{
    m_name = data.name;
    m_version = data.version;
    m_changelogUrl = data.changelogUrl;
    m_extraModules = data.extraModules;
    
    m_isInstalled = data.isInstalled;
    m_isInUse = data.isInUse;
    m_isLTS = data.isLTS;
    m_isRealTime = false;  // Not in KernelData, handled separately by list model
    m_isEOL = false;       // Not in KernelData, handled separately by list model
    m_isExperimental = false; // Not in KernelData, handled separately by list model
    
    updateDisplay();
}

void KernelItemWidget::setKernelData(const QVariantMap& data)
{
    m_name = data.value("name").toString();
    m_version = data.value("version").toString();
    m_changelogUrl = data.value("changelogUrl").toString();
    m_extraModules = data.value("extraModules").toStringList();
    
    m_isInstalled = data.value("isInstalled").toBool();
    m_isInUse = data.value("isInUse").toBool();
    m_isLTS = data.value("isLTS").toBool();
    m_isRealTime = data.value("isRealTime").toBool();
    m_isEOL = data.value("isEOL").toBool();
    m_isExperimental = data.value("isExperimental").toBool();
    
    updateDisplay();
}

void KernelItemWidget::updateDisplay()
{
    m_kernelNameLabel->setText("Linux " + m_version);
    
    QString info = m_name;
    
    if (!m_changelogUrl.isEmpty()) {
        info += QString(" • <a href=\"%1\" style=\"color: #2980b9;\">%2</a> ↗")
            .arg(m_changelogUrl, tr("Changelog"));
    }
    m_packageInfoLabel->setText(info);
    
    clearBadges();
    
    if (m_isRealTime) {
        addInlineBadge(tr("RT"), BadgeWidget::RealTime);
    }
    if (m_isLTS) {
        addInlineBadge(tr("LTS"), BadgeWidget::LTS);
    }
    
    if (m_isEOL) {
        addStatusBadge(tr("Unsupported"), BadgeWidget::EOL);
    }
    if (m_isExperimental) {
        addStatusBadge(tr("Experimental"), BadgeWidget::Experimental);
    }
    
    if (m_isInUse) {
        m_actionButton->setText(tr("Running"));
        m_actionButton->setIcon(QIcon());
        m_actionButton->setEnabled(false);
    } else if (m_isInstalled) {
        m_actionButton->setText(tr("Remove"));
        m_actionButton->setIcon(QIcon::fromTheme("edit-delete"));
        m_actionButton->setEnabled(true);
    } else {
        m_actionButton->setText(tr("Install"));
        m_actionButton->setIcon(QIcon::fromTheme("download"));
        m_actionButton->setEnabled(true);
    }
}

void KernelItemWidget::clearBadges()
{
    for (auto* badge : m_inlineBadges) {
        m_inlineBadgesLayout->removeWidget(badge);
        delete badge;
    }
    m_inlineBadges.clear();
    
    for (auto* badge : m_statusBadges) {
        m_statusBadgesLayout->removeWidget(badge);
        delete badge;
    }
    m_statusBadges.clear();
}

void KernelItemWidget::addInlineBadge(const QString& text, BadgeWidget::Type type)
{
    auto* badge = new BadgeWidget(text, type, this);
    m_inlineBadgesLayout->addWidget(badge);
    m_inlineBadges.append(badge);
}

void KernelItemWidget::addStatusBadge(const QString& text, BadgeWidget::Type type)
{
    auto* badge = new BadgeWidget(text, type, this);
    m_statusBadgesLayout->addWidget(badge);
    m_statusBadges.append(badge);
}

void KernelItemWidget::setAlternateBackground(bool alternate)
{
    setAutoFillBackground(true);
    if (alternate) {
        QPalette pal = palette();
        pal.setColor(QPalette::Window, pal.color(QPalette::AlternateBase));
        setPalette(pal);
    }
}

} // namespace mcp::qt::kernel
