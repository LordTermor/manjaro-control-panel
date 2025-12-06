#include <qglobal.h>
#include <QDebug>
#include <QtQml/qqml.h>

#include "VersionInfo.h"

void initQmlResourceComponents()
{
    Q_INIT_RESOURCE(Components);
    
    // Register VersionInfo singleton
    qmlRegisterSingletonType<MCP::Qt::Common::VersionInfo>(
        "org.manjaro.mcp.components", 1, 0, "VersionInfo",
        [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new MCP::Qt::Common::VersionInfo();
        });
}