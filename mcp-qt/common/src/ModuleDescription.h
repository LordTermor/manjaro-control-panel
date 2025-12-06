/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <modulemanager/ModuleDescription.h>

#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QWidget>
#include <QtQml/QQmlExtensionPlugin>

namespace mcp::qt::common {
namespace {
namespace Modules = mcp::lib::common::Modules;
}

template<const char Name[], const char Description[], const char Icon[], typename TWidget>
class LegacyModuleDescription : public Modules::ModuleDescription<Name, Description, Icon>
{
public:
    static void inject(QWidget *wgt, kgr::container &container)
    {
        //do nothing
    }

    static void register_dependencies(kgr::container &container)
    {
        //do nothing
    }

    static QWidget *create_view() { return new TWidget; }
    static void init(QWidget *wgt)
    {
        //do nothing
    }
};

template<typename T>
concept ExtensionPlugin = requires(T, const char uri[], kgr::container &container)
{
    T::registerTypes(uri);
    T::registerDependencies(container);
};

template<const char ResourceUrl[],
         const char Name[],
         const char Description[],
         const char Icon[],
         ExtensionPlugin TPlugin,
         typename TViewModel>
class QmlModuleDescription : public Modules::ModuleDescription<Name, Description, Icon>
{
public:
    static void inject(QWidget *wgt, kgr::container &container)
    {
        auto &vm = container.service<TViewModel>();

        qobject_cast<QQuickWidget *>(wgt)->rootContext()->setContextProperty("vm",
                                                                             QVariant::fromValue(
                                                                                 &vm));
    }

    static void register_dependencies(kgr::container &container)
    {
        TPlugin::registerDependencies(container);
    }

    static QWidget *create_view() { return new QQuickWidget; }

    static void init(QWidget *wgt)
    {
        TPlugin::registerTypes(Name);
        qobject_cast<QQuickWidget *>(wgt)->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qobject_cast<QQuickWidget *>(wgt)->setSource(QUrl(QString::fromUtf8(ResourceUrl)));
    }
};

inline QVariantMap mapModuleMetaInfo(const Modules::ModuleMetaInfo &info)
{
    return {{"name", QString::fromStdString(info.name)},
            {"description", QString::fromStdString(info.description)},
            {"icon", QString::fromStdString(info.icon)}};
}

} // namespace mcp::qt::common
