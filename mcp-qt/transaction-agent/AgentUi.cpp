/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "AgentUi.h"

#include <pamac/database.hpp>
#include <pamac/transaction.hpp>

#include <QDebug>
#include <QPushButton>

namespace mcp::agent {

AgentUi::AgentUi(QWidget* parent)
    : QWidget(parent)
    , m_database(pamac::Database::instance().value().get())
{
    setWindowTitle(QStringLiteral("MCP Transaction Agent"));
    setMinimumSize(600, 150);
    resize(600, 150); // Start compact

    // Setup UI
    auto* layout = new QVBoxLayout(this);

    m_statusLabel = new QLabel(QStringLiteral("Initializing..."), this);
    m_statusLabel->setStyleSheet(QStringLiteral("font-weight: bold; font-size: 14px;"));
    layout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    layout->addWidget(m_progressBar);

    m_detailsButton = new QPushButton(QStringLiteral("Show Details"), this);
    connect(m_detailsButton, &QPushButton::clicked, this, &AgentUi::toggleDetails);
    layout->addWidget(m_detailsButton);

    m_logView = new QTextEdit(this);
    m_logView->setReadOnly(true);
    m_logView->setStyleSheet(QStringLiteral("font-family: monospace;"));
    m_logView->setVisible(false); // Hidden by default
    layout->addWidget(m_logView);

    setLayout(layout);
}

void AgentUi::startTransaction(
    const QString& operation,
    const QStringList& packages,
    bool force,
    bool refresh)
{
    logMessage(QStringLiteral("Starting %1 operation").arg(operation));
    logMessage(QStringLiteral("Packages: %1").arg(packages.join(QStringLiteral(", "))));

    if (operation == QStringLiteral("install")) {
        runInstall(packages);
    } else if (operation == QStringLiteral("remove")) {
        runRemove(packages, force);
    } else if (operation == QStringLiteral("upgrade")) {
        runUpgrade(refresh);
    } else {
        logMessage(QStringLiteral("ERROR: Unknown operation: ") + operation);
        Q_EMIT transactionFinished(false);
    }
}

QCoro::Task<void> AgentUi::runInstall(const QStringList& packages)
{
    m_statusLabel->setText(QStringLiteral("Installing packages..."));
    
    pamac::Transaction txn(m_database);
    mcp::ProgressFlattener flattener;
    flattener.connect_to_transaction(txn);
    connectTransactionSignals(txn, flattener);

    // Add packages
    for (const auto& pkg : packages) {
        logMessage(QStringLiteral("Adding to install: ") + pkg);
        txn.add_pkg_to_install(pkg.toStdString());
    }

    // Get authorization (done automatically by libpamac)
    logMessage(QStringLiteral("Getting authorization..."));
    bool authorized = co_await txn.get_authorization_async();
    if (!authorized) {
        logMessage(QStringLiteral("ERROR: Authorization denied"));
        Q_EMIT transactionFinished(false);
        co_return;
    }

    // Run transaction
    logMessage(QStringLiteral("Running transaction..."));
    bool success = co_await txn.run_async();
    
    txn.remove_authorization();

    if (success) {
        m_progressBar->setValue(100);
        m_statusLabel->setText(QStringLiteral("Installation completed successfully"));
        logMessage(QStringLiteral("SUCCESS: Transaction completed"));
    } else {
        m_statusLabel->setText(QStringLiteral("Installation failed"));
        logMessage(QStringLiteral("ERROR: Transaction failed"));
    }

    Q_EMIT transactionFinished(success);
}

QCoro::Task<void> AgentUi::runRemove(const QStringList& packages, bool force)
{
    m_statusLabel->setText(QStringLiteral("Removing packages..."));
    
    pamac::Transaction txn(m_database);
    mcp::ProgressFlattener flattener;
    flattener.connect_to_transaction(txn);
    connectTransactionSignals(txn, flattener);

    // Add packages
    for (const auto& pkg : packages) {
        logMessage(QStringLiteral("Adding to remove: %1 %2")
                       .arg(pkg)
                       .arg(force ? QStringLiteral("(forced)") : QStringLiteral("")));
        txn.add_pkg_to_remove(pkg.toStdString());
    }

    // Get authorization
    logMessage(QStringLiteral("Getting authorization..."));
    bool authorized = co_await txn.get_authorization_async();
    if (!authorized) {
        logMessage(QStringLiteral("ERROR: Authorization denied"));
        Q_EMIT transactionFinished(false);
        co_return;
    }

    // Run transaction
    logMessage(QStringLiteral("Running transaction..."));
    bool success = co_await txn.run_async();
    
    txn.remove_authorization();

    if (success) {
        m_progressBar->setValue(100);
        m_statusLabel->setText(QStringLiteral("Removal completed successfully"));
        logMessage(QStringLiteral("SUCCESS: Transaction completed"));
    } else {
        m_statusLabel->setText(QStringLiteral("Removal failed"));
        logMessage(QStringLiteral("ERROR: Transaction failed"));
    }

    Q_EMIT transactionFinished(success);
}

QCoro::Task<void> AgentUi::runUpgrade(bool refresh)
{
    m_statusLabel->setText(QStringLiteral("Upgrading system..."));
    
    pamac::Transaction txn(m_database);
    mcp::ProgressFlattener flattener;
    flattener.connect_to_transaction(txn);
    connectTransactionSignals(txn, flattener);

    logMessage(QStringLiteral("System upgrade %1").arg(refresh ? QStringLiteral("(forced refresh)") : QStringLiteral("")));
    txn.add_pkgs_to_upgrade(refresh);

    // Get authorization
    logMessage(QStringLiteral("Getting authorization..."));
    bool authorized = co_await txn.get_authorization_async();
    if (!authorized) {
        logMessage(QStringLiteral("ERROR: Authorization denied"));
        Q_EMIT transactionFinished(false);
        co_return;
    }

    // Run transaction
    logMessage(QStringLiteral("Running transaction..."));
    bool success = co_await txn.run_async();
    
    txn.remove_authorization();

    if (success) {
        m_progressBar->setValue(100);
        m_statusLabel->setText(QStringLiteral("Upgrade completed successfully"));
        logMessage(QStringLiteral("SUCCESS: Transaction completed"));
    } else {
        m_statusLabel->setText(QStringLiteral("Upgrade failed"));
        logMessage(QStringLiteral("ERROR: Transaction failed"));
    }

    Q_EMIT transactionFinished(success);
}

void AgentUi::connectTransactionSignals(pamac::Transaction& txn, mcp::ProgressFlattener& flattener)
{
    // Bridge sigc++ signals to Qt using lambdas that capture this
    flattener.signal_progress_changed.connect([this](double progress) {
        m_progressBar->setValue(static_cast<int>(progress * 100));
    });

    flattener.signal_phase_changed.connect([this](const std::string& phase) {
        QString phaseStr = QString::fromStdString(phase);
        logMessage(QStringLiteral("Phase: ") + phaseStr);
        m_statusLabel->setText(phaseStr);
    });

    // Use flattener's deduplicated details
    flattener.signal_details_changed.connect([this](const std::string& details) {
        if (!details.empty()) {
            logMessage(QStringLiteral("  ") + QString::fromStdString(details));
        }
    });

    // Action signals for status updates
    txn.signal_emit_action.connect([this](const std::string& action) {
        QString actionStr = QString::fromStdString(action);
        logMessage(QStringLiteral("Action: ") + actionStr);
        m_statusLabel->setText(actionStr);
    });

    // Error signal
    txn.signal_emit_error.connect(
        [this](const std::string& message, const std::vector<std::string>& details) {
            logMessage(QStringLiteral("ERROR: ") + QString::fromStdString(message));
            for (const auto& detail : details) {
                logMessage(QStringLiteral("  - ") + QString::fromStdString(detail));
            }
        });
}

void AgentUi::logMessage(const QString& msg)
{
    m_logView->append(msg);
    qInfo() << msg;
}

void AgentUi::updateProgress(const QString& action, double progress)
{
    m_progressBar->setValue(static_cast<int>(progress * 100));
    m_statusLabel->setText(action);
}

void AgentUi::toggleDetails()
{
    bool isVisible = m_logView->isVisible();
    m_logView->setVisible(!isVisible);
    m_detailsButton->setText(isVisible ? QStringLiteral("Show Details") : QStringLiteral("Hide Details"));
    
    // Adjust window size when toggling
    if (!isVisible) {
        // Expanding - make window larger
        resize(width(), 500);
    } else {
        // Collapsing - make window smaller
        resize(width(), 150);
    }
}

} // namespace mcp::agent
