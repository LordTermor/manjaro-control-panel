/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "KernelCardWidget.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QStyleOption>
#include <QUrl>
#include <QIcon>

/*
 * Kernel card widget for "Current" and "Recommended" sections.
 * Custom-painted with QPainter to match KernelListDelegate styling.
 * Displays kernel info with badges and a clickable changelog link.
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
    
    const QColor BadgeGreenBg = QColor(60, 118, 61, 38);
    const QColor BadgeGreenBorder = QColor(60, 118, 61, 77);
    const QColor BadgeGreenText("#3c763d");
    
    const QColor BadgeRedBg = QColor(231, 76, 60, 38);
    const QColor BadgeRedBorder = QColor(231, 76, 60, 77);
    const QColor BadgeRedText("#e74c3c");
    
    const QColor BadgeExperimentalBg = QColor(241, 196, 15, 38);
    const QColor BadgeExperimentalBorder = QColor(241, 196, 15, 77);
    const QColor BadgeExperimentalText("#f39c12");
    
    const QColor LinkColor("#2980b9");
    
    constexpr int Padding = 8;
    constexpr int BadgeSpacing = 6;
}

KernelCardWidget::KernelCardWidget(QWidget* parent)
    : QFrame(parent)
    , m_actionBtn(new QPushButton(this))
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(1);
    setProperty("_breeze_force_frame", true);
    
    setMouseTracking(true);
    
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(Padding, Padding, Padding, Padding);
    mainLayout->addStretch();
    mainLayout->addWidget(m_actionBtn);
    
    connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
        if (m_isInUse) return;
        if (m_isInstalled) {
            Q_EMIT removeClicked(m_name);
        } else {
            Q_EMIT installClicked(m_name, m_extraModules);
        }
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
    m_isRecommended = data.value("isRecommended").toBool();
    m_isRealTime = data.value("isRealTime").toBool();
    m_isEOL = data.value("isEOL").toBool();
    m_isExperimental = data.value("isExperimental").toBool();
    
    updateButtonState();
    update();
}

void KernelCardWidget::setLabel(const QString& label)
{
    m_label = label;
    update();
}

void KernelCardWidget::updateButtonState()
{
    if (m_isInUse) {
        m_actionBtn->setText(tr("Running"));
        m_actionBtn->setIcon(QIcon());
        m_actionBtn->setEnabled(false);
    } else if (m_isInstalled) {
        m_actionBtn->setText(tr("Remove"));
        m_actionBtn->setIcon(QIcon::fromTheme("edit-delete"));
        m_actionBtn->setEnabled(true);
    } else {
        m_actionBtn->setText(tr("Install"));
        m_actionBtn->setIcon(QIcon::fromTheme("download"));
        m_actionBtn->setEnabled(true);
    }
}

QSize KernelCardWidget::sizeHint() const
{
    QFontMetrics fm(font());
    int height = fm.height() * 3 + Padding * 3;
    return QSize(400, qMax(height, 70));
}

QSize KernelCardWidget::minimumSizeHint() const
{
    return QSize(300, 60);
}

void KernelCardWidget::drawBadge(QPainter& painter, const QRect& rect, const QString& text,
                                  const QColor& bgColor, const QColor& borderColor, 
                                  const QColor& textColor) const
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    QPainterPath path;
    path.addRoundedRect(rect, 4, 4);
    
    painter.fillPath(path, bgColor);
    painter.setPen(QPen(borderColor, 1));
    painter.drawPath(path);
    painter.setPen(textColor);
    
    QFont badgeFont = painter.font();
    badgeFont.setBold(true);
    badgeFont.setCapitalization(QFont::AllUppercase);
    badgeFont.setPointSize(badgeFont.pointSize() - 1);
    painter.setFont(badgeFont);
    painter.drawText(rect, Qt::AlignCenter, text);
}

void KernelCardWidget::drawKernelName(QPainter& painter, int& currentX, int row1Y) const
{
    QFont nameFont = font();
    nameFont.setPointSize(font().pointSize() + 2);
    nameFont.setBold(true);
    painter.setFont(nameFont);
    QFontMetrics nameFm(nameFont);
    
    QString displayName = "Linux " + m_version;
    painter.setPen(palette().color(QPalette::WindowText));
    painter.drawText(currentX, row1Y + nameFm.ascent(), displayName);
    currentX += nameFm.horizontalAdvance(displayName) + Padding;
}

int KernelCardWidget::getNameFontHeight() const
{
    QFont nameFont = font();
    nameFont.setPointSize(font().pointSize() + 2);
    nameFont.setBold(true);
    QFontMetrics nameFm(nameFont);
    return nameFm.height();
}

void KernelCardWidget::drawInlineBadges(QPainter& painter, int& currentX, int row1Y, int nameFontHeight) const
{
    QFont badgeFont = font();
    badgeFont.setPointSize(font().pointSize() - 1);
    painter.setFont(badgeFont);
    QFontMetrics badgeFm(badgeFont);
    int badgeHeight = badgeFm.height() + 4;
    int badgeY = row1Y + (nameFontHeight - badgeHeight) / 2;
    
    auto drawBadgeInline = [&](const QString& text, const QColor& bg, const QColor& border, const QColor& textColor) {
        int w = qMin(badgeFm.horizontalAdvance(text) + Padding * 2, 120);
        QRect badgeRect(currentX, badgeY, w, badgeHeight);
        drawBadge(painter, badgeRect, text, bg, border, textColor);
        currentX += w + BadgeSpacing;
    };
    
    if (m_isInUse) {
        drawBadgeInline(tr("Running"), BadgeGreenBg, BadgeGreenBorder, BadgeGreenText);
    } else if (m_isInstalled) {
        drawBadgeInline(tr("Installed"), BadgeGreenBg, BadgeGreenBorder, BadgeGreenText);
    }
    
    if (m_isLTS) {
        drawBadgeInline(tr("LTS"), BadgeLTSBg, BadgeLTSBorder, BadgeLTSText);
    }
    if (m_isRealTime) {
        drawBadgeInline(tr("RT"), BadgeRTBg, BadgeRTBorder, BadgeRTText);
    }
    if (m_isExperimental) {
        drawBadgeInline(tr("Experimental"), BadgeExperimentalBg, BadgeExperimentalBorder, BadgeExperimentalText);
    }
    if (m_isEOL) {
        drawBadgeInline(tr("EOL"), BadgeRedBg, BadgeRedBorder, BadgeRedText);
    }
}

void KernelCardWidget::drawPackageInfo(QPainter& painter, int row2Y) const
{
    QFont packageFont = font();
    packageFont.setPointSize(font().pointSize() - 1);
    painter.setFont(packageFont);
    QFontMetrics packageFm(packageFont);
    
    painter.setPen(palette().color(QPalette::PlaceholderText));
    painter.drawText(Padding, row2Y + packageFm.ascent(), m_name);
    
    if (!m_changelogUrl.isEmpty()) {
        int pkgWidth = packageFm.horizontalAdvance(m_name);
        QString linkText = tr("Changelog") + " ↗";
        int linkX = Padding + pkgWidth + Padding;
        int linkWidth = packageFm.horizontalAdvance(linkText);
        
        m_changelogLinkRect = QRect(linkX, row2Y, linkWidth, packageFm.height());
        
        QFont linkFont = packageFont;
        linkFont.setUnderline(m_changelogHovered);
        painter.setFont(linkFont);
        painter.setPen(LinkColor);
        painter.drawText(linkX, row2Y + packageFm.ascent(), linkText);
    } else {
        m_changelogLinkRect = QRect();
    }
}

void KernelCardWidget::paintEvent(QPaintEvent* event)
{
    QFrame::paintEvent(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_name.isEmpty()) {
        painter.setPen(palette().color(QPalette::Disabled, QPalette::Text));
        painter.drawText(rect(), Qt::AlignCenter, tr("No kernel selected"));
        return;
    }
    
    int nameFontHeight = getNameFontHeight();
    QFont packageFont = font();
    packageFont.setPointSize(font().pointSize() - 1);
    QFontMetrics packageFm(packageFont);
    int totalContentHeight = nameFontHeight + 4 + packageFm.height();
    
    int availableHeight = height() - Padding * 2;
    int row1Y = Padding + (availableHeight - totalContentHeight) / 2;
    
    int currentX = Padding;
    drawKernelName(painter, currentX, row1Y);
    drawInlineBadges(painter, currentX, row1Y, nameFontHeight);
    
    int row2Y = row1Y + nameFontHeight + 4;
    drawPackageInfo(painter, row2Y);
}

void KernelCardWidget::mouseMoveEvent(QMouseEvent* event)
{
    bool wasHovered = m_changelogHovered;
    m_changelogHovered = m_changelogLinkRect.contains(event->pos());
    
    if (wasHovered != m_changelogHovered) {
        setCursor(m_changelogHovered ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
    
    QWidget::mouseMoveEvent(event);
}

void KernelCardWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_changelogLinkRect.contains(event->pos())) {
        Q_EMIT changelogClicked(m_changelogUrl);
        return;
    }
    QWidget::mousePressEvent(event);
}

void KernelCardWidget::leaveEvent(QEvent* event)
{
    if (m_changelogHovered) {
        m_changelogHovered = false;
        setCursor(Qt::ArrowCursor);
        update();
    }
    QWidget::leaveEvent(event);
}

} // namespace mcp::qt::kernel
