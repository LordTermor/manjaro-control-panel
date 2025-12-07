/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <QFrame>
#include <QPushButton>
#include <QVariantMap>
#include <QRect>

/*
 * Kernel card widget for "Current" and "Recommended" sections.
 * Custom-painted with QPainter to match KernelListDelegate styling.
 */

namespace mcp::qt::kernel {

class KernelCardWidget : public QFrame
{
    Q_OBJECT

public:
    explicit KernelCardWidget(QWidget* parent = nullptr);
    ~KernelCardWidget() override = default;

    void setKernelData(const QVariantMap& data);
    void setLabel(const QString& label);
    
    QString kernelName() const { return m_name; }
    QStringList extraModules() const { return m_extraModules; }
    bool isInstalled() const { return m_isInstalled; }
    bool isInUse() const { return m_isInUse; }

Q_SIGNALS:
    void installClicked(const QString& name, const QStringList& extraModules);
    void removeClicked(const QString& name);
    void changelogClicked(const QString& changelogUrl);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    void updateButtonState();
    
    void drawBadge(QPainter& painter, const QRect& rect, const QString& text,
                   const QColor& bgColor, const QColor& borderColor, const QColor& textColor) const;
    
    void drawKernelName(QPainter& painter, int& currentX, int row1Y) const;
    int getNameFontHeight() const;
    void drawInlineBadges(QPainter& painter, int& currentX, int row1Y, int nameFontHeight) const;
    void drawPackageInfo(QPainter& painter, int row2Y) const;

    QString m_name;
    QString m_version;
    QString m_changelogUrl;
    QStringList m_extraModules;
    QString m_label;
    
    bool m_isInstalled = false;
    bool m_isInUse = false;
    bool m_isLTS = false;
    bool m_isRecommended = false;
    bool m_isRealTime = false;
    bool m_isEOL = false;
    bool m_isExperimental = false;
    
    bool m_changelogHovered = false;
    mutable QRect m_changelogLinkRect;

    QPushButton* m_actionBtn;
};

} // namespace mcp::qt::kernel
