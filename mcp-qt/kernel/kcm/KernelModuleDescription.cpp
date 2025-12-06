/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include "KernelModuleDescription.h"

void initResources()
{
    Q_INIT_RESOURCE(kernel);
}

void mcp::qt::kernel::KernelPlugin::registerTypes(const char *uri)
{
    initResources();

    // @uri Kernel
    static int t1 = qmlRegisterUncreatableType<mcp::qt::kernel::KernelViewModel>(
        uri, 1, 0, "ViewModel", "You can't create view model, use `vm` context property instead");

    static int t2 = qmlRegisterUncreatableType<mcp::qt::kernel::KernelListModel>(
        uri,
        1,
        0,
        "ListModel",
        "You can't create list model, use `vm.model` context property instead");

    qmlRegisterUncreatableType<mcp::qt::common::ProgressNotifier>(
        uri,
        1,
        0,
        "ProgressNotifier",
        "You can't create view model, use `vm` context property instead");
}
