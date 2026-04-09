/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Unified shell entry point combining kernel and hardware management modules
 * into a single Kirigami application with sidebar navigation.
 */

#include "KernelListModel.h"
#include "KernelViewModel.h"
#include "MhwdViewModel.h"
#include "VersionInfo.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include <pamac/database.hpp>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Manjaro Control Panel"));
    app.setOrganizationName(QStringLiteral("Manjaro"));
    app.setApplicationVersion(QString::fromLatin1(MCP_VERSION));
    app.setDesktopFileName(QStringLiteral("mcp-qt"));

    auto status = pamac::Database::initialize("/etc/pamac.conf");
    if (status != pamac::DatabaseStatus::Ok
        && status != pamac::DatabaseStatus::AlreadyInitialized) {
        qCritical() << QStringLiteral("Failed to initialize package database");
        return 1;
    }

    // Kernel module
    mcp::qt::kernel::KernelListModel kernelListModel;
    mcp::qt::kernel::KernelViewModel kernelViewModel(kernelListModel);

    // MHWD module — registered as QML type, instantiated from QML
    qmlRegisterType<mcp::qt::mhwd::MhwdViewModel>(
        "org.manjaro.mcp.mhwd", 1, 0, "MhwdViewModel");

    qmlRegisterSingletonType<mcp::qt::common::VersionInfo>(
        "org.manjaro.mcp.components", 1, 0, "VersionInfo",
        [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new mcp::qt::common::VersionInfo();
        });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("vm"), &kernelViewModel);

    engine.load(QUrl(QStringLiteral("qrc:/shell/ui/main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qCritical() << QStringLiteral("Failed to load QML");
        return 1;
    }

    return app.exec();
}
