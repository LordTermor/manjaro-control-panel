/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-FileCopyrightText: 2025 Daniil Ludwig <eightbyte81@gmail.com>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include <QLabel>

/*
 * Stylesheet-based colored badge widget.
 * Displays status/type labels (LTS, RT, Running, etc.) with predefined color schemes.
 */

namespace mcp::qt::kernel {

class BadgeWidget : public QLabel
{
    Q_OBJECT

public:
    enum Type {
        LTS,
        RealTime,
        Installed,
        Running,
        EOL,
        Experimental,
        Recommended
    };
    Q_ENUM(Type)

    explicit BadgeWidget(QWidget* parent = nullptr);
    BadgeWidget(const QString& text, Type type, QWidget* parent = nullptr);
    ~BadgeWidget() override = default;

    void setType(Type type);
    Type type() const { return m_type; }

private:
    void applyStyle();

    Type m_type = LTS;
};

} // namespace mcp::qt::kernel
