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

namespace mcp::cli::kernel {

using namespace mcp::kernel;

class RunningCommand : public Command {
public:
    [[nodiscard]] int execute() override {
        KernelProvider provider;
        auto result = provider.get_running_kernel();

        if (!result) {
            switch (result.error()) {
                case KernelError::DatabaseNotInitialized:
                    out().error("Failed to initialize package database.");
                    break;
                case KernelError::NotFound:
                    out().error("Could not determine running kernel.");
                    break;
                case KernelError::ParseError:
                    out().error("Failed to parse running kernel information.");
                    break;
            }
            return 1;
        }

        KernelFormatter::print_detail(*result);
        return 0;
    }
};

} // namespace mcp::cli::kernel
