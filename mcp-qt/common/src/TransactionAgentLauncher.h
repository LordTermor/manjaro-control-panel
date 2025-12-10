/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * TransactionAgentLauncher - Qt wrapper for launching transaction agent process.
 * 
 * Simplifies spawning mcp-transaction-agent and monitoring its progress.
 * Emits signals for integration with Qt/QML UI.
 */

#pragma once

#include <agent/Command.hpp>

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace mcp::qt::common {

class TransactionAgentLauncher : public QObject
{
    Q_OBJECT

public:
    explicit TransactionAgentLauncher(QObject* parent = nullptr);
    ~TransactionAgentLauncher() override;

    void launchCommand(const mcp::agent::Command& cmd);
    
    // Legacy methods for backwards compatibility
    void installPackages(const QStringList& packages);
    void removePackages(const QStringList& packages, bool force = false);
    void upgradeSystem(bool forceRefresh = false);
    
    bool isRunning() const;

Q_SIGNALS:
    void started();
    void outputReceived(const QString& output);
    void finished(bool success, int exitCode);
    void error(const QString& errorMessage);

private:
    void launchAgent(const QStringList& arguments);
    void handleReadyReadStandardOutput();
    void handleReadyReadStandardError();
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleError(QProcess::ProcessError error);

    QProcess* m_process;
    QString m_agentPath;
};

} // namespace mcp::qt::common
