/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Standalone entry point for the MHWD module.
 * Allows running and debugging without the full KCM infrastructure.
 */

#include "MhwdViewModel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

using namespace mcp::qt::mhwd;

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("MCP Hardware Configuration"));
    app.setOrganizationName(QStringLiteral("Manjaro"));

    qmlRegisterType<MhwdViewModel>("org.manjaro.mcp.mhwd", 1, 0, "MhwdViewModel");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/mhwd/ui/standalone_main.qml")));

    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML";
        return 1;
    }

    return app.exec();
}
