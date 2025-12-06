/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "../KernelViewModel.h"

#include <ModuleDescription.h>

#include <QtQml/QQmlExtensionPlugin>

namespace mcp::qt::kernel {

class KernelPlugin
{
public:
    static void registerTypes(const char *uri);
};

static constexpr char ModuleUrl[] = "qrc:/kernel-page/package/contents/ui/View.qml";
static constexpr char ModuleName[] = "Kernel";
static constexpr char ModuleDescription[] = "Description";
static constexpr char ModuleIcon[] = "preferences-system-linux";

using KernelModuleDescription = mcp::qt::common::QmlModuleDescription<ModuleUrl,
                                                                      ModuleName,
                                                                      ModuleDescription,
                                                                      ModuleIcon,
                                                                      KernelPlugin>;
} // namespace mcp::qt::kernel
