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
    // Try multiple locations for agent executable
    m_agentPath = QStandardPaths::findExecutable(QStringLiteral("mcp-transaction-agent"));
    
    if (m_agentPath.isEmpty()) {
        // Build directory: bin/mcp-qt -> bin/mcp-transaction-agent
        QString buildPath = QCoreApplication::applicationDirPath() + QStringLiteral("/mcp-transaction-agent");
        if (QFile::exists(buildPath)) {
            m_agentPath = buildPath;
        }
    }
    
    if (m_agentPath.isEmpty()) {
        QString installedPath = QStringLiteral(MCP_TRANSACTION_AGENT_PATH);
        if (QFile::exists(installedPath)) {
            m_agentPath = installedPath;
        }
    }
    
    if (m_agentPath.isEmpty()) {
        qWarning() << "mcp-transaction-agent executable not found in any location";
        qWarning() << "  - Tried system PATH";
        qWarning() << "  - Tried build directory:" << QCoreApplication::applicationDirPath() + QStringLiteral("/mcp-transaction-agent");
        qWarning() << "  - Tried installed path:" << QStringLiteral(MCP_TRANSACTION_AGENT_PATH);
    } else {
        qInfo() << "Found transaction agent at:" << m_agentPath;
    }

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

TransactionAgentLauncher::~TransactionAgentLauncher()
{
    // Detach process on destruction to prevent "destroyed while running" warning
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->disconnect();
        m_process->setParent(nullptr);
        // Process will continue independently
    }
}

void TransactionAgentLauncher::launchCommand(const mcp::agent::Command& cmd)
{
    QStringList args;
    args << QString::fromStdString(cmd.operation);
    
    if (cmd.force && cmd.operation == "remove") {
        args << QStringLiteral("--force");
    }
    if (cmd.refresh && cmd.operation == "upgrade") {
        args << QStringLiteral("--refresh");
    }
    
    for (const auto& pkg : cmd.packages) {
        args << QString::fromStdString(pkg);
    }
    
    launchAgent(args);
}

void TransactionAgentLauncher::installPackages(const QStringList& packages)
{
    QStringList args = {QStringLiteral("install")};
    args.append(packages);
    launchAgent(args);
}

void TransactionAgentLauncher::removePackages(const QStringList& packages, bool force)
{
    QStringList args = {QStringLiteral("remove")};
    if (force) {
        args << QStringLiteral("--force");
    }
    args.append(packages);
    launchAgent(args);
}

void TransactionAgentLauncher::upgradeSystem(bool forceRefresh)
{
    QStringList args = {QStringLiteral("upgrade")};
    if (forceRefresh) {
        args << QStringLiteral("--refresh");
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
        Q_EMIT error(QStringLiteral("Transaction already in progress"));
        return;
    }

    if (m_agentPath.isEmpty()) {
        qCritical() << "mcp-transaction-agent executable not found";
        Q_EMIT error(QStringLiteral("Transaction agent not found"));
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
        errorMsg = QStringLiteral("Failed to start transaction agent");
        break;
    case QProcess::Crashed:
        errorMsg = QStringLiteral("Transaction agent crashed");
        break;
    case QProcess::Timedout:
        errorMsg = QStringLiteral("Transaction agent timed out");
        break;
    case QProcess::WriteError:
    case QProcess::ReadError:
        errorMsg = QStringLiteral("Communication error with transaction agent");
        break;
    default:
        errorMsg = QStringLiteral("Unknown error with transaction agent");
        break;
    }
    
    qCritical() << "Process error:" << errorMsg;
    Q_EMIT this->error(errorMsg);
}

} // namespace mcp::qt::common
