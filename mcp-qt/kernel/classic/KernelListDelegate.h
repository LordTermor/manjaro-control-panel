/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include "../KernelListModel.h"

#include <QStyledItemDelegate>
#include <QStyle>

/*
 * Custom delegate for kernel list items.
 * Paints rows with version name, inline badges, and action buttons.
 */

namespace mcp::qt::kernel {

class KernelListDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit KernelListDelegate(QObject* parent = nullptr);
    ~KernelListDelegate() override = default;

    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, 
                   const QModelIndex& index) const override;
    bool editorEvent(QEvent* event, QAbstractItemModel* model,
                     const QStyleOptionViewItem& option, 
                     const QModelIndex& index) override;

Q_SIGNALS:
    void installClicked(const QModelIndex& index);
    void removeClicked(const QModelIndex& index);
    void changelogClicked(const QModelIndex& index);

private:
    struct ButtonRects {
        QRectF actionButton;
        QRectF changelogButton;
    };
    
    ButtonRects calculateButtonRects(const QStyleOptionViewItem& option) const;
    
    void drawBadge(QPainter* painter, QRectF& rect, const QString& text,
                   const QColor& bgColor, const QColor& borderColor, 
                   const QColor& textColor) const;
    
    void drawKernelName(QPainter* painter, const QRect& itemRect,
                        const QString& version, const QFont& baseFont) const;
    
    QRectF getKernelNameRect(const QRect& itemRect, const QString& version,
                             const QFont& baseFont) const;
    
    void drawInlineBadges(QPainter* painter, const QRectF& nameRect,
                          bool isRealTime, bool isLTS, const QFont& baseFont) const;
    
    void drawStatusBadges(QPainter* painter, const QRect& itemRect,
                          const ButtonRects& buttonRects, bool isInUse,
                          bool isInstalled, bool isEOL, bool isExperimental,
                          const QFont& baseFont) const;
    
    void drawPackageInfo(QPainter* painter, const QRectF& nameRect,
                         const QString& name, bool isLTS, bool isInUse,
                         const QString& changelogUrl, const QFont& baseFont) const;

    mutable QStyle::State m_actionButtonState = QStyle::State_Raised;
    mutable QStyle::State m_changelogButtonState = QStyle::State_Raised;
    mutable QRectF m_changelogLinkRect;
    mutable bool m_changelogHovered = false;
};

} // namespace mcp::qt::kernel
