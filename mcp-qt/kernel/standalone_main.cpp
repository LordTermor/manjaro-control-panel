/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Standalone entry point for the Kernel module.
 * Allows running and debugging without the full KCM infrastructure.
 */

#include "KernelListModel.h"
#include "KernelViewModel.h"
#include "VersionInfo.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include <pamac/database.hpp>

using namespace mcp::qt::kernel;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("MCP Kernel Manager");
    app.setOrganizationName("Manjaro");
    app.setApplicationVersion(MCP_VERSION);
   

    // Initialize pamac database
    auto status = pamac::Database::initialize("/etc/pamac.conf");
    if (status != pamac::DatabaseStatus::Ok &&
        status != pamac::DatabaseStatus::AlreadyInitialized) {
        qCritical() << "Failed to initialize package database";
        return 1;
    }

    qmlRegisterSingletonType<mcp::qt::common::VersionInfo>(
        "org.manjaro.mcp.components", 1, 0, "VersionInfo",
        [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new mcp::qt::common::VersionInfo();
        });

    // Create model and view model
    KernelListModel model;
    KernelViewModel viewModel(model);

    QQmlApplicationEngine engine;

    // Expose the view model to QML
    engine.rootContext()->setContextProperty("vm", &viewModel);

    // Load the standalone QML
    engine.load(QUrl("qrc:/kernel-page/ui/standalone_main.qml"));

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML";
        return 1;
    }

    return app.exec();
}
