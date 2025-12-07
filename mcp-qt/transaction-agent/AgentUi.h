/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * AgentUi - Simple widget that shows transaction progress.
 * Takes cmdline args, runs pamac transaction, exits with status code.
 */

#pragma once

#include <ProgressFlattener.hpp>

#include <QCoro/QCoroTask>
#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QStringList>

// Forward declarations
namespace pamac {
class Database;
class Transaction;
}

namespace mcp::agent {

class AgentUi : public QWidget
{
    Q_OBJECT

public:
    explicit AgentUi(QWidget* parent = nullptr);
    ~AgentUi() override = default;

    void startTransaction(
        const QString& operation,
        const QStringList& packages,
        bool force = false,
        bool refresh = false
    );

Q_SIGNALS:
    void transactionFinished(bool success);

private:
    QCoro::Task<void> runInstall(const QStringList& packages);
    QCoro::Task<void> runRemove(const QStringList& packages, bool force);
    QCoro::Task<void> runUpgrade(bool refresh);

    void connectTransactionSignals(pamac::Transaction& txn, mcp::ProgressFlattener& flattener);
    void logMessage(const QString& msg);
    void updateProgress(const QString& action, double progress);
    void toggleDetails();

    pamac::Database& m_database;
    
    // UI components
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    QPushButton* m_detailsButton;
    QTextEdit* m_logView;
};

} // namespace mcp::agent
