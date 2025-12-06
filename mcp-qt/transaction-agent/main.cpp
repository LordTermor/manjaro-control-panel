/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * MCP Transaction Agent - standalone transaction executor.
 * 
 * Simple Qt Widgets app that:
 * - Takes operation (install/remove/upgrade) and packages as command-line args
 * - Shows progress in simple UI
 * - Exits with code 0 (success) or 1 (failure)
 * 
 * Usage:
 *   mcp-transaction-agent install linux66 linux66-headers
 *   mcp-transaction-agent remove linux515
 *   mcp-transaction-agent upgrade
 */

#include "AgentUi.h"

#include <pamac/database.hpp>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QTimer>

using namespace mcp::agent;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setApplicationName("MCP Transaction Agent");
    app.setOrganizationName("Manjaro");
    app.setApplicationVersion(MCP_VERSION);

    // Parse command line
    QCommandLineParser parser;
    parser.setApplicationDescription("MCP package transaction executor");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("operation", "Operation: install, remove, or upgrade");
    parser.addPositionalArgument("packages", "Package names (for install/remove)", "[packages...]");

    parser.addOption({{"f", "force"}, "Force operation (e.g., remove running kernel)"});
    parser.addOption({{"r", "refresh"}, "Force refresh before upgrade"});

    parser.process(app);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        qCritical() << "Error: No operation specified";
        parser.showHelp(1);
    }

    QString operation = args.takeFirst();
    QStringList packages = args;

    bool force = parser.isSet("force");
    bool refresh = parser.isSet("refresh");

    // Validate operation
    if (operation == "install" || operation == "remove") {
        if (packages.isEmpty()) {
            qCritical() << "Error: No packages specified for" << operation;
            return 1;
        }
    } else if (operation != "upgrade") {
        qCritical() << "Error: Unknown operation:" << operation;
        parser.showHelp(1);
    }

    qInfo() << "Transaction agent starting:" << operation << packages;

    // Initialize pamac database
    auto db_status = pamac::Database::initialize("/etc/pamac.conf");
    if (db_status != pamac::DatabaseStatus::Ok && 
        db_status != pamac::DatabaseStatus::AlreadyInitialized) {
        qCritical() << "Failed to initialize pamac database";
        return 1;
    }

    auto db_result = pamac::Database::instance();
    if (!db_result) {
        qCritical() << "Failed to get pamac database instance";
        return 1;
    }

    // Create UI
    AgentUi ui;
    ui.show();

    // Track result
    bool success = false;
    QObject::connect(&ui, &AgentUi::transactionFinished,
        [&app, &success](bool result) {
            success = result;
            qInfo() << "Transaction finished:" << (success ? "SUCCESS" : "FAILED");
            // Give UI a moment to update before quitting
            QTimer::singleShot(1000, &app, &QApplication::quit);
        });

    // Start transaction
    ui.startTransaction(operation, packages, force, refresh);

    // Run event loop
    app.exec();

    return success ? 0 : 1;
}
