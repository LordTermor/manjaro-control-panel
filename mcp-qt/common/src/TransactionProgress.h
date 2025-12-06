/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once
#include <QObject>
#include <QString>

namespace mcp::qt::common {

struct TransactionProgress
{
    Q_GADGET

    Q_PROPERTY(QString action READ action)
    Q_PROPERTY(QString details READ details)
    Q_PROPERTY(double progress READ progress)

public:
    QString action() const { return m_action; }
    QString details() const { return m_details; }
    double progress() const { return m_progress; }

    QString m_action;
    QString m_details;
    double m_progress = 0.0;
};
} // namespace mcp::qt::common

Q_DECLARE_METATYPE(mcp::qt::common::TransactionProgress)
