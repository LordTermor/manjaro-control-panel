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

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>

namespace mcp::qt::common {

/**
 * Launches and monitors mcp-transaction-agent subprocess.
 * 
 * Usage:
 *   TransactionAgentLauncher launcher;
 *   connect(&launcher, &TransactionAgentLauncher::finished, ...);
 *   
 *   launcher.installPackages({"linux66", "linux66-headers"});
 *   // or
 *   launcher.removePackages({"linux515"}, false);
 *   // or
 *   launcher.upgradeSystem(true);
 */
class TransactionAgentLauncher : public QObject
{
    Q_OBJECT

public:
    explicit TransactionAgentLauncher(QObject* parent = nullptr);
    ~TransactionAgentLauncher() override = default;

    /**
     * Launch agent to install packages.
     */
    void installPackages(const QStringList& packages);

    /**
     * Launch agent to remove packages.
     * @param force - bypass safety checks
     */
    void removePackages(const QStringList& packages, bool force = false);

    /**
     * Launch agent to upgrade system.
     * @param forceRefresh - force database refresh
     */
    void upgradeSystem(bool forceRefresh = false);

    /**
     * Check if agent process is currently running.
     */
    bool isRunning() const;

Q_SIGNALS:
    /**
     * Emitted when agent process starts.
     */
    void started();

    /**
     * Emitted with agent's stdout/stderr output.
     * Can be parsed for progress information.
     */
    void outputReceived(const QString& output);

    /**
     * Emitted when agent process finishes.
     * @param success - true if exit code was 0
     * @param exitCode - process exit code
     */
    void finished(bool success, int exitCode);

    /**
     * Emitted on process error (failed to start, crashed, etc.)
     */
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
