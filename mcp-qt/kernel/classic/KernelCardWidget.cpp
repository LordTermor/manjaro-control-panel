/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelCardWidget.h"

#include <QIcon>

/*
 * Kernel card widget using .ui layout.
 * Dynamically adds/removes BadgeWidget instances based on kernel properties.
 */

namespace mcp::qt::kernel {

KernelCardWidget::KernelCardWidget(QWidget* parent)
    : QFrame(parent)
{
    ui.setupUi(this);
    
    // Force frame borders on Breeze and similar themes
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(1);
    setProperty("_breeze_force_frame", true);
    
    connect(ui.actionButton, &QPushButton::clicked, this, [this]() {
        if (m_isInUse) return;
        if (m_isInstalled) {
            Q_EMIT removeClicked(m_name);
        } else {
            Q_EMIT installClicked(m_name, m_extraModules);
        }
    });
    
    // Forward changelog link clicks
    connect(ui.packageInfoLabel, &QLabel::linkActivated, this, [this](const QString&) {
        Q_EMIT changelogClicked(m_changelogUrl);
    });
}

void KernelCardWidget::setKernelData(const QVariantMap& data)
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
    
    ui.kernelNameLabel->setText("Linux " + m_version);
    
    updateBadges();
    updateButtonState();
    updatePackageInfo();
}

void KernelCardWidget::updateBadges()
{
    // Clear existing badges
    for (auto* badge : m_badges) {
        ui.badgesLayout->removeWidget(badge);
        delete badge;
    }
    m_badges.clear();
    
    auto addBadge = [this](const QString& text, BadgeWidget::Type type) {
        auto* badge = new BadgeWidget(text, type, this);
        ui.badgesLayout->addWidget(badge);
        m_badges.append(badge);
    };
    
    if (m_isInUse) {
        addBadge(tr("Running"), BadgeWidget::Running);
    } else if (m_isInstalled) {
        addBadge(tr("Installed"), BadgeWidget::Installed);
    }
    
    if (m_isLTS) {
        addBadge(tr("LTS"), BadgeWidget::LTS);
    }
    if (m_isRealTime) {
        addBadge(tr("RT"), BadgeWidget::RealTime);
    }
    if (m_isExperimental) {
        addBadge(tr("Experimental"), BadgeWidget::Experimental);
    }
    if (m_isEOL) {
        addBadge(tr("EOL"), BadgeWidget::EOL);
    }
}

void KernelCardWidget::updateButtonState()
{
    if (m_isInUse) {
        ui.actionButton->setText(tr("Running"));
        ui.actionButton->setIcon(QIcon());
        ui.actionButton->setEnabled(false);
    } else if (m_isInstalled) {
        ui.actionButton->setText(tr("Remove"));
        ui.actionButton->setIcon(QIcon::fromTheme("edit-delete"));
        ui.actionButton->setEnabled(true);
    } else {
        ui.actionButton->setText(tr("Install"));
        ui.actionButton->setIcon(QIcon::fromTheme("download"));
        ui.actionButton->setEnabled(true);
    }
}

void KernelCardWidget::updatePackageInfo()
{
    QString info = m_name;
    
    if (!m_changelogUrl.isEmpty()) {
        info += QString(" • <a href=\"%1\" style=\"color: #2980b9;\">%2</a> ↗")
            .arg(m_changelogUrl, tr("Changelog"));
    }
    
    ui.packageInfoLabel->setText(info);
}

} // namespace mcp::qt::kernel
