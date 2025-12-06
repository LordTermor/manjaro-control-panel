/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "TransactionAgentLauncher.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>

namespace mcp::qt::common {

TransactionAgentLauncher::TransactionAgentLauncher(QObject* parent)
    : QObject(parent)
    , m_process(new QProcess(this))
{
    // Find agent executable - try multiple locations
    
    // 1. Try in PATH
    m_agentPath = QStandardPaths::findExecutable("mcp-transaction-agent");
    
    if (m_agentPath.isEmpty()) {
        // 2. Try build directory relative to application (for development)
        QString buildPath = QCoreApplication::applicationDirPath() + "/../mcp/transaction-agent/mcp-transaction-agent";
        if (QFile::exists(buildPath)) {
            m_agentPath = buildPath;
        }
    }
    
    if (m_agentPath.isEmpty()) {
        // 3. Try installed location from compile-time define
        QString installedPath = QStringLiteral(MCP_TRANSACTION_AGENT_PATH);
        if (QFile::exists(installedPath)) {
            m_agentPath = installedPath;
        }
    }
    
    if (m_agentPath.isEmpty()) {
        qWarning() << "mcp-transaction-agent executable not found in any location";
    } else {
        qInfo() << "Found transaction agent at:" << m_agentPath;
    }

    // Connect process signals
    connect(m_process, &QProcess::started,
            this, &TransactionAgentLauncher::started);
    
    connect(m_process, &QProcess::readyReadStandardOutput,
            this, &TransactionAgentLauncher::handleReadyReadStandardOutput);
    
    connect(m_process, &QProcess::readyReadStandardError,
            this, &TransactionAgentLauncher::handleReadyReadStandardError);
    
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &TransactionAgentLauncher::handleFinished);
    
    connect(m_process, &QProcess::errorOccurred,
            this, &TransactionAgentLauncher::handleError);
}

void TransactionAgentLauncher::installPackages(const QStringList& packages)
{
    QStringList args = {"install"};
    args.append(packages);
    launchAgent(args);
}

void TransactionAgentLauncher::removePackages(const QStringList& packages, bool force)
{
    QStringList args = {"remove"};
    if (force) {
        args << "--force";
    }
    args.append(packages);
    launchAgent(args);
}

void TransactionAgentLauncher::upgradeSystem(bool forceRefresh)
{
    QStringList args = {"upgrade"};
    if (forceRefresh) {
        args << "--refresh";
    }
    launchAgent(args);
}

bool TransactionAgentLauncher::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

void TransactionAgentLauncher::launchAgent(const QStringList& arguments)
{
    if (isRunning()) {
        qWarning() << "Transaction agent already running";
        Q_EMIT error("Transaction already in progress");
        return;
    }

    if (m_agentPath.isEmpty()) {
        qCritical() << "mcp-transaction-agent executable not found";
        Q_EMIT error("Transaction agent not found");
        return;
    }

    qInfo() << "Launching transaction agent:" << m_agentPath << arguments;
    
    m_process->setProgram(m_agentPath);
    m_process->setArguments(arguments);
    m_process->start();
}

void TransactionAgentLauncher::handleReadyReadStandardOutput()
{
    QString output = QString::fromUtf8(m_process->readAllStandardOutput());
    qDebug() << "Agent stdout:" << output;
    Q_EMIT outputReceived(output);
}

void TransactionAgentLauncher::handleReadyReadStandardError()
{
    QString output = QString::fromUtf8(m_process->readAllStandardError());
    qDebug() << "Agent stderr:" << output;
    Q_EMIT outputReceived(output);
}

void TransactionAgentLauncher::handleFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    bool success = (exitStatus == QProcess::NormalExit && exitCode == 0);
    
    qInfo() << "Transaction agent finished with exit code" << exitCode
            << "status" << (exitStatus == QProcess::NormalExit ? "normal" : "crash");
    
    Q_EMIT finished(success, exitCode);
}

void TransactionAgentLauncher::handleError(QProcess::ProcessError error)
{
    QString errorMsg;
    
    switch (error) {
    case QProcess::FailedToStart:
        errorMsg = "Failed to start transaction agent";
        break;
    case QProcess::Crashed:
        errorMsg = "Transaction agent crashed";
        break;
    case QProcess::Timedout:
        errorMsg = "Transaction agent timed out";
        break;
    case QProcess::WriteError:
    case QProcess::ReadError:
        errorMsg = "Communication error with transaction agent";
        break;
    default:
        errorMsg = "Unknown error with transaction agent";
        break;
    }
    
    qCritical() << "Process error:" << errorMsg;
    Q_EMIT this->error(errorMsg);
}

} // namespace mcp::qt::common
