/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#pragma once

#include "TransactionProgress.h"

#include <QObject>
#include <pamac/transaction.hpp>
#include <sigc++/connection.h>
#include <memory>

namespace mcp::qt::common {

/**
 * ProgressNotifier - Qt wrapper for PamacCpp::Transaction signals.
 * 
 * Connects to pamac-cpp transaction signals and emits Qt signals
 * for use in Qt/QML applications.
 */
class ProgressNotifier : public QObject
{
    Q_OBJECT
public:
    explicit ProgressNotifier(QObject *parent = nullptr);
    ~ProgressNotifier() override;

    /**
     * Connect to a pamac::Transaction.
     * This sets up signal forwarding from pamac to Qt.
     */
    void connect_transaction(pamac::Transaction& transaction);

    /**
     * Disconnect from current transaction.
     */
    void disconnect_transaction();

Q_SIGNALS:
    void started();
    void progressChanged(const mcp::qt::common::TransactionProgress &progress);
    void finished(bool success);
    void errorOccured(const QString &message);

private:
    std::vector<sigc::connection> m_connections;
};

} // namespace mcp::qt::common
