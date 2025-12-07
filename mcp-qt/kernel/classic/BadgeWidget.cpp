/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "BadgeWidget.h"

/*
 * Stylesheet-based colored badge widget.
 * Uses CSS-like styling for background, border, and text colors.
 */

namespace mcp::qt::kernel {

namespace {
    // Color schemes: background, border, text (matching QML badge colors)
    struct BadgeColors {
        std::string_view bg;
        std::string_view border;
        std::string_view text;
    };

    const BadgeColors colorSchemes[] = {
        // LTS - orange
        {"rgba(233, 117, 23, 0.15)", "rgba(233, 117, 23, 0.3)", "#e97517"},
        // RealTime - blue
        {"rgba(52, 152, 219, 0.15)", "rgba(52, 152, 219, 0.3)", "#3498db"},
        // Installed - green
        {"rgba(60, 118, 61, 0.15)", "rgba(60, 118, 61, 0.3)", "#3c763d"},
        // Running - green (same as installed)
        {"rgba(60, 118, 61, 0.15)", "rgba(60, 118, 61, 0.3)", "#3c763d"},
        // EOL - red
        {"rgba(231, 76, 60, 0.15)", "rgba(231, 76, 60, 0.3)", "#e74c3c"},
        // Experimental - yellow
        {"rgba(241, 196, 15, 0.15)", "rgba(241, 196, 15, 0.3)", "#f39c12"},
    };
}

BadgeWidget::BadgeWidget(QWidget* parent)
    : QLabel(parent)
{
    applyStyle();
}

BadgeWidget::BadgeWidget(const QString& text, Type type, QWidget* parent)
    : QLabel(text, parent)
    , m_type(type)
{
    applyStyle();
}

void BadgeWidget::setType(Type type)
{
    if (m_type == type) return;
    m_type = type;
    applyStyle();
}

void BadgeWidget::applyStyle()
{
    const auto& colors = colorSchemes[static_cast<int>(m_type)];
    
    setStyleSheet(QString(
        "QLabel {"
        "  background-color: %1;"
        "  border: 1px solid %2;"
        "  border-radius: 4px;"
        "  color: %3;"
        "  padding: 2px 8px;"
        "  font-weight: bold;"
        "  font-size: 9pt;"
        "  text-transform: uppercase;"
        "}"
    ).arg(colors.bg, colors.border, colors.text));
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

} // namespace mcp::qt::kernel
