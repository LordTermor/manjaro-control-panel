/* === This file is part of MCP ===
 *
 *   SPDX-FileCopyrightText: 2022-2025 Artem Grinev <agrinev@manjaro.org>
 *   SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include "command.hpp"
#include "kernel_formatter.hpp"
#include "common/output.hpp"

#include "kernel/KernelProvider.hpp"

#include <coro/sync_wait.hpp>

#include <fmt/core.h>

#include <string>

namespace mcp::cli::kernel {

using namespace mcp::kernel;

class InfoCommand : public Command {
    std::string m_package_name;

public:
    explicit InfoCommand(std::string package_name)
        : m_package_name(std::move(package_name)) {}

    [[nodiscard]] int execute() override {
        KernelProvider provider;
        auto result = coro::sync_wait(provider.get_kernel(m_package_name));

        if (!result) {
            switch (result.error()) {
                case KernelError::DatabaseNotInitialized:
                    out().error("Failed to initialize package database.");
                    break;
                case KernelError::NotFound:
                    out().error(fmt::format("Kernel '{}' not found.", m_package_name));
                    break;
                case KernelError::ParseError:
                    out().error("Failed to parse kernel information.");
                    break;
            }
            return 1;
        }

        KernelFormatter::print_detail(*result);
        return 0;
    }
};

} // namespace mcp::cli::kernel
