/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include "KernelCardWidget.h"

#include <QWidget>

class QLabel;

/*
 * Container for Current and Recommended kernel cards.
 * Displays two KernelCardWidget instances side by side.
 */

namespace mcp::qt::kernel {

class SelectedKernelsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SelectedKernelsWidget(QWidget* parent = nullptr);
    ~SelectedKernelsWidget() override = default;

    void setInUseKernel(const QVariantMap& data);
    void setRecommendedKernel(const QVariantMap& data);

Q_SIGNALS:
    void installClicked(const QString& name, const QStringList& extraModules);
    void removeClicked(const QString& name);
    void changelogClicked(const QString& changelogUrl);

private:
    KernelCardWidget* m_inUseCard;
    KernelCardWidget* m_recommendedCard;
    QLabel* m_inUseLabel;
    QLabel* m_recommendedLabel;
};

} // namespace mcp::qt::kernel
