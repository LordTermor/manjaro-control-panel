/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "ProgressNotifier.h"

namespace mcp::qt::common {

ProgressNotifier::ProgressNotifier(QObject *parent) : QObject(parent) {}

ProgressNotifier::~ProgressNotifier()
{
    disconnect_transaction();
}

void ProgressNotifier::connect_transaction(pamac::Transaction& transaction)
{
    // Disconnect any existing connections
    disconnect_transaction();

    // Connect to pamac transaction signals
    m_connections.push_back(
        transaction.signal_emit_action.connect([this](const std::string& action) {
            QMetaObject::invokeMethod(
                this, [this]() { Q_EMIT started(); }, ::Qt::QueuedConnection);
        })
    );

    m_connections.push_back(
        transaction.signal_emit_action_progress.connect(
            [this](const std::string& action, const std::string& details, double progress) {
                QMetaObject::invokeMethod(
                    this,
                    [this, action = QString::fromStdString(action),
                          details = QString::fromStdString(details), progress]() {
                        Q_EMIT progressChanged(TransactionProgress{action, details, progress});
                    },
                    ::Qt::QueuedConnection);
            })
    );

    m_connections.push_back(
        transaction.signal_emit_download_progress.connect(
            [this](const std::string& action, const std::string& details, double progress) {
                QMetaObject::invokeMethod(
                    this,
                    [this, action = QString::fromStdString(action),
                          details = QString::fromStdString(details), progress]() {
                        Q_EMIT progressChanged(TransactionProgress{action, details, progress});
                    },
                    ::Qt::QueuedConnection);
            })
    );

    m_connections.push_back(
        transaction.signal_emit_error.connect(
            [this](const std::string& message, const std::vector<std::string>&) {
                QMetaObject::invokeMethod(
                    this,
                    [this, message = QString::fromStdString(message)]() {
                        Q_EMIT errorOccured(message);
                        Q_EMIT finished(false);
                    },
                    ::Qt::QueuedConnection);
            })
    );
}

void ProgressNotifier::disconnect_transaction()
{
    for (auto& connection : m_connections) {
        connection.disconnect();
    }
    m_connections.clear();
}

} // namespace mcp::qt::common
