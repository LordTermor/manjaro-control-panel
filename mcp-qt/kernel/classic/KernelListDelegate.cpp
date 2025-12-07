/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelListDelegate.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionButton>
#include <QIcon>

/*
 * Custom delegate for kernel list items.
 * Paints rows with version name, inline color-coded badges, and action button.
 * Changelog is rendered as a clickable link next to the package name.
 */

namespace mcp::qt::kernel {

namespace {
    // Badge colors - alpha 38/77 for bg/border transparency matching QML
    const QColor BadgeRTBg = QColor(52, 152, 219, 38);
    const QColor BadgeRTBorder = QColor(52, 152, 219, 77);
    const QColor BadgeRTText("#3498db");
    
    const QColor BadgeLTSBg = QColor(233, 117, 23, 38);
    const QColor BadgeLTSBorder = QColor(233, 117, 23, 77);
    const QColor BadgeLTSText("#e97517");
    
    const QColor BadgeGreenBg("#d6e9c6");
    const QColor BadgeGreenBorder("#dff0d8");
    const QColor BadgeGreenText("#3c763d");
    
    const QColor BadgeRedBg("#f2dede");
    const QColor BadgeRedBorder("#ebccd1");
    const QColor BadgeRedText("#a94442");
    
    const QColor BadgeYellowBg("#FCF8E3");
    const QColor BadgeYellowBorder("#FAEBCC");
    const QColor BadgeYellowText("#8A6D3B");
    
    const QColor LinkColor("#2980b9");
    
    constexpr int Padding = 5;
}

KernelListDelegate::KernelListDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

QSize KernelListDelegate::sizeHint(const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
    Q_UNUSED(index)
    
    QFont buttonFont = option.font;
    QFontMetrics fm(buttonFont);
    QSize buttonSize = fm.size(Qt::TextSingleLine, "TestTest");
    return QSize(300, ((buttonSize.height() + 8) * 2) + 10);
}

KernelListDelegate::ButtonRects 
KernelListDelegate::calculateButtonRects(const QStyleOptionViewItem& option) const
{
    QString removeStr = tr("Remove");
    QString installStr = tr("Install");
    QString changelogStr = tr("Changelog");
    QStringList buttonTexts = {removeStr, installStr, changelogStr};
    
    QFont buttonFont = option.font;
    QFontMetrics fm(buttonFont);
    
    int buttonWidth = 0;
    int buttonHeight = 0;
    for (const QString& str : buttonTexts) {
        QSize size = fm.size(Qt::TextSingleLine, str);
        buttonWidth = qMax(buttonWidth, size.width());
        buttonHeight = qMax(buttonHeight, size.height());
    }
    
    ButtonRects rects;
    
    // Action button (Install/Remove) - centered vertically
    rects.actionButton = QRectF(QPointF(), QSize(buttonWidth + 40, buttonHeight + 8));
    rects.actionButton.moveCenter(QPointF(option.rect.right() - buttonWidth/2 - 30,
                                          option.rect.center().y()));
    
    // Changelog button - below action button
    rects.changelogButton = QRectF(QPointF(), QSize(buttonWidth + 20, buttonHeight + 8));
    rects.changelogButton.moveTopRight(QPointF(option.rect.right() - Padding,
                                               option.rect.center().y() + 2));
    
    return rects;
}

void KernelListDelegate::drawBadge(QPainter* painter, QRectF& rect, const QString& text,
                                    const QColor& bgColor, const QColor& borderColor,
                                    const QColor& textColor) const
{
    painter->setRenderHint(QPainter::Antialiasing, true);
    
    QPainterPath path;
    path.addRoundedRect(rect, 4, 4);
    
    painter->fillPath(path, bgColor);
    painter->setPen(QPen(borderColor, 1));
    painter->drawPath(path);
    painter->setPen(textColor);
    
    QFont badgeFont = painter->font();
    badgeFont.setBold(true);
    badgeFont.setCapitalization(QFont::AllUppercase);
    badgeFont.setPointSize(badgeFont.pointSize() - 1);
    painter->setFont(badgeFont);
    painter->drawText(rect, Qt::AlignCenter, text);
}

void KernelListDelegate::drawKernelName(QPainter* painter, const QRect& itemRect,
                                         const QString& version, const QFont& baseFont) const
{
    QString displayName = "Linux " + version;
    
    QFont nameFont = baseFont;
    nameFont.setPointSize(baseFont.pointSize() * 1.75);
    nameFont.setBold(true);
    QFontMetrics nameFm(nameFont);
    
    QSize nameSize = nameFm.size(Qt::TextSingleLine, displayName);
    QRectF nameRect(QPointF(), nameSize);
    nameRect.moveTopLeft(itemRect.topLeft() + QPoint(Padding, Padding));
    
    painter->setFont(nameFont);
    painter->drawText(nameRect, Qt::TextSingleLine, displayName);
}

QRectF KernelListDelegate::getKernelNameRect(const QRect& itemRect, const QString& version,
                                               const QFont& baseFont) const
{
    QString displayName = "Linux " + version;
    
    QFont nameFont = baseFont;
    nameFont.setPointSize(baseFont.pointSize() * 1.75);
    nameFont.setBold(true);
    QFontMetrics nameFm(nameFont);
    
    QSize nameSize = nameFm.size(Qt::TextSingleLine, displayName);
    QRectF nameRect(QPointF(), nameSize);
    nameRect.moveTopLeft(itemRect.topLeft() + QPoint(Padding, Padding));
    
    return nameRect;
}

void KernelListDelegate::drawInlineBadges(QPainter* painter, const QRectF& nameRect,
                                           bool isRealTime, bool isLTS, const QFont& baseFont) const
{
    QFont labelFont = baseFont;
    labelFont.setPointSize(baseFont.pointSize() - 1);
    QFontMetrics labelFm(labelFont);
    
    int labelHeight = labelFm.height() + 4;
    
    painter->setFont(labelFont);
    int badgeX = nameRect.right() + Padding * 2;
    int badgeY = nameRect.top() + (nameRect.height() - labelHeight) / 2;
    
    if (isRealTime) {
        QString realtimeStr = tr("Real-Time");
        int w = qMin(labelFm.horizontalAdvance(realtimeStr) + Padding * 2, 120);
        QRectF badgeRect(badgeX, badgeY, w, labelHeight);
        drawBadge(painter, badgeRect, realtimeStr, BadgeRTBg, BadgeRTBorder, BadgeRTText);
        badgeX += w + Padding;
    }
    
    if (isLTS) {
        QString ltsStr = tr("LTS");
        int w = qMin(labelFm.horizontalAdvance(ltsStr) + Padding * 2, 120);
        QRectF badgeRect(badgeX, badgeY, w, labelHeight);
        drawBadge(painter, badgeRect, ltsStr, BadgeLTSBg, BadgeLTSBorder, BadgeLTSText);
    }
}

void KernelListDelegate::drawStatusBadges(QPainter* painter, const QRect& itemRect,
                                           const ButtonRects& buttonRects, bool isInUse,
                                           bool isInstalled, bool isEOL, bool isExperimental,
                                           const QFont& baseFont) const
{
    QFont labelFont = baseFont;
    labelFont.setPointSize(baseFont.pointSize() - 1);
    QFontMetrics labelFm(labelFont);
    
    int labelHeight = labelFm.height() + 4;
    int buttonAreaWidth = buttonRects.actionButton.width() + Padding * 2 + 40;
    
    painter->setFont(labelFont);
    int statusBadgeX = itemRect.right() - buttonAreaWidth;
    int statusBadgeY = itemRect.top() + Padding;
    
    if (isInUse) {
        QString runningStr = tr("Running");
        int w = qMin(labelFm.horizontalAdvance(runningStr) + Padding * 2, 120);
        QRectF badgeRect(statusBadgeX - w, statusBadgeY, w, labelHeight);
        drawBadge(painter, badgeRect, runningStr, BadgeGreenBg, BadgeGreenBorder, BadgeGreenText);
        statusBadgeY += labelHeight + 4;
    }
    
    if (isInstalled && !isInUse) {
        QString installedStr = tr("Installed");
        int w = qMin(labelFm.horizontalAdvance(installedStr) + Padding * 2, 120);
        QRectF badgeRect(statusBadgeX - w, statusBadgeY, w, labelHeight);
        drawBadge(painter, badgeRect, installedStr, BadgeGreenBg, BadgeGreenBorder, BadgeGreenText);
        statusBadgeY += labelHeight + 4;
    }
    
    if (isEOL) {
        QString unsupportedStr = tr("Unsupported");
        int w = qMin(labelFm.horizontalAdvance(unsupportedStr) + Padding * 2, 120);
        QRectF badgeRect(statusBadgeX - w, statusBadgeY, w, labelHeight);
        drawBadge(painter, badgeRect, unsupportedStr, BadgeRedBg, BadgeRedBorder, BadgeRedText);
    }
    
    if (isExperimental) {
        QString experimentalStr = tr("Experimental");
        int w = qMin(labelFm.horizontalAdvance(experimentalStr) + Padding * 2, 120);
        QRectF badgeRect(statusBadgeX - w, statusBadgeY, w, labelHeight);
        drawBadge(painter, badgeRect, experimentalStr, BadgeYellowBg, BadgeYellowBorder, BadgeYellowText);
    }
}

void KernelListDelegate::drawPackageInfo(QPainter* painter, const QRectF& nameRect,
                                          const QString& name, bool isLTS, bool isInUse,
                                          const QString& changelogUrl, const QFont& baseFont) const
{
    QFont packageFont = baseFont;
    packageFont.setPointSize(baseFont.pointSize() * 0.9);
    QFontMetrics packageFm(packageFont);
    
    QString infoLine = name;
    if (isLTS) {
        infoLine += " • Long-term support";
    } else if (isInUse) {
        infoLine += " • Current kernel";
    }
    
    QSize infoSize = packageFm.size(Qt::TextSingleLine, infoLine);
    QRectF infoRect(QPointF(), infoSize);
    infoRect.moveTopLeft(nameRect.bottomLeft() + QPoint(0, 2));
    
    painter->setFont(packageFont);
    painter->drawText(infoRect, Qt::TextSingleLine, infoLine);
    
    if (!changelogUrl.isEmpty()) {
        QString separator = " • ";
        QString changelogText = tr("Changelog");
        QString linkIcon = " ↗";
        
        int separatorWidth = packageFm.horizontalAdvance(separator);
        int changelogWidth = packageFm.horizontalAdvance(changelogText + linkIcon);
        
        QRectF sepRect(infoRect.right(), infoRect.top(), separatorWidth, infoRect.height());
        painter->drawText(sepRect, Qt::TextSingleLine, separator);
        
        m_changelogLinkRect = QRectF(sepRect.right(), infoRect.top(), changelogWidth, infoRect.height());
        
        painter->setPen(LinkColor);
        QFont linkFont = packageFont;
        if (m_changelogHovered) {
            linkFont.setUnderline(true);
        }
        painter->setFont(linkFont);
        painter->drawText(m_changelogLinkRect, Qt::TextSingleLine, changelogText + linkIcon);
    }
}

void KernelListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, QModelIndex());
    
    painter->save();
    
    QString name = index.data(KernelListModel::Name).toString();
    QString version = index.data(KernelListModel::Version).toString();
    bool isInstalled = index.data(KernelListModel::IsInstalled).toBool();
    bool isInUse = index.data(KernelListModel::IsInUse).toBool();
    bool isLTS = index.data(KernelListModel::IsLTS).toBool();
    bool isRealTime = index.data(KernelListModel::IsRealTime).toBool();
    bool isEOL = index.data(KernelListModel::IsEOL).toBool();
    bool isExperimental = index.data(KernelListModel::IsExperimental).toBool();
    QString changelogUrl = index.data(KernelListModel::ChangelogUrl).toString();
    
    ButtonRects buttonRects = calculateButtonRects(option);
    
    if (!isInUse) {
        QStyleOptionButton actionBtn;
        actionBtn.rect = buttonRects.actionButton.toRect();
        actionBtn.text = isInstalled ? tr("Remove") : tr("Install");
        actionBtn.icon = isInstalled ? QIcon::fromTheme("edit-delete") : QIcon::fromTheme("download");
        actionBtn.iconSize = QSize(16, 16);
        actionBtn.state = m_actionButtonState | QStyle::State_Enabled;
        QApplication::style()->drawControl(QStyle::CE_PushButton, &actionBtn, painter);
    }
    
    painter->setPen(option.palette.color(QPalette::Normal, QPalette::WindowText));
    drawKernelName(painter, option.rect, version, option.font);
    
    QRectF nameRect = getKernelNameRect(option.rect, version, option.font);
    
    drawInlineBadges(painter, nameRect, isRealTime, isLTS, option.font);
    drawStatusBadges(painter, option.rect, buttonRects, isInUse, isInstalled, 
                     isEOL, isExperimental, option.font);
    
    painter->setPen(option.palette.color(QPalette::Disabled, QPalette::WindowText));
    drawPackageInfo(painter, nameRect, name, isLTS, isInUse, changelogUrl, option.font);
    
    painter->restore();
}

bool KernelListDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index)
{
    Q_UNUSED(model)
    
    bool isInUse = index.data(KernelListModel::IsInUse).toBool();
    bool isInstalled = index.data(KernelListModel::IsInstalled).toBool();
    QString changelogUrl = index.data(KernelListModel::ChangelogUrl).toString();
    
    ButtonRects buttonRects = calculateButtonRects(option);
    
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF pos = mouseEvent->position();
        
        bool wasHovered = m_changelogHovered;
        m_changelogHovered = m_changelogLinkRect.contains(pos) && !changelogUrl.isEmpty();
        
        if (wasHovered != m_changelogHovered) {
            return true;
        }
        return false;
    }
    
    if (event->type() != QEvent::MouseButtonPress &&
        event->type() != QEvent::MouseButtonRelease) {
        m_actionButtonState = QStyle::State_Raised;
        return true;
    }
    
    QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
    QPointF pos = mouseEvent->position();
    
    bool inActionButton = buttonRects.actionButton.contains(pos) && !isInUse;
    bool inChangelogLink = m_changelogLinkRect.contains(pos) && !changelogUrl.isEmpty();
    
    if (!inActionButton && !inChangelogLink) {
        m_actionButtonState = QStyle::State_Raised;
        return true;
    }
    
    if (event->type() == QEvent::MouseButtonPress) {
        if (inActionButton) {
            m_actionButtonState = QStyle::State_Sunken;
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        m_actionButtonState = QStyle::State_Raised;
        
        if (inActionButton) {
            if (isInstalled) {
                Q_EMIT removeClicked(index);
            } else {
                Q_EMIT installClicked(index);
            }
        }
        if (inChangelogLink) {
            Q_EMIT changelogClicked(index);
        }
    }
    
    return true;
}

} // namespace mcp::qt::kernel
