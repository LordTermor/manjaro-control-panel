/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include "BadgeWidget.h"
#include "../KernelData.h"

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVariantMap>

/*
 * Unified kernel item widget used for both card display and list rows.
 * Mode determines styling: Card has frame borders, ListItem has zebra striping.
 */

namespace mcp::qt::kernel {

class KernelItemWidget : public QFrame
{
    Q_OBJECT

public:
    enum Mode { Card, ListItem };
    
    explicit KernelItemWidget(Mode mode = ListItem, QWidget* parent = nullptr);
    ~KernelItemWidget() override = default;

    void setKernelData(const KernelData& data);
    void setKernelData(const QVariantMap& data);
    void setAlternateBackground(bool alternate);
    
    QString kernelName() const { return m_name; }
    QStringList extraModules() const { return m_extraModules; }
    bool isInstalled() const { return m_isInstalled; }
    bool isInUse() const { return m_isInUse; }

Q_SIGNALS:
    void installClicked(const QString& name, const QStringList& extraModules);
    void removeClicked(const QString& name);
    void changelogClicked(const QString& changelogUrl);

private:
    void setupUi();
    void updateDisplay();
    void clearBadges();
    void addInlineBadge(const QString& text, BadgeWidget::Type type);
    void addStatusBadge(const QString& text, BadgeWidget::Type type);

    Mode m_mode;
    
    // UI elements
    QLabel* m_kernelNameLabel;
    QLabel* m_packageInfoLabel;
    QPushButton* m_actionButton;
    QHBoxLayout* m_inlineBadgesLayout;
    QHBoxLayout* m_statusBadgesLayout;
    
    // Data
    QString m_name;
    QString m_version;
    QString m_changelogUrl;
    QStringList m_extraModules;
    
    bool m_isInstalled = false;
    bool m_isInUse = false;
    bool m_isLTS = false;
    bool m_isRealTime = false;
    bool m_isEOL = false;
    bool m_isExperimental = false;
    
    QList<BadgeWidget*> m_inlineBadges;
    QList<BadgeWidget*> m_statusBadges;
};

} // namespace mcp::qt::kernel
